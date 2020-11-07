#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include "contraints.graph.h"
#include "contraints.h"
#include "variable.h"
#include "csp.h"

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

enum ALG       { DFS, FC, ARC};
enum RET_VALUE { CORRECTNESS, ITERATIONS, CALLS, TIME};

int queens (const unsigned size, ALG alg, RET_VALUE ret_value) try {

	ConstraintGraph<Constraint<Variable> > cg;

	std::vector<int> range;
	for (unsigned i=0;i<size;++i) { range.push_back(i); }

	Variable ** array_of_variables = new Variable* [size];
	for (unsigned i=0;i<size;++i) {
		char name[5];
		sprintf(name,"x%i",i);
		array_of_variables[i] = new Variable ( name, range );
		cg.InsertVariable(*array_of_variables[i]);
	}


	unsigned index = 0;
	std::vector< Constraint<Variable>*> array_of_constraints (size*(size-1));
	for (unsigned i=0;i<size-1;++i) {
		for (unsigned j=i+1;j<size;++j) {
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

	cg.PreProcess();

	CSP<ConstraintGraph<Constraint<Variable> > > csp( cg );

	typedef bool (CSP<ConstraintGraph<Constraint<Variable> > >::*Csp_mem_fun)(unsigned);

	Csp_mem_fun algs[3] = { 
		&CSP<ConstraintGraph<Constraint<Variable> > >::SolveDFS, 
		&CSP<ConstraintGraph<Constraint<Variable> > >::SolveFC, 
		&CSP<ConstraintGraph<Constraint<Variable> > >::SolveARC  };

	int attacks = 0;
	int num_unassigned_variables = 0;
	int solution_not_found = 0;

	std::clock_t start = std::clock(); 
	int time = 0;
	if ( (csp.*algs[alg]) (0) ) {
		time = (std::clock() - start) / 1;

        //statistics
        std::cout << "RecursiveCallCounter = " 
            << csp.GetRecursiveCallCounter() << std::endl;
        std::cout << "IterationCounter     = " 
            << csp.GetIterationCounter() << std::endl;

		for (unsigned i=0;i<size;++i) {
			if ( ! array_of_variables[i]->IsAssigned() ) ++num_unassigned_variables;
		}

        if ( num_unassigned_variables == 0 ) {
            //check attacks
            for (unsigned i=0;i<size-1;++i) {
                for (unsigned j=i+1;j<size;++j) {
                    //|xi-xj| != |j-i|
                    int i_value = array_of_variables[i]->GetValue();
                    int j_value = array_of_variables[j]->GetValue();

                    if ( i_value == j_value  
                            ||
                            std::abs( static_cast<int>(i) - static_cast<int>(j) ) == std::abs( i_value - j_value ) )
                    {
                        ++attacks;
                        std::cout << "queens at " << i << " and " << j << " are attacking each other\n";
                    }
                }
            }
            if ( attacks>0 ) {
                std::cout << "FAILED - some queens are attacking each other\n";
            } else {
                std::cout << "Solution is correct\n";
            }
        } else {
			std::cout << "Some variables are unassigned\n";
		}

	}
	else {
		std::cout << "No solution found\n";
		solution_not_found = 1;
	}

	for (unsigned i=0;i<size;++i) { delete array_of_variables[i]; }
	delete [] array_of_variables;


	if (solution_not_found>0 || num_unassigned_variables>0 || attacks>0) {
		return -1; //this is CORRECTNESS
	}

	if      ( ret_value == TIME )       { return time; }
	else if ( ret_value == ITERATIONS ) { return csp.GetIterationCounter(); }
	else if ( ret_value == CALLS )      { return csp.GetRecursiveCallCounter(); }

	return 0; //this is CORRECTNESS
} catch ( const char * msg ) {
	std::cout << msg << std::endl;
	return 0;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

int test0() { //basic correctness - DFS 
	return queens(10,DFS,CORRECTNESS);
}
int test1() { //basic correctness - FC
	return queens(100,FC,CORRECTNESS);
}

int (*pTests[])() = { test0,test1 };

//program arguments 
//<test>  -- run predefined test, see above
//-100  <queen board size> <algorithm> <what to test>
int main (int argc, char ** argv) try {
    if (argc >1) {
        int test = 0; 
        std::sscanf(argv[1],"%i",&test); 

        pTests[test]();
        return 0; 
    }
} catch ( std::exception& e) {
	std::cerr << e.what() << std::endl;
}
