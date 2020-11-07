/******************************************************************************/
/*!
\file   contraints.graph.h
\author Dmitri Volper
\par    email: dvolper\@digipen.edu
\date   Wed 15 Apr 2009 12:19:18 PM PDT
\brief  
  Class representing data structure containing constraints and variables
  for Constraint Satisfaction Problem.

  Implements several types of methods
  1) insertion of constraints and variables
  2) preprocessing constraints and variables for fast retrieval
  3) methods to retrieve sub-collections of constraints and/or variables.
  4) checks
*/
/******************************************************************************/

/******************************************************************************/
/*!
  \class Variable
  \brief  
  Class representing all constraints for Constraint Satisfaction Problem.
  Implements:
	insertion of variables
  	insertion of constraints
	retrieval of all constraints by pointer to a Variable
	retrieval of all Variable connected to a given Variable
*/
/******************************************************************************/
#if defined(_MSC_VER)
// warning C4290: C++ exception specification ignored except to indicate
// a function is not __declspec(nothrow)
# pragma warning(disable:4290)
//warning C4996: 'std::xxx' was declared deprecated # pragma warning(disable:4996)
#endif 


#ifndef CONSTRAINT_GRAPH_H
#define CONSTRAINT_GRAPH_H
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <iostream>
#include <string>
#include <cstdarg>
#include <cassert>
#include <utility>
#include <algorithm>
#include "contraints.h"


//constraint graph - used in CSP Problem
//just a data structure to simplify access to constraints from variables
//and vice versa
template <typename T>
class ConstraintGraph {
	public:
		//typedef's
		typedef T Constraint;
		typedef typename Constraint::Variable Variable;

		//ctors,dtors
		ConstraintGraph();
		~ConstraintGraph();

		//insertion methods
		////////////////////////////////////////////////////////////
		void InsertVariable( Variable& var );
		////////////////////////////////////////////////////////////
		void InsertConstraint( const Constraint & c );

		////////////////////////////////////////////////////////////
		//pre-build collections
		void PreProcess();
		
		//retrieval methods
		////////////////////////////////////////////////////////////
		//set of Variables which are connected to a given Variable 
		//by a constraint 
		const typename std::set<Variable*>& 
			GetNeighbors( Variable* p_var );
		////////////////////////////////////////////////////////////
		//all constraints that use the given variables
		const typename std::vector<const Constraint*>& 
			GetConstraints( Variable* p_var ) const;
		////////////////////////////////////////////////////////////
		//vector of Constraints connecting 2 given Variables
		const typename std::set<const Constraint*>& 
			GetConnectingConstraints( Variable* p_var1, Variable* p_var2 );
		////////////////////////////////////////////////////////////
		//vector of all Variables
		const typename std::vector<Variable*>& GetAllVariables( ) const;

		//checks
		////////////////////////////////////////////////////////////
		//activate/disactivate constraints
		void CheckActivity();
		////////////////////////////////////////////////////////////
		//detect solution (that is -- all variable are assigned)
		bool AllVariablesAssigned() const;

		////////////////////////////////////////////////////////////
		void Print() const;
		////////////////////////////////////////////////////////////
	private:
		//data-structures to simplify life

		//mapping from variable to constraints the variable is used in.
		//that is constraints that depend on the variable
		std::map<Variable*,std::vector<const Constraint*> > var2constr;
		//all variables
		std::vector<Variable*> vars;
		//all constraints
		std::vector<Constraint*> constraints;//mainly for print
		//mapping from variable x to a set of variables Z
		//each z in Z is connected to x by a constraint
		//that is: there is a constraint that uses both variables x and z 
		std::map<Variable*, std::set<Variable*> > neighbors;
		//for each pair of variables all constraints that use both of them
		//used in consistency checking
		std::map< std::pair<Variable*,Variable*>, 
			std::set<const Constraint*> > connecting_constraints;
		//for internal use only
		std::map<std::string,Variable*> name2vars;
};

#include "contraints.graph.cpp"

#endif
