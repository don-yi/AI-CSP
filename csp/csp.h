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
#include <map>

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

		// deep cp dom for mod'ing orig dom
		std::set<typename Variable::Value>* CpDomFromVar(Variable* var);

		//data
		//deque of arcs (2 Variables connected through a Constraint)
		std::set< Arc<Constraint> > arc_consistency;
		T &cg;
		int solution_counter,recursive_call_counter,iteration_counter;
};

#ifdef INLINE_CSP
	//#warning "INFO - inlining CSP methods"
	#define INLINE inline
#else   
	//#warning "INFO - NOT inlining CSP methods"
	#define INLINE 
#endif

////////////////////////////////////////////////////////////
//CSP constructor
template <typename T> 
CSP<T>::CSP(T &cg) :
	arc_consistency(),
	cg(cg),
	solution_counter(0),
	recursive_call_counter(0),
	iteration_counter(0) 
{
}

template<typename T>
bool CSP<T>::SolveFC_count(unsigned level) {
	return false;
}

////////////////////////////////////////////////////////////
//CSP solver, brute force - no forward checking
template <typename T> 
bool CSP<T>::SolveDFS(unsigned level) {
  // debugging purpose
  bool const isDebugOn = false;

  // update rec call ct
  ++recursive_call_counter;

  if (isDebugOn)
    std::cout << "entering SolveDFS (level " << level << ")\n";

  // true ending
  if (cg.AllVariablesAssigned()) {
    if (isDebugOn)
      std::cout << "exiting SolveDFS (level " << level << ")\n";
    return true;
  }

  Variable* var_to_assign = MinRemVal();

  // get var w/ mrv
  std::set<typename Variable::Value> const& domain = var_to_assign->GetDomain();

	// for each val in domain
  for (
    typename std::set<typename Variable::Value>::const_iterator i
    = domain.begin();
    i != domain.end();
    ++i
    ) {

    ++iteration_counter;

		//// init's
		var_to_assign->Assign(*i);
		if (isDebugOn)
			std::cout << "trying assigning, "
			<< var_to_assign->Name() << ": " << *i << "\n";

		bool isSatisfied = true;


		//  for each constraint c such that v is a variable of c
		//            and all other variables of c
		//            are assigned.
		for (auto& constr : cg.GetConstraints(var_to_assign)) {
			// check every constr's are satisfied
			if (not constr->Satisfiable()) {
				isSatisfied = false;
				break;
			}
    }

    // if satis'ed, rec to nxt lvl
		if (isSatisfied) {
      if (isDebugOn)
        std::cout << " satisfied, one more down br " << "\n" << "\n";
			if (SolveDFS(level + 1))
				return true;
      if (isDebugOn)
        std::cout << "\n";
		}

    // otherwise, unassign and try nxt val in domain
    if (isDebugOn)
      std::cout << " Unsatisfied, unassigning "
      << var_to_assign->Name() << ": "
      << var_to_assign->GetValue() << "\n" << "\n";
    var_to_assign->UnAssign();
  }

  if (isDebugOn)
    std::cout << "exiting SolveDFS (level " << level << ")\n";

	return false;
}
////////////////////////////////////////////////////////////
//CSP solver, uses forward checking
template <typename T> 
bool CSP<T>::SolveFC(unsigned level) {
  // debugging purpose
  bool const isDebugOn = false;

  // update rec call ct
  ++recursive_call_counter;

  if (isDebugOn)
    std::cout << "entering SolveFC (level " << level << ")\n";

  // true ending
  if (cg.AllVariablesAssigned()) {
    if (isDebugOn)
      std::cout << "exiting SolveFC (level " << level << ")\n";
    return true;
  }

  // get next var to assign
  Variable* var_to_assign = MinRemVal();

  // save state of all unassigned var's except curr
  std::map<
    typename CSP<T>::Variable*, std::set<typename CSP<T>::Variable::Value>
  > savedState = SaveState(var_to_assign);

  // for each val in domain
  std::set<typename Variable::Value> const& domain1
    = var_to_assign->GetDomain();
  for (
    typename std::set<typename Variable::Value>::const_iterator domItr1
    = domain1.begin();
    domItr1 != domain1.end();
    ++domItr1
    ) {

		++iteration_counter;

		//// init's
		bool hasPossibleFuture = true;

    if (isDebugOn)
      std::cout << "trying assigning, "
      << var_to_assign->Name() << ": " << *domItr1 << "\n";
    var_to_assign->Assign(*domItr1);

    // for each neighboring var's
    std::set<Variable*> const& neighbors = cg.GetNeighbors(var_to_assign);
    for (
      typename std::set<Variable*>::const_iterator neiItr = neighbors.begin();
      neiItr != neighbors.end();
      ++neiItr
      ) {

      // skip assigned var's
      if ((*neiItr)->IsAssigned())
        continue;

      // for each val in domain of neighbor
      std::set<typename Variable::Value> const* const domain2
				= CpDomFromVar(*neiItr);
      for (
        typename std::set<typename Variable::Value>::const_iterator domItr2
        = domain2->begin();
        domItr2 != domain2->end();
        ++domItr2
        ) {

        // assign neighbor to test constr's
        (*neiItr)->Assign(*domItr2);

        // for each connected constr
        const std::set<const Constraint*>& constr
          = cg.GetConnectingConstraints(var_to_assign, *neiItr);
        for (
          typename std::set<const Constraint*>::const_iterator constrItr
          = constr.begin();
          constrItr != constr.end();
          ++constrItr
          ) {

          if (not (*constrItr)->Satisfiable()) {
            // non-satis cond, rm val from dom
            if (isDebugOn)
              std::cout << "  unsatisfied, rm'ing val from neighbor "
              << (*neiItr)->Name() << ": " << *domItr2 << "\n";
            (*neiItr)->RemoveValue(*domItr2);
            break;
          }
        }

        // val tested, so unassign
        (*neiItr)->UnAssign();
      }

			delete domain2;

			// var w/o domain, no possible future
      if ((*neiItr)->IsImpossible()) {
				// has no future w/ val assignment
				hasPossibleFuture = false;
        break;
      }
    }

    // if assignment has possible future, rec to nxt lvl
    if (hasPossibleFuture) {
      if (isDebugOn)
        std::cout << "    has possible future, to nxt lvl w/ "
        << var_to_assign->Name() << ": " << var_to_assign->GetValue()
        << "\n" << "\n";
      if (SolveFC(level + 1))
        return true;
      if (isDebugOn)
        std::cout << "\n";
    }

    // otherwise, unassign and try nxt val in domain w/ orig state
    if (isDebugOn) {
      std::cout << "      unsatisfied, unassigning "
        << var_to_assign->Name() << ": "
        << var_to_assign->GetValue() << "\n" << "\n";
    }
    var_to_assign->UnAssign();
    // load state and break out to try diff var to assign
    LoadState(savedState);
  }

  // bad ending
  if (isDebugOn)
    std::cout << "exiting SolveFC (level " << level << ")\n";
  return false;
}
////////////////////////////////////////////////////////////
//CSP solver, uses arc consistency
template <typename T> 
bool CSP<T>::SolveARC(unsigned level) {

	// TODO: place holder
	(void)level;
	return false;

}


