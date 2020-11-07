#ifndef CONSTRAINT_H
#define CONSTRAINT_H
#include <vector>
#include <fstream>
#include <cstdarg> /* va_list */

//interface for constraints object
template <typename T>
class Constraint {
	public:
		typedef T Variable;
	protected:
		std::vector<Variable*> vars;
		bool active; //active <=> non all variables are given values,
                     //when all vars are assigned it does not make sense to check the constraint
	public:
		////////////////////////////////////////////////////////////
		//ctor, collect pointers to variables involved in the constraint
		Constraint(Variable* v1, va_list valist);
		////////////////////////////////////////////////////////////
		//default ctor
		Constraint() : vars(),active(true) { }
		////////////////////////////////////////////////////////////
		//this is a base class, so make destructor virtual
		virtual ~Constraint() {}
		////////////////////////////////////////////////////////////
		//this is a Virtual Constructor - must be overridden in all base
		//classes
		virtual Constraint* clone () const { return NULL; }
		////////////////////////////////////////////////////////////
		//returns true if it is still possible to satisfy this constraints
		//only derived class knows how to implement this check
		virtual bool Satisfiable() const = 0;
		////////////////////////////////////////////////////////////
		virtual void Print (std::ostream& os) const = 0;
		////////////////////////////////////////////////////////////
		void AddVariable(Variable* new_var) { this->vars.push_back( new_var ); }
		////////////////////////////////////////////////////////////
		//return reference to vector of variables used in this 
		//constraint
		const std::vector<Variable*> & GetVars() const {
			return this->vars;
		}
		////////////////////////////////////////////////////////////
		//returns true if Constraint is marked as active
		bool IsActive() const { return active; }
		////////////////////////////////////////////////////////////
		//decides whether the Constraint is active by counting unassigned variables
		//no unassigned variables - mark as inactive (safe)
		void SetActive();

		////////////////////////////////////////////////////////////
		//returns true if 
		//1) all variables are assigned 
		//2) constraint holds
		//used for debugging and as a final test
		//not used in CSP - may be used in grading
		bool Check () const;
};

//concrete constraint - sum of any number of variables is equal to SUM
template <typename Variable, int SUM>
class SumEqual : public Constraint<Variable> {
	private:
		va_list valist; //need this to pass va_list to base class ctor
	public:
		////////////////////////////////////////////////////////////
		SumEqual() : Constraint<Variable>() {}
		////////////////////////////////////////////////////////////
		SumEqual(Variable* v1, ...) 
			: Constraint<Variable>(v1, (va_start(valist, v1), valist ) ) 
		{ va_end(valist); }
		////////////////////////////////////////////////////////////
		virtual SumEqual<Variable,SUM>* clone () const;
		////////////////////////////////////////////////////////////
		virtual bool Satisfiable() const;
		////////////////////////////////////////////////////////////
		void Print (std::ostream& os) const;
};

//concrete constraint - all variables are different
template <typename Variable>
class AllDiff : public Constraint<Variable> {
	private:
	va_list valist; //need this to pass va_list to base class ctor
	public:
	////////////////////////////////////////////////////////////
	AllDiff() : Constraint<Variable>() { }
	////////////////////////////////////////////////////////////
	AllDiff(Variable* v1, ...) 
		: Constraint<Variable>(v1, (va_start(valist, v1), valist ) ) 
	{ va_end(valist); }
	////////////////////////////////////////////////////////////
	virtual AllDiff<Variable>* clone () const;
	////////////////////////////////////////////////////////////
	void Print (std::ostream& os) const;
	////////////////////////////////////////////////////////////
	//constraint is true if all currently assigned variables have 
	//different values
	bool Satisfiable() const;
};


//concrete constraint - all variables are different
template <typename Variable>
class AllDiff2 : public Constraint<Variable> {
	public:
		////////////////////////////////////////////////////////////
		AllDiff2() : Constraint<Variable>() {}
		////////////////////////////////////////////////////////////
		AllDiff2(Variable* v1, Variable* v2) 
		{ this->vars.push_back(v1); this->vars.push_back(v2); }
		////////////////////////////////////////////////////////////
		virtual AllDiff2<Variable>* clone () const;
		////////////////////////////////////////////////////////////
		void Print (std::ostream& os) const;
		////////////////////////////////////////////////////////////
		//constraint is true if all currently assigned variables have 
		//different values
		bool Satisfiable() const;
};

//concrete constraint - all variables are different
template <typename Variable>
class DifferenceNotEqual : public Constraint<Variable> {
	private:
		va_list valist; //need this to pass va_list to base class ctor
		int constant;
	public:
		////////////////////////////////////////////////////////////
		DifferenceNotEqual() : Constraint<Variable>(), constant() {}
		////////////////////////////////////////////////////////////
		DifferenceNotEqual(int c, Variable* v1, ...);
		////////////////////////////////////////////////////////////
		virtual DifferenceNotEqual<Variable>* clone () const;
		////////////////////////////////////////////////////////////
		void Print (std::ostream& os) const;
		////////////////////////////////////////////////////////////
		//constraint is true if all currently assigned variables have 
		//different values
		bool Satisfiable() const;
};

#include "contraints.cpp"

#endif
