#include "contraints.graph.h"

#ifdef INLINE_CONSTRAINT_GRAPH
	//#warning "INFO - inlining ConstraintGraph methods"
	#define INLINE inline
#else   
	//#warning "INFO - NOT inlining ConstraintGraph methods"
	#define INLINE 
#endif

template <typename T>
ConstraintGraph<T>::ConstraintGraph() :
		var2constr(),
		vars(),
		constraints(),
		neighbors(),
		connecting_constraints(),
		name2vars()
{
}

template <typename T>
ConstraintGraph<T>::~ConstraintGraph() {
	typename std::vector<Constraint*>::const_iterator 
		b_constr = constraints.begin();
	typename std::vector<Constraint*>::const_iterator 
		e_constr = constraints.end();

	for ( ;b_constr!=e_constr;++b_constr) {
		delete *b_constr;
	}

}
////////////////////////////////////////////////////////////
//activate/disactivate constraints
template <typename T>
INLINE 
void ConstraintGraph<T>::CheckActivity() {
	typename std::vector<Constraint*>::iterator 
		b_constr = constraints.begin();
	typename std::vector<Constraint*>::iterator 
		e_constr = constraints.end();

	for ( ;b_constr!=e_constr;++b_constr) {
		(*b_constr)->SetActive();
	}
}
////////////////////////////////////////////////////////////
//build the above data-structures
template <typename T>
void ConstraintGraph<T>::PreProcess() {
	typename std::vector<Variable*>::const_iterator 
		b_vars = vars.begin();
	typename std::vector<Variable*>::const_iterator 
		e_vars = vars.end();
	for ( ;b_vars!=e_vars;++b_vars) { 
		const std::vector<const Constraint*>& 
			constr = GetConstraints( *b_vars );

		//for all constraints current variable is used in
		//find all other variables and put them in the set
		//set ensures no duplicates
		std::set<Variable*> neigh;
		typename std::vector<const Constraint*>::const_iterator 
			b_constr = constr.begin();
		typename std::vector<const Constraint*>::const_iterator 
			e_constr = constr.end();

		for ( ;b_constr!=e_constr;++b_constr) {
			typename std::vector< Variable*>::const_iterator 
				b_vars2 = (*b_constr)->GetVars().begin();
			typename std::vector< Variable*>::const_iterator 
				e_vars2 = (*b_constr)->GetVars().end();
			for ( ;b_vars2!=e_vars2;++b_vars2) {
				if ( *b_vars2 != *b_vars )//excluding the given variable
				{
					neigh.insert(*b_vars2);
					//current constraint "connects" 2 variables
					connecting_constraints[
						std::make_pair// C++11 does not like it <Variable*,Variable*>
						(*b_vars,*b_vars2)
						].insert(*b_constr);
				}

			}
		}
		neighbors[*b_vars] = neigh;
	}			
}