template <typename T> 
INLINE
bool CSP<T>::ForwardChecking(Variable *x) {

	// todo: place holder
	return false;






}
////////////////////////////////////////////////////////////
//load states (available values) of all unassigned variables 
template <typename T> 
void CSP<T>::LoadState(
		std::map<Variable*, 
		std::set<typename CSP<T>::Variable::Value> >& saved) const 
{
	typename std::map<Variable*, std::set<typename Variable::Value> >::iterator 
		b_result = saved.begin();
	typename std::map<Variable*, std::set<typename Variable::Value> >::iterator 
		e_result = saved.end();

	for ( ; b_result != e_result; ++b_result ) {
		//std::cout << "loading state for " /cg
		//<< b_result->first->Name() << std::endl;
		(*b_result).first->SetDomain( (*b_result).second );
	}
}


////////////////////////////////////////////////////////////
//save states (available values) of all unassigned variables 
//except the current
template <typename T> 
INLINE
std::map< typename CSP<T>::Variable*, std::set<typename CSP<T>::Variable::Value> > 
CSP<T>::SaveState(typename CSP<T>::Variable* x) const {
	std::map<Variable*, std::set<typename Variable::Value> > result;

	const std::vector<Variable*>& all_vars = cg.GetAllVariables();
	typename std::vector<Variable*>::const_iterator 
		b_all_vars = all_vars.begin();
	typename std::vector<Variable*>::const_iterator 
		e_all_vars = all_vars.end();
	for ( ; b_all_vars!=e_all_vars; ++b_all_vars) {
		if ( !(*b_all_vars)->IsAssigned() && *b_all_vars!=x ) {
			//std::cout << "saving state for " 
			//<< (*b_all_vars)->Name() << std::endl;
			result[ *b_all_vars ] = (*b_all_vars)->GetDomain();
		}
	}
	return result;
}
////////////////////////////////////////////////////////////
//check the current (incomplete) assignment for satisfiability
template <typename T> 
INLINE
bool CSP<T>::AssignmentIsConsistent( Variable* p_var ) const {

	// todo: place holder
	return false;












}
////////////////////////////////////////////////////////////
//insert pair 
//(neighbors of the current variable, the current variable)
//current variable is th variable that just lost some values
// for all y~x insert (y,x)
//into arc-consistency queue
template <typename T> 
INLINE
void CSP<T>::InsertAllArcsTo( Variable* cv ) {











}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//AIMA p.146 AC-3 algorithm
template <typename T> 
INLINE
bool CSP<T>::CheckArcConsistency(Variable* x) {

	// todo: place holder
	return false;














}
////////////////////////////////////////////////////////////
//CHECK that for each value of x there is a value of y 
//which makes all constraints involving x and y satisfiable
template <typename T> 
INLINE
bool CSP<T>::RemoveInconsistentValues(Variable* x,Variable* y,const Constraint* c) {

	// todo: place holder
	return false;














}
////////////////////////////////////////////////////////////
//choose next variable for assignment
//choose the one with minimum remaining values
template <typename T> 
INLINE
typename CSP<T>::Variable* CSP<T>::MinRemVal() {
	const typename std::vector<Variable*> vars = cg.GetAllVariables();
	typename std::vector<Variable*>::const_iterator b_vars = vars.begin();
	typename std::vector<Variable*>::const_iterator e_vars = vars.end();
	typename std::vector<Variable*>::const_iterator mrv = e_vars;
	for (; b_vars != e_vars; ++b_vars) {
		if ((*b_vars)->IsAssigned())
			continue;

		if (mrv == e_vars)
      mrv = b_vars;

    if ((*b_vars)->SizeDomain() < (*mrv)->SizeDomain())
      mrv = b_vars;
	}

	return *mrv;
}
////////////////////////////////////////////////////////////
//choose next variable for assignment
//choose the one with max degree
template <typename T> 
typename CSP<T>::Variable* CSP<T>::MaxDegreeHeuristic() {

	// todo: place holder
	return {};
















}

template<typename T>
inline std::set<typename CSP<T>::Variable::Value>* CSP<T>::CpDomFromVar(
	CSP<T>::Variable* var
	) {

	std::set<typename Variable::Value> const& orig = var->GetDomain();
	typename std::set<typename Variable::Value>::const_iterator b_dom
		= orig.begin();
	typename std::set<typename Variable::Value>::const_iterator e_dom
		= orig.end();
	std::set<typename CSP<T>::Variable::Value>* const cp
    = new std::set<typename CSP<T>::Variable::Value>;
	for (; b_dom != e_dom; ++b_dom) 
		cp->insert(*b_dom);

	return cp;
}

#undef INLINE

#endif
