#include <vector>
#include <iostream>
#include <fstream>
#include "contraints.graph.h"
#include "contraints.h"
#include "variable.h"
#include "csp.h"
#include <cmath>
#include <cstdlib>
#include <ctime>


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

//global function 
std::vector<int> getVector(int val, ...) {
	std::vector<int> result;

	va_list valist;
	int arg;
	va_start(valist, val);
	result.push_back(val);
	while ((arg = va_arg(valist, int)) != 0) {
		result.push_back(arg);
	}
	va_end(valist);
	return result;
}


#ifdef EXAMPLE
int main () {
	std::vector<int> range = getVector(0,1,2,3,NULL);
	Variable x ( "x", range );
	Variable y ( "y", range );
	Variable z ( "z", range );


	SumEqual<Variable,8> c1(&x,&y,&z,NULL);
	SumEqual<Variable,5> c2(&x,&z,NULL);
	SumEqual<Variable,8> c4(&x,&z,NULL); //x+z cannot be 8
	std::cout << c1;
	std::cout << c2;
	std::cout << c4;
	std::cout << x;
	std::cout << y;
	std::cout << z;

	if ( c1.Satisfiable() ) std::cout << "c1 is Satisfiable\n"; 
	else std::cout << "c1 is not Satisfiable\n";
	if ( c2.Satisfiable() ) std::cout << "c2 is Satisfiable\n"; 
	else std::cout << "c2 is not Satisfiable\n";
	if ( c4.Satisfiable() ) std::cout << "c4 is Satisfiable\n"; 
	else std::cout << "c4 is not Satisfiable\n";

	std::cout << "================\n";

	AllDiff<Variable> c3(&x,&y,&z,NULL);
	std::cout << c3;
	if ( c3.Satisfiable() ) std::cout << "c3 is Satisfiable\n"; 
	else std::cout << "c3 is not Satisfiable\n";

	std::cout << "Let's assign values to variables\n";
	x.Assign(3);
	y.Assign(2);
	z.Assign(2);
	std::cout << x << y << z;
	if ( c3.Satisfiable() ) std::cout << "c3 is Satisfiable\n"; 
	else std::cout << "c3 is not Satisfiable\n";

	std::vector<int> range01 = getVector(0,1,NULL);

	Variable u ( "u", range01 );
	Variable v ( "v", range01 );
	Variable w ( "w", range01 );
	Variable w2 ( "w2", getVector(0,1,2,NULL) );

	AllDiff<Variable> c5(&u,&v,&w,NULL);
	AllDiff<Variable> c6(&u,&v,&w2,NULL);
	std::cout << c5 << u << v << w;

	if ( c5.Satisfiable() ) std::cout << "c5 is Satisfiable\n"; 
	else std::cout << "c5 is not Satisfiable\n";

	std::cout << c6 << u << v << w2;
	if ( c6.Satisfiable() ) std::cout << "c6 is Satisfiable\n"; 
	else std::cout << "c6 is not Satisfiable\n";

	try {
		ConstraintGraph<Constraint<Variable> > cg;
		cg.InsertVariable(u);
		cg.InsertVariable(v);
		cg.InsertVariable(w);
		cg.InsertVariable(w2);
		cg.InsertConstraint(c5);
		cg.InsertConstraint(c6);
		cg.Print();
		cg.PreProcess();

		std::set<Variable*> neighbors = cg.GetNeighbors(&w2);
		std::set<Variable*>::const_iterator b_neighbor = neighbors.begin();
		std::set<Variable*>::const_iterator e_neighbor = neighbors.end();
		std::cout << "Neighbors of " << w2 << " are \n";
		for ( ; b_neighbor!=e_neighbor; ++b_neighbor ) {
			std::cout << "--- " << **b_neighbor;
		}

		CSP<ConstraintGraph<Constraint<Variable> > > csp( cg );

#ifdef DFS
		if ( 
				csp.SolveDFS(0)
		   ) {
			cg.Print();
		}
		else {
			std::cout << "No solution found\n";
		}
#else
		std::cout << "Skip SolveDFS\n";
#endif
	} catch ( const char * msg ) {
		std::cout << msg << std::endl;
	}
}
#endif

