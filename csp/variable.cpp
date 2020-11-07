#include "variable.h"
#include <iostream>
#include <algorithm>

#ifndef INLINE_VARIABLE
	//#warning "INFO - NOT inlining Variable"
	#include "variable.inl"
#endif
	
unsigned Variable::nextid=0;

/******************************************************************************/
/*!
	Creates a variable object with a given name and domain
	\param name
		a string specifying symbol/name
	\param av 
		domain of the variable
*/
/******************************************************************************/
Variable::Variable ( const std::string & name, const std::vector<Value> & av ) : 
	name(name),
	domain (av.begin(),av.end()),
	assigned_value(Value()),
	is_assigned(false),
	id(++nextid) 
{}

/******************************************************************************/
/*!
	Stream insertion for Variable
*/
/******************************************************************************/
std::ostream& operator<<(std::ostream& os, const Variable& v) {
	std::set<Variable::Value>::const_iterator b = v.GetDomain().begin();
	std::set<Variable::Value>::const_iterator e = v.GetDomain().end();
	os << "Variable \"" << v.Name() << "\" available values: ";
	for ( ;b!=e;++b) { os << *b << " "; }
	if ( v.IsAssigned() ) 
		os << " \nassigned value " << v.GetValue();
	return os;
}

/******************************************************************************/
/*!
	Print this variable
*/
/******************************************************************************/
void Variable::Print() const {
	std::cout << *this;
}

