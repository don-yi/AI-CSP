#ifndef CSP_H
#define CSP_H
#include <vector>
#include <set>
#include <iostream>
#include <string>
#include <cstdarg>
#include <cassert>
#include <deque>
#include <utility>
#include <algorithm>
#include <limits>

template <typename C>
struct Arc {
	typedef typename C::Variable Variable;
	//non-owning semantics, therefore DO NOT need big-4
	Variable* x;
	Variable* y;
	const C* c;
	Arc(Variable* x,Variable* y,const C* c) : x(x),y(y),c(c) {}
	bool operator< (const Arc<C>& rhs) const {
		if ( x<rhs.x) return true;
		if ( x==rhs.x && y<rhs.y) return true;
		if ( x==rhs.x && y==rhs.y && c<rhs.c) return true;

		return false;
	}
};

template <typename T> 
class CSP {
		//typedef's for intenal use
		typedef typename T::Constraint      Constraint;
		typedef typename T::Variable        Variable;
		typedef typename T::Variable::Value Value;
	public:
		////////////////////////////////////////////////////////////
		//counters
		////////////////////////////////////////////////////////////
		//ctor
		CSP(T &cg);

		//get the number of found solutions
		int GetSolutionCounter() const { return solution_counter; }
		//get the number of recursive calls - for debugging
		int GetRecursiveCallCounter() const { return recursive_call_counter; }
		//get the number of variable assigns in Solve* - for debugging
		int GetIterationCounter() const { return iteration_counter; }

		//CSP counting
		bool SolveFC_count(unsigned level);
		//CSP solver, brute force - no forward checking
		bool SolveDFS(unsigned level);
		//CSP solver, uses forward checking
		bool SolveFC(unsigned level);
		//CSP solver, uses arc consistency
		bool SolveARC(unsigned level);
	private:
		//2 versions of forward checking algorithms
		bool ForwardChecking(Variable *x);
		//load states (available values) of all unassigned variables 
		void LoadState(std::map<Variable*, std::set<typename CSP<T>::Variable::Value> >& saved) 
			//note: "CSP<T>::" in "std::set<typename CSP<T>::Variable::Value>" required for MSC
		const ;
		//save states (available values) of all unassigned variables 
		//except the current
		std::map<Variable*, std::set<typename Variable::Value> > 
			SaveState(Variable* x) const;
		//check the current (incomplete) assignment for satisfiability
		bool AssignmentIsConsistent( Variable* p_var ) const;
		//insert pair 
		//(neighbors of the current variable, the current variable)
		//for all y~x insert (y,x)
		//into arc-consistency queue
		void InsertAllArcsTo( Variable* cv );

		//AIMA p.146 AC-3 algorithm
		bool CheckArcConsistency(Variable* x);
		//CHECK that for each value of x there is a value of y 
		//which makes all constraints involving x and y satisfiable
		bool RemoveInconsistentValues(Variable* x,Variable* y,const Constraint* c);
		//choose next variable for assignment
		//choose the one with minimum remaining values
		Variable* MinRemVal();
		//choose next variable for assignment
		//choose the one with max degree
		Variable* MaxDegreeHeuristic();


		//data
		//deque of arcs (2 Variables connected through a Constraint)
		std::set< Arc<Constraint> > arc_consistency;
		T &cg;
		int solution_counter,recursive_call_counter,iteration_counter;
};

#include "csp.cpp"

#endif