#ifdef SIMPLE
int main () {
	std::vector<int> range = getVector(0,1,2,3,NULL);
	Variable x ( "x", range );
	Variable y ( "y", range );
	Variable z ( "z", range );


	SumEqual<Variable,8> c1(&x,&y,&z,NULL);
	SumEqual<Variable,5> c2(&x,&z,NULL);
	ConstraintGraph<Constraint<Variable> > cg;
	cg.InsertVariable(x);
	cg.InsertVariable(y);
	cg.InsertVariable(z);

	cg.InsertConstraint(c1);
	cg.InsertConstraint(c2);
	//cg.Print();
	cg.PreProcess();

	CSP<ConstraintGraph<Constraint<Variable> > > csp( cg );
	clock_t start = std::clock();
	if ( 
#ifdef ARC
			csp.SolveARC(0) 
#endif
#ifdef FC
			csp.SolveFC(0) 
#endif
#ifdef DFS
			csp.SolveDFS(0) 
#endif
	   ) {
		cg.Print();
		clock_t finish = std::clock();
		std::cout << "Time " << static_cast<float>(finish-start)/CLOCKS_PER_SEC << std::endl;
		std::cout << "RecursiveCallCounter = " << csp.GetRecursiveCallCounter() << std::endl;
		std::cout << "IterationCounter     = " << csp.GetIterationCounter() << std::endl;
	}
	else {
		std::cout << "No solution found\n";
	}
}
#endif