////////////////////////////////////////////////////////////
//set of Variables which are connected to a given Variable 
//by a constraint 
template <typename T>
INLINE 
const typename std::set<typename ConstraintGraph<T>::Variable*>& 
ConstraintGraph<T>::GetNeighbors( typename ConstraintGraph<T>::Variable* p_var ) {
	return neighbors[p_var];
}
////////////////////////////////////////////////////////////
//set of unassigned Variables which are connected to a given Variable 
//by a constraint 
//template <typename T>
//unsigned ConstraintGraph<T>::GetCountUnassignedNeighbors( Variable* p_var ) {
//	unsigned result;
//	typename std::set<Variable*>::const_iterator b_n = neighbors[p_var].begin();
//	typename std::set<Variable*>::const_iterator e_n = neighbors[p_var].end();
//	for ( ; b_n != e_n; ++b_n ) {
//		result += ( (*b_n)->IsAssigned() ? 0:1 );
//	}
//	return result;
//}
////////////////////////////////////////////////////////////
//vector of Constraints connecting 2 given Variables
template <typename T>
INLINE 
const typename std::set<const typename ConstraintGraph<T>::Constraint*>& 
ConstraintGraph<T>::GetConnectingConstraints( 
		typename ConstraintGraph<T>::Variable* p_var1, 
		typename ConstraintGraph<T>::Variable* p_var2 ) 
{
	return connecting_constraints[
		std::make_pair// C++11 does not like it  <Variable*,Variable*> 
            (p_var1,p_var2)
		];
}
////////////////////////////////////////////////////////////
//detect dead-end
//template <typename T>
//bool ConstraintGraph<T>::CheckPossibleAssignments() const {
//	typename std::vector<typename ConstraintGraph<T>::Variable*>::const_iterator 
//		b_vars = vars.begin();
//	typename std::vector<typename ConstraintGraph<T>::Variable*>::const_iterator 
//		e_vars = vars.end();
//	for ( ;b_vars!=e_vars;++b_vars) { 
//		if ( ! (*b_vars)->IsImpossible() ) return false;
//	}
//	return true;
//}
////////////////////////////////////////////////////////////
template <typename T>
void ConstraintGraph<T>::InsertVariable( typename ConstraintGraph<T>::Variable& var ) {
	//Variable* p_var = new Variable(var);
	Variable* p_var = &var;

	vars.push_back( p_var );
	name2vars[var.Name()] =  p_var;

	var2constr.insert( 
			std::make_pair// C++11 does not like it <Variable*,std::vector<const Constraint*> >
			( p_var, std::vector<const Constraint*>()) ); //initially empty 

	//std::cout << __FILE__ << " " << __LINE__ << " var address " << p_var << std::endl;
	//std::cout << *p_var << std::endl; 
	//std::cout << std::endl;
}
////////////////////////////////////////////////////////////
template <typename T>
void ConstraintGraph<T>::InsertConstraint( const Constraint & c ) {
	Constraint* p_c = c.clone();
	//std::cout << "local constraint " << *p_c << std::endl;
	const std::vector<Variable*> & vars_in_constraint = p_c->GetVars();
	//check we know all variables
	typename std::vector<Variable*>::const_iterator 
		b = vars_in_constraint.begin();
	typename std::vector<Variable*>::const_iterator 
		e = vars_in_constraint.end();

	//insert constraint as an outgoing to all variables
	//used in the constraint
	for ( ;b!=e;++b) {
		typename std::map<std::string,Variable*>::iterator 
			it = name2vars.find( (*b)->Name() );
		if ( it == name2vars.end() ) {
			std::cout << "unknown variable name " << (*b)->Name() << std::endl;
			throw "constraint uses unknown variable name";
		}
		//successfully found variable
		var2constr[ it->second ].push_back ( p_c );
	}
	constraints.push_back( p_c );
}
////////////////////////////////////////////////////////////
template <typename T>
INLINE 
const typename std::vector<const typename ConstraintGraph<T>::Constraint*>& 
ConstraintGraph<T>::GetConstraints( typename ConstraintGraph<T>::Variable* p_var ) const 
{
	typename 
		std::map< Variable*, std::vector<const Constraint*> >::
		const_iterator it = var2constr.find( p_var );
	if ( it	!= var2constr.end() ) return it->second;
	else throw "cannot find variable in the graph";
}
////////////////////////////////////////////////////////////
//vector of all Variables
template <typename T>
INLINE 
const typename std::vector<typename ConstraintGraph<T>::Variable*>&
ConstraintGraph<T>::GetAllVariables( ) const {
	return vars;
}
////////////////////////////////////////////////////////////
//detect solution (that is -- all variables are assigned)
template <typename T>
INLINE 
bool ConstraintGraph<T>::AllVariablesAssigned() const {
	typename std::vector<Variable*>::const_iterator 
		b_vars = vars.begin();
	typename std::vector<Variable*>::const_iterator 
		e_vars = vars.end();
	for ( ;b_vars!=e_vars;++b_vars) { 
		if ( ! (*b_vars)->IsAssigned() ) return false;
	}
	return true;
}
////////////////////////////////////////////////////////////
template <typename T>
void ConstraintGraph<T>::Print() const {
	typename std::vector<Variable*>::const_iterator 
		b_vars = vars.begin();
	typename std::vector<Variable*>::const_iterator 
		e_vars = vars.end();
	std::cout << "+------------------+\n";
	std::cout << "|  ConstraintGraph |\n";
	std::cout << "+------------------+\n";
	std::cout << "|-->Variables :\n";
	for ( ;b_vars!=e_vars;++b_vars) { 
		std::cout << "|   " << **b_vars << std::endl;
		//std::cout << "   "; (*b_vars)->Print();
	}

	typename std::vector<Constraint*>::const_iterator 
		b_constr = constraints.begin();
	typename std::vector<Constraint*>::const_iterator 
		e_constr = constraints.end();
	std::cout << "|-->Constraints :\n";
	for ( ;b_constr!=e_constr;++b_constr) { 
		std::cout << "|   "; std::cout << **b_constr << std::endl;
	}
	std::cout << "+------------------+\n";
}
////////////////////////////////////////////////////////////
#undef INLINE
