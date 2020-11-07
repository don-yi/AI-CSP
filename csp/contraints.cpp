#include "contraints.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

#ifdef INLINE_CONSTRAINT
	//#warning "INFO - inlining Constraint methods"
	#define INLINE inline
#else   
	//#warning "INFO - NOT inlining Constraint methods"
	#define INLINE 
#endif

////////////////////////////////////////////////////////////
//ctor, collect pointers to variables involved in the constraint
template <typename T>
Constraint<T>::Constraint(Variable* v1, va_list valist) : vars(), active(true) {
	Variable* arg;
	this->vars.push_back(v1);
	while ((arg = va_arg(valist, Variable*)) != 0) {
		this->vars.push_back(arg);
	}
}

////////////////////////////////////////////////////////////
//decides whether the Constraint is active by counting unassigned variables
//no unassigned variables - mark as inactive (safe)
template <typename T>
INLINE void Constraint<T>::SetActive() {
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	bool all_assigned = true;
	for ( ; b!=e && all_assigned; ++b ) {
		all_assigned = all_assigned && (*b)->IsAssigned();
	}
	active = !all_assigned;
}
////////////////////////////////////////////////////////////
//returns true if 
//1) all variables are assigned 
//2) constraint holds
//used for debugging and as a final test
//not used in CSP - may be used in grading
template <typename T>
bool Constraint<T>::Check () const {
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	for ( ; b!=e; ++b ) {
		if ( ! (*b)->IsAssigned() ) return false;
	}
	return Satisfiable();
}

////////////////////////////////////////////////////////////
template <typename T>
std::ostream& operator<<(std::ostream& os, const Constraint<T>& c) {
	c.Print(os); //virtual call 
	return os;
}

////////////////////////////////////////////////////////////
//SumEqual implementation
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//constructor
template <typename Variable, int SUM>
SumEqual<Variable,SUM>* SumEqual<Variable,SUM>::clone () const 
{
	SumEqual<Variable,SUM>* copy = new SumEqual<Variable,SUM>();
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	for ( ; b!=e; ++b ) {
		copy->AddVariable(*b);
	}
	return copy;
}
////////////////////////////////////////////////////////////
//keeps track of minimum and maximum assignment of variables
//in order for the constraint to be satisfiable 
//the SUM has to be between the 2 quantities above
template <typename Variable, int SUM>
INLINE bool SumEqual<Variable,SUM>::Satisfiable() const {
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	typename Variable::Value min_sum=0;
	typename Variable::Value max_sum=0;
	for ( ; b!=e; ++b ) {
		min_sum += (*b)->GetMinValue();
		max_sum += (*b)->GetMaxValue();
	}
	return min_sum <= SUM && max_sum >=SUM;
}
////////////////////////////////////////////////////////////
template <typename Variable, int SUM>
void SumEqual<Variable,SUM>::Print (std::ostream& os) const {
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	std::cout << "CONSTRAINT: sum of ";
	for ( ; b!=e; ++b ) {
		os << (*b)->Name() << " ";
	}
	os << " is " << SUM;// << std::endl;
	b = this->vars.begin();
	//for ( ; b!=e; ++b ) { os << **b; }
}


////////////////////////////////////////////////////////////
//AllDiff implementation
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//cloning method
template <typename Variable>
AllDiff<Variable>* AllDiff<Variable>::clone () const { 
	AllDiff<Variable>* copy = new AllDiff<Variable>();
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	for ( ; b!=e; ++b ) {
		copy->AddVariable(*b);
	}
	return copy;
}

////////////////////////////////////////////////////////////
//Print
template <typename Variable>
void AllDiff<Variable>::Print (std::ostream& os) const {
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	std::cout << "CONSTRAINT: all different of ";
	for ( ; b!=e; ++b ) {
		os << (*b)->Name() << " ";
	}
	//std::cout << std::endl;
	//b = this->vars.begin();
	//for ( ; b!=e; ++b ) { os << **b; }
}

////////////////////////////////////////////////////////////
//constraint is true if all currently assigned variables have 
//different values
template <typename Variable>
INLINE bool AllDiff<Variable>::Satisfiable() const {
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	typename std::set<typename Variable::Value> values;

	unsigned counter = 0;
	for ( ; b!=e; ++b ) {
		if ( (*b)->IsAssigned() ) {
			++counter;
			values.insert( (*b)->GetValue() );
			if (counter != values.size()) return false; //if not equal - we have seen duplicates
		}
	}
	return true;
}

////////////////////////////////////////////////////////////
//AllDiff2 implementation
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//cloning itself
template <typename Variable>
AllDiff2<Variable>* AllDiff2<Variable>::clone () const { 
	return new AllDiff2<Variable>(this->vars[0],this->vars[1]); 
}

////////////////////////////////////////////////////////////
template <typename Variable>
void AllDiff2<Variable>::Print (std::ostream& os) const {
	os << "CONSTRAINT: all different of "
		<< this->vars[0]->Name() << " " << this->vars[1]->Name() << " " << std::endl;
}

////////////////////////////////////////////////////////////
//constraint is true if all currently assigned variables have 
//different values
template <typename Variable>
INLINE bool AllDiff2<Variable>::Satisfiable() const {
	if ( !this->vars[0]->IsAssigned() || !this->vars[1]->IsAssigned() ) { return true; } 
	else 
		return this->vars[0]->GetValue() != this->vars[1]->GetValue();
}

////////////////////////////////////////////////////////////
//DifferenceNotEqual implementation
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
template <typename Variable>
DifferenceNotEqual<Variable>* 
DifferenceNotEqual<Variable>::clone () const { 
	DifferenceNotEqual<Variable>* copy = new DifferenceNotEqual<Variable>();
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	for ( ; b!=e; ++b ) {
		copy->AddVariable(*b);
	}
	copy->constant = constant;
	return copy;
}

////////////////////////////////////////////////////////////
template <typename Variable>
DifferenceNotEqual<Variable>::DifferenceNotEqual(int c, Variable* v1, ...) 
: Constraint<Variable>(v1, (va_start(valist, v1), valist ) ),
	constant( c>0 ? c:-c )
{
	va_end(valist); //finish va_start from MIL
}

////////////////////////////////////////////////////////////
template <typename Variable>
void DifferenceNotEqual<Variable>::Print (std::ostream& os) const {
	typename std::vector<Variable*>::const_iterator b = this->vars.begin();
	typename std::vector<Variable*>::const_iterator e = this->vars.end();
	os << "CONSTRAINT: abs of difference of 2 vars is NOT "
		<< constant << " ";
	for ( ; b!=e; ++b ) {
		std::cout << (*b)->Name() << " ";
	}
//	std::cout << std::endl;
	//b = this->vars.begin();
	//for ( ; b!=e; ++b ) { os << *b; }
}

////////////////////////////////////////////////////////////
//constraint is true if all currently assigned variables have 
//different values
template <typename Variable>
INLINE bool DifferenceNotEqual<Variable>::Satisfiable() const {
	if ( !this->vars[0]->IsAssigned() || !this->vars[1]->IsAssigned() ) { return true; } 
	else 
		return std::abs( this->vars[0]->GetValue() - this->vars[1]->GetValue() ) != constant;
}
#undef INLINE
