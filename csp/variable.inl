#ifdef INLINE_VARIABLE
	#define INLINE inline
#else   
	#define INLINE 
#endif

#include <iostream>

/******************************************************************************/
/*!
	Remove value from the domain
	\param val
		value to be removed
	\exception VariableException 
		thrown if the removed value is not in the domain
*/
/******************************************************************************/
INLINE void Variable::RemoveValue(Value val) {
	//check value is in domain
	std::set<Value>::iterator it = domain.find(val);
	if ( it != domain.end() ) {
		domain.erase(it);
	}
	else {
		throw VariableException("Variable::RemoveValue - value is not in the domain");
	}

}
/******************************************************************************/
/*!
	Getter for name
	\return 
		a string specifying symbol/name
*/
/******************************************************************************/
INLINE const std::string & Variable::Name() const { 
	return name; 
}
/******************************************************************************/
/*!
	Number of remaining values for this variable
	\return 
		number of remaining values for this variable
*/
/******************************************************************************/
INLINE int Variable::SizeDomain() const { 
	return domain.size(); 
}
/******************************************************************************/
/*!
	Set the domain of variable
	\param vals
		a set of values forming a new domain
*/
/******************************************************************************/
INLINE void Variable::SetDomain(const std::set<Value>& vals) { 
	domain.clear();
	std::copy(vals.begin(),vals.end(),
			std::inserter( domain, 
				domain.begin() ));
}
/******************************************************************************/
/*!
	Returns a reference to the domain of this variable
	used to save state of the variable
	\return 
		a const reference to a domain
*/
/******************************************************************************/
INLINE const std::set<Variable::Value>& Variable::GetDomain() const { 
	return domain;
}
/******************************************************************************/
/*!
	Whether variable has a value assigned to it or not
	\return 
		bool
*/
/******************************************************************************/
INLINE bool Variable::IsAssigned() const { 
	return is_assigned; 
}
/******************************************************************************/
/*!
	Wheter variable has non-empty domain
	returns true if there is no more value left for this variable
	\return 
		bool
*/
/******************************************************************************/
INLINE bool Variable::IsImpossible() const { 
	return domain.size() == 0; 
}
/******************************************************************************/
/*!
	Getter for the unique ID
	\return 
		ID
*/
/******************************************************************************/
INLINE unsigned Variable::ID() const { 
	return id; 
}
/******************************************************************************/
/*!
	Assigns a specific value to the variable. Given value should be legal, 
	that is	be in the domain of this variable, method does not perform 
	assignment and returns false otherwise.
	\exception VariableException 
		thrown if the assigned value is not in the domain
*/
/******************************************************************************/
INLINE void Variable::Assign(Variable::Value val)  {
#ifdef DEBUG
	std::set<Variable::Value>::iterator it = domain.find(val);
	//check value is in domain
	if ( it != domain.end() ) {
		is_assigned = true;
		assigned_value = val;
		return;
	}
	throw VariableException("Variable::Assign(Variable::Value) -- value is not in the domain");
#else
	is_assigned = true;
	assigned_value = val;
#endif
}
/******************************************************************************/
/*!
	Assigns a random value from the domain. 
	\exception VariableException 
		thrown if there is no value in the domain
*/
/******************************************************************************/
INLINE void Variable::Assign()  {
	if ( IsImpossible() ) throw VariableException("Variable::Assign() -- empty domain");
	is_assigned = true;
	assigned_value = *domain.begin();
	return;
}
/******************************************************************************/
/*!
	Returns minimum value in the domain of this variable
	used to test satisfiability of some of the constraints. 
	\return 
		Value 
	\exception VariableException 
		thrown if there is no value in the domain
*/
/******************************************************************************/
INLINE Variable::Value Variable::GetMinValue() const {
	if ( is_assigned ) return assigned_value;
	else if (IsImpossible()) throw VariableException("GetMinValue - empty domain");
	else return *domain.begin();
		//*std::min_element (domain.begin(),domain.end() );
}
/******************************************************************************/
/*!
	Returns maximum value in the domain of this variable
	used to test satisfiability of some of the constraints. 
	\return 
		Value 
	\exception VariableException 
		thrown if there is no value in the domain
*/
/******************************************************************************/
INLINE Variable::Value Variable::GetMaxValue() const {
	if ( is_assigned ) return assigned_value;
	else if (IsImpossible()) throw VariableException("GetMaxValue - empty domain");
	else { std::set<Value>::const_iterator it = domain.end(); std::advance(it,-1); return *it; }
		//*std::max_element (domain.begin(),domain.end() );
}
/******************************************************************************/
/*!
	Returns assigned value of this variable
	aborts if value is not assigned
	\return 
		Value 
	\exception VariableException 
		thrown if there is no assigned value 
*/
/******************************************************************************/
INLINE Variable::Value Variable::GetValue() const {
	if ( !is_assigned ) throw VariableException("Variable::GetValue - unassigned");
	return assigned_value;
}
/******************************************************************************/
/*!
	Unassign this variable
	\exception VariableException 
		thrown if variable is not initially assigned 
*/
/******************************************************************************/
INLINE void Variable::UnAssign() {
	if ( !is_assigned ) 
		throw VariableException("Variable::UnAssign - already unassigned");
	is_assigned = false; 
}

#undef INLINE
