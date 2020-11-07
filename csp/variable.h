/******************************************************************************/
/*!
\file   variable.h
\author Dmitri Volper
\par    email: dvolper\@digipen.edu
\date   Tue 31 Mar 2009 01:51:20 PM PDT
\brief  
  Class representing a variable for Constraint Satisfaction Problem.
  Implements domain, assigned/not assigned state.
  Variable has a name and a unique id (id is not currently used)
  
*/
/******************************************************************************/

/******************************************************************************/
/*!
  \class Variable
  \brief  
  Class representing a variable for Constraint Satisfaction Problem.
  Implements domain, assigned/not assigned state.
  Variable has a name and a unique id (id is not currently used)

    Operations include:

*/
/******************************************************************************/
#if defined(_MSC_VER)
// warning C4290: C++ exception specification ignored except to indicate
// a function is not __declspec(nothrow)
# pragma warning(disable:4290)
//warning C4996: 'std::xxx' was declared deprecated # pragma warning(disable:4996)
#endif 


#ifndef VARIABLE_H
#define VARIABLE_H
#include <vector>
#include <set>
#include <fstream>
#include <string>


class VariableException : public std::exception {
	std::string msg;
	public:
	VariableException(const std::string & _msg) : msg(_msg) {}
	const char * what() const throw () { return msg.c_str(); }
	virtual ~VariableException() throw () {}
};

class Variable {
	public:
		//! variable has to specify it's value type - used by other classes 
		typedef int Value;

	private:
		static unsigned nextid;

		//! symbol/name of the variable 
		std::string name;

		//! all remaining available values
		std::set<Value> domain;

		//! currently assigned value (invalid if is_assigned == false)
		Value assigned_value; 

		//! validity of the assigned_value 
		bool is_assigned; 

		//! unique ID (not used)
		unsigned id;

	public:
		Variable ( const std::string & name, const std::vector<Value> & av );
		const std::string & Name() const;
		void  RemoveValue(Value val);
		void  SetDomain(const std::set<Value>& vals);
		int   SizeDomain() const;
		const std::set<Value>& GetDomain() const;
		bool  IsImpossible() const;
		unsigned ID() const;
		bool  IsAssigned() const;
		void  Assign(Value val);
		void  Assign();
		void  UnAssign();
		Value GetMinValue() const;
		Value GetMaxValue() const;
		Value GetValue() const;
		void  Print() const;
};

std::ostream& operator<<(std::ostream& os, const Variable& v);

#ifdef INLINE_VARIABLE
	//#warning "INFO - inlining Variable methods"
	#include "variable.inl"
#endif

#include "variable.cpp"

#endif