#ifdef MSBC
int main () try { //magic square SIZExSIZE
	const int magic_constant = (SIZE*SIZE*SIZE + SIZE ) /2;
	const int NUM_VARIABLES  = SIZE*SIZE;
	//SIZE rows + SIZE columns + 2 diagonals + SIZE*(SIZE-1)/2 different pairs
	const int NUM_CONSTRAINTS  = 2*SIZE + 2 + NUM_VARIABLES*(NUM_VARIABLES-1)/2;

	std::vector<int> range;
	for (int i=0;i<NUM_VARIABLES;++i) { range.push_back(i+1); } //1,....,SIZE^2

	Variable* variables[ NUM_VARIABLES ];
	Constraint<Variable>* constraints[ NUM_CONSTRAINTS ];
	ConstraintGraph<Constraint<Variable> > cg;

	for (int i=0;i<NUM_VARIABLES;++i) {
		char name[5];
		sprintf(name,"x%i",i);
		variables[i] = new Variable ( name, range );
		cg.InsertVariable(*variables[i]);
	}

	unsigned index = 0;

	//all different as a set of binary constraints (same as QUEEN)
	for (int i=0; i<NUM_VARIABLES-1; ++i) {
		for (int j=i+1; j<NUM_VARIABLES; ++j) {
			constraints[index] = 
				new AllDiff2<Variable>( variables[i], variables[j] );
			//constraints[index] = new AllDiff2<Variable>();
			//constraints[index]->AddVariable( variables[ i ] );
			//constraints[index]->AddVariable( variables[ j ] );
			cg.InsertConstraint( *constraints[index] );
			++index;
		}
	}

	for (unsigned i=0;i<SIZE;++i) {
		//row
		constraints[index] = new SumEqual<Variable,magic_constant>();
		for (unsigned j=0;j<SIZE;++j) { constraints[index]->AddVariable( variables[ i*SIZE +j ] ); }
		cg.InsertConstraint( *constraints[index] );
		++index;
		//column
		constraints[index] = new SumEqual<Variable,magic_constant>();
		for (unsigned j=0;j<SIZE;++j) { constraints[index]->AddVariable( variables[ j*SIZE +i ] ); }
		cg.InsertConstraint( *constraints[index] );
		++index;
	}
	//main diagonal
	constraints[index] = new SumEqual<Variable,magic_constant>();
	for (unsigned i=0;i<SIZE;++i) { 
		constraints[index]->AddVariable( variables[ (SIZE+1)*i ] ); 
	}
	cg.InsertConstraint( *constraints[index] );
	++index;
	//secondary diagonal
	constraints[index] = new SumEqual<Variable,magic_constant>();
	for (unsigned i=0;i<SIZE;++i) { 
		constraints[index]->AddVariable( variables[ SIZE-1 + (SIZE-1)*i ] ); 
	}
	cg.InsertConstraint( *constraints[index] );
	++index;

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	//cg.Print();
	cg.PreProcess();

	CSP<ConstraintGraph<Constraint<Variable> > > csp( cg );
	clock_t start = std::clock();
	if ( 
#ifdef ARC
			csp.SolveARC(0) 
#endif
#ifdef FC
			csp.SolveFC(0) 
#endif
#ifdef DFS
			csp.SolveDFS(0) 
#endif
#ifdef DFScount
			csp.SolveFC_count(0)
#endif
	   ) {
		//cg.Print();
		clock_t finish = std::clock();
#ifdef DFScount
			std::cout << "Solution counter = " 
				<< csp.GetSolutionCounter() << std::endl;
#else
		std::cout << "Time " << static_cast<float>(finish-start)/CLOCKS_PER_SEC << std::endl;
		std::cout << "RecursiveCallCounter = " << csp.GetRecursiveCallCounter() << std::endl;
		std::cout << "IterationCounter     = " << csp.GetIterationCounter() << std::endl;

		for (unsigned i=0;i<SIZE;++i) {
			for (unsigned j=0;j<SIZE;++j) {
				std::cout << variables[ i*SIZE+j ]->GetValue() << "   ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
#endif
	}
	else std::cout << "No solution found\n";

	for (int i=0;i<NUM_CONSTRAINTS;++i) delete constraints[i];

	for (int i=0;i<NUM_VARIABLES;++i) { delete variables[i]; }
} catch ( const char * msg ) {
	std::cout << msg << std::endl;
}
#endif

#ifdef MS
int main () try {//magic square SIZExSIZE
	const int magic_constant = (SIZE*SIZE*SIZE + SIZE ) /2;
	const int NUM_VARIABLES  = SIZE*SIZE;
	//SIZE rows + SIZE columns + 2 diagonals + 1 all different
	const int NUM_CONSTRAINTS  = 2*SIZE + 2 + 1;

	std::vector<int> range;
	for (int i=0;i<NUM_VARIABLES;++i) { range.push_back(i+1); } //1,....,SIZE^2

	Variable* variables[ NUM_VARIABLES ];
	Constraint<Variable>* constraints[ NUM_CONSTRAINTS ];
	ConstraintGraph<Constraint<Variable> > cg;

	for (int i=0;i<NUM_VARIABLES;++i) {
		char name[5];
		sprintf(name,"x%i",i);
		variables[i] = new Variable ( name, range );
		cg.InsertVariable(*variables[i]);
	}

	unsigned index = 0;

	for (unsigned i=0;i<SIZE;++i) {
		//row
		constraints[index] = new SumEqual<Variable,magic_constant>();
		for (unsigned j=0;j<SIZE;++j) { constraints[index]->AddVariable( variables[ i*SIZE +j ] ); }
		cg.InsertConstraint( *constraints[index] );
		++index;
		//column
		constraints[index] = new SumEqual<Variable,magic_constant>();
		for (unsigned j=0;j<SIZE;++j) { constraints[index]->AddVariable( variables[ j*SIZE +i ] ); }
		cg.InsertConstraint( *constraints[index] );
		++index;
	}
	//main diagonal
	constraints[index] = new SumEqual<Variable,magic_constant>();
	for (unsigned i=0;i<SIZE;++i) { 
		constraints[index]->AddVariable( variables[ (SIZE+1)*i ] ); 
	}
	cg.InsertConstraint( *constraints[index] );
	++index;
	//secondary diagonal
	constraints[index] = new SumEqual<Variable,magic_constant>();
	for (unsigned i=0;i<SIZE;++i) { 
		constraints[index]->AddVariable( variables[ SIZE-1 + (SIZE-1)*i ] ); 
	}
	cg.InsertConstraint( *constraints[index] );
	++index;

	//all different - single constraint
	constraints[index] = new AllDiff<Variable>();
	for (int j=0;j<NUM_VARIABLES;++j) { 
		constraints[index]->AddVariable( variables[ j ] ); 
	}
	cg.InsertConstraint( *constraints[index] );


	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	//cg.Print();
	cg.PreProcess();

	CSP<ConstraintGraph<Constraint<Variable> > > csp( cg );
	clock_t start = std::clock();
	if ( 
#ifdef ARC
			csp.SolveARC(0) 
#endif
#ifdef FC
			csp.SolveFC(0) 
#endif
#ifdef DFS
			csp.SolveDFS(0) 
#endif
#ifdef DFScount
			csp.SolveFC_count(0)
#endif
	   ) {
		clock_t finish = std::clock();
		//cg.Print();
#ifdef DFScount
		std::cout << "Solution counter = " 
			<< csp.GetSolutionCounter() << std::endl;
#else
		std::cout << "Time " << static_cast<float>(finish-start)/CLOCKS_PER_SEC << std::endl;
		std::cout << "RecursiveCallCounter = " << csp.GetRecursiveCallCounter() << std::endl;
		std::cout << "IterationCounter     = " << csp.GetIterationCounter() << std::endl;

		for (unsigned i=0;i<SIZE;++i) {
			for (unsigned j=0;j<SIZE;++j) {
				std::cout << variables[ i*SIZE+j ]->GetValue() << "   ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
#endif
	}
	else std::cout << "No solution found\n";

	for (int i=0;i<NUM_CONSTRAINTS;++i) delete constraints[i];
	for (int i=0;i<NUM_VARIABLES;++i) { delete variables[i]; }
} catch ( const char * msg ) {
	std::cout << msg << std::endl;
}
#endif

#ifdef QUEEN
int main () {
	//n-queen
	//x_1,...,x_n - rows for queens 1,...,n
	//x_i is in column i
	//AllDiff(x_1,...,x_n)
	//|x_i - x_j| != |i-j|
	try {
		ConstraintGraph<Constraint<Variable> > cg;

		std::vector<int> range;
		for (int i=0;i<SIZE;++i) { range.push_back(i); }

		Variable ** array_of_variables = new Variable* [SIZE];
		for (int i=0;i<SIZE;++i) {
			char name[5];
			sprintf(name,"x%i",i);
			array_of_variables[i] = new Variable ( name, range );
			cg.InsertVariable(*array_of_variables[i]);
		}


		unsigned index = 0;
		Constraint<Variable>* array_of_constraints[SIZE*(SIZE-1)];
		for (unsigned i=0;i<SIZE-1;++i) {
			for (unsigned j=i+1;j<SIZE;++j) {
				//|xi-xj| != |j-i|
				array_of_constraints[index] = 
					new DifferenceNotEqual<Variable>(j-i,array_of_variables[i],array_of_variables[j],NULL);
				cg.InsertConstraint( *array_of_constraints[index] );
				++index;
				array_of_constraints[index] = 
					new 
					AllDiff2<Variable>(array_of_variables[i],array_of_variables[j]);
				cg.InsertConstraint( *array_of_constraints[index] );
				++index;
			}
		}

		//cg.Print();
		cg.PreProcess();

		CSP<ConstraintGraph<Constraint<Variable> > > csp( cg );
		clock_t start = std::clock();
		if ( 
#ifdef ARC
				csp.SolveARC(0) 
#endif
#ifdef FC
				csp.SolveFC(0) 
#endif
#ifdef DFS
				csp.SolveDFS(0) 
#endif
#ifdef DFScount
				csp.SolveFC_count(0)
#endif
		   ) {
			//cg.Print();
			clock_t finish = std::clock();
#ifdef DFScount
			std::cout << "Solution counter = " 
				<< csp.GetSolutionCounter() << std::endl;
#else

			for (int i=0;i<SIZE;++i) {
				for (int j=0;j<SIZE;++j) {
					if ( j == array_of_variables[ i ]->GetValue() ) std::cout << " Q ";
					else std::cout << " . ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
#endif
			std::cout << "Time " << static_cast<float>(finish-start)/CLOCKS_PER_SEC << std::endl;
			std::cout << "RecursiveCallCounter = " 
				<< csp.GetRecursiveCallCounter() << std::endl;
			std::cout << "IterationCounter     = " 
				<< csp.GetIterationCounter() << std::endl;

		}
		else std::cout << "No solution found\n";

		for (int i=0;i<SIZE*(SIZE-1);++i) delete array_of_constraints[i];

		for (int i=0;i<SIZE;++i) { delete array_of_variables[i]; }
		delete [] array_of_variables;
	}
	catch ( const char * msg ) {
		std::cout << msg << std::endl;
	}

}
#endif
