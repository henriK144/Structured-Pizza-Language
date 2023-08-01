#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include "parsetypes.hpp"

// Defines the types of statements and how the program processes them 

struct ProgramState;

class Statement
{
private:
public:
	virtual int execute(ProgramState& ps) = 0; // The return value is presumably some kind of error code,
	virtual ~Statement() = 0; // although I do hope we can catch most errors at interpret-time rather than run-time
};
inline Statement::~Statement() { }


class StartSession: public Statement
{
private: // Whatever the statement is parameterized over
	std::string name;
	int reserves; // this should be the amount of pizzas you expect to be ordered, roughly
public:
	StartSession(const std::string& _name, int _reserves) : name{_name}, reserves{_reserves} {}
	virtual int execute(ProgramState& ps);
};

class NameSession: public Statement
{
private:
	std::string name;
public:
	NameSession(const std::string& _name) : name{_name} {}
	virtual int execute(ProgramState& ps);
}; 

class EndSession: public Statement
{
private:
public:
	EndSession() {}
	virtual int execute(ProgramState& ps);
};

class SaveSession: public Statement
{
private:
	std::string filepath;
public:
	SaveSession(const std::string& _filepath) : filepath{_filepath} {}
	virtual int execute(ProgramState& ps);
}; 

class LoadSession: public Statement
{
private:
	std::string filepath;
public:
	LoadSession(const std::string& _filepath) : filepath{_filepath} {}
	virtual int execute(ProgramState& ps);
}; 

class AddPizza: public Statement
{
private:
	Pizza p;
	std::string name;
public:
	AddPizza(Pizza _p, const std::string& _name) : p{_p}, name{_name} {}
	virtual int execute(ProgramState& ps);
};  

class RemovePizza: public Statement
{
private:
	PizzaSpecifier pspec;
public:
	RemovePizza(const PizzaSpecifier& _pspec) : pspec{_pspec} {}
	virtual int execute(ProgramState& ps);
}; 

class ViewPizza: public Statement
{
private:
	PizzaSpecifier pspec;
	bool details;
	bool all;
public:
	ViewPizza(const PizzaSpecifier& _pspec, bool _details, bool _all) : pspec{_pspec}, details{_details}, all{_all} {}
	virtual int execute(ProgramState& ps);
}; 

class VotePizza: public Statement
{
private:
	PizzaSpecifier pspec;
	int n;
public:
	VotePizza(const PizzaSpecifier& _pspec, int _n) : pspec{_pspec}, n{_n} {}
	virtual int execute(ProgramState& ps);
};

class SelectTopPizza: public Statement
{
private:
	int n;
public:
	SelectTopPizza(int _n) : n{_n} {}
	virtual int execute(ProgramState& ps);
};

class ResetSessionVotes: public Statement
{
private:
public:
	ResetSessionVotes() {}
	virtual int execute(ProgramState& ps);
};

class ResetSession: public Statement
{
private:
public:
	ResetSession() {}
	virtual int execute(ProgramState& ps);
};

class AlterPizzaAdd: public Statement
{
private:
	PizzaSpecifier pspec;
	ToppingArrangement ta;
public:
	AlterPizzaAdd(const PizzaSpecifier& _pspec, ToppingArrangement _ta) : pspec{_pspec}, ta{_ta} {}
	virtual int execute(ProgramState& ps);
};

class AlterPizzaRemove: public Statement
{
private:
	PizzaSpecifier pspec;
	ToppingArrangement ta;
public:
	AlterPizzaRemove(const PizzaSpecifier& _pspec, ToppingArrangement _ta) : pspec{_pspec}, ta{_ta} {}
	virtual int execute(ProgramState& ps);
};

class AlterPizzaSetCrust: public Statement
{
private:
	PizzaSpecifier pspec;
	Crust c;
public:
	AlterPizzaSetCrust(const PizzaSpecifier& _pspec, Crust _c) : pspec{_pspec}, c{_c} {}
	virtual int execute(ProgramState& ps);
};

class AlterPizzaSetSauce: public Statement
{
private:
	PizzaSpecifier pspec;
	Sauce s;
public:
	AlterPizzaSetSauce(const PizzaSpecifier& _pspec, Sauce _s) : pspec{_pspec}, s{_s} {}
	virtual int execute(ProgramState& ps);
};

class AlterPizzaSetCheese: public Statement
{
private:
	PizzaSpecifier pspec;
	Cheese ch;
public:
	AlterPizzaSetCheese(const PizzaSpecifier& _pspec, Cheese _ch) : pspec{_pspec}, ch{_ch} {}
	virtual int execute(ProgramState& ps);
};

class Quit: public Statement
{
private:
public:
	Quit() {}
	virtual int execute(ProgramState& ps);
};

using Program = std::vector<std::unique_ptr<Statement>>;
int execute(Program& p, ProgramState& ps);

inline std::ostream& operator<<(std::ostream& os, const Program& p)
{
	int n = 1;
	os << "Program pointers: [\n";
	for (const auto& pp : p) {
		if (pp) {
			os << "#" << n << " - " << pp << " statement type = " << typeid(*pp).name() << "\n";
		} else {
			os << "#" << n << " - " << pp << " empty statement\n";
		}
		++n;
	}
	os << "]";
	return os;
}
