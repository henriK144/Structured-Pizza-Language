#include "printer.hpp"
#include "program.hpp"

#include <iostream>

int StartSession::execute(ProgramState& ps)
{
	if (ps.session) {
		printer::reportRuntimeError("Error: A session is already active.", ps);
		return 1;
	} else {
		ps.session = OrderSession(name, reserves);
		return 0;
	}
}

int NameSession::execute(ProgramState& ps)
{
	if (ps.session)
	ps.session->session_name = name;
	return 0;
}

int EndSession::execute(ProgramState& ps)
{
	if (ps.session) {
		ps.session.reset();
		return 0;
	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}
	return 0;
}

int SaveSession::execute(ProgramState& ps)
{
	printer::copy("Session saving has not been implemented yet.");
	printer::lineBreak();
	return 0;
}

int LoadSession::execute(ProgramState& ps)
{
	printer::copy("Session loading has not been implemented yet.");
	printer::lineBreak();
	return 0;
}

int AddPizza::execute(ProgramState& ps)
{
	if (ps.session) {
		ps.session->orders.push_back(PizzaOrder{p, name});
		return 0;
	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}
	return 0;
}

int RemovePizza::execute(ProgramState& ps)
{
	if (ps.session) {
		std::visit([&](auto&& arg) { ps.session->orders.erase(ps.session->locateIt(arg)); }, pspec);
		return 0;
	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int ViewPizza::execute(ProgramState& ps)
{
	if (ps.session) {

		if (all) { // show all the pizzas
	
			if (details) {
				printer::showSessionInfo(*ps.session, true);
			} else {
				printer::showSessionInfo(*ps.session, false);
			} 
			printer::lineBreak();
		} else { // show only the selected pizza
	
			if (std::visit([&](auto&& arg) { return ps.session->located(arg); }, pspec)) {

				std::visit([&](auto&& arg) { 

					if (details) {
						printer::showOrderWithInfo(ps.session->locate(arg), ps.session->locateIndex(arg));
					} else {
						printer::showOrder(ps.session->locate(arg), ps.session->locateIndex(arg));
					} 
					printer::lineBreak();
				}, pspec);

				return 0;

			} else {
				printer::reportRuntimeError("Error: No such pizza has been ordered.", ps);
				return 1;
			}

		}
	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int VotePizza::execute(ProgramState& ps)
{
	if (ps.session) {

		if (std::visit([&](auto&& arg) { return ps.session->located(arg); }, pspec)) {
			std::visit([&](auto&& arg) { ps.session->vote(arg, n); }, pspec);
			return 0;
		} else {
			printer::reportRuntimeError("Error: No such pizza has been ordered.", ps);
			return 1;
		}

	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int SelectTopPizza::execute(ProgramState& ps)
{
	if (ps.session) {
		if (n < 0) {
			printer::reportRuntimeError("Error: Number of selections must be non-negative.", ps);
			return 1;
		} else {
			printer::showTopOrders(*ps.session, n);
		}

		printer::lineBreak();
		return 0;
	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int ResetSessionVotes::execute(ProgramState& ps)
{
	if (ps.session) {
		ps.session->resetVotes();
		return 0;
	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int ResetSession::execute(ProgramState& ps)
{
	if (ps.session) {
		ps.session->reset();
		return 0;
	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int AlterPizzaAdd::execute(ProgramState& ps)
{
	if (ps.session) {

		if (std::visit([&](auto&& arg) { return ps.session->located(arg); }, pspec)) {

			Pizza& pz = std::visit([&](auto&& arg) -> Pizza& { return ps.session->locate(arg).pizza; }, pspec);
			if (containsItem(pz.toppings, ta)) {
				printer::reportRuntimeError("Error: This topping arrangement is already on the pizza", ps);
				return 1;
			} else {
				pz.toppings.push_back(ta);
				return 0;
			}

		} else {
			printer::reportRuntimeError("Error: No such pizza has been ordered.", ps);
			return 1;
		}

	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int AlterPizzaRemove::execute(ProgramState& ps)
{
	if (ps.session) {

		if (std::visit([&](auto&& arg) { return ps.session->located(arg); }, pspec)) {
			Pizza& pz = std::visit([&](auto&& arg) -> Pizza& { return ps.session->locate(arg).pizza; }, pspec);
			std::erase(pz.toppings, ta);
			return 0;
		} else {
			printer::reportRuntimeError("Error: No such pizza has been ordered.", ps);
			return 1;
		}

	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int AlterPizzaSetCrust::execute(ProgramState& ps)
{
	if (ps.session) {

		if (std::visit([&](auto&& arg) { return ps.session->located(arg); }, pspec)) {
			Pizza& pz = std::visit([&](auto&& arg) -> Pizza& { return ps.session->locate(arg).pizza; }, pspec);
			pz.crust = c;
			return 0;
		} else {
			printer::reportRuntimeError("Error: No such pizza has been ordered.", ps);
			return 1;
		}

	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int AlterPizzaSetSauce::execute(ProgramState& ps)
{
	if (ps.session) {

		if (std::visit([&](auto&& arg) { return ps.session->located(arg); }, pspec)) {
			Pizza& pz = std::visit([&](auto&& arg) -> Pizza& { return ps.session->locate(arg).pizza; }, pspec);
			pz.sauce = s;
			return 0;
		} else {
			printer::reportRuntimeError("Error: No such pizza has been ordered.", ps);
			return 1;
		}

	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int AlterPizzaSetCheese::execute(ProgramState& ps)
{
	if (ps.session) {

		if (std::visit([&](auto&& arg) { return ps.session->located(arg); }, pspec)) {
			Pizza& pz = std::visit([&](auto&& arg) -> Pizza& { return ps.session->locate(arg).pizza; }, pspec);
			pz.cheese = ch;
			return 0;
		} else {
			printer::reportRuntimeError("Error: No such pizza has been ordered.", ps);
			return 1;
		}

	} else {
		printer::reportRuntimeError("Error: No session active.", ps);
		return 1;
	}

	return 0;
}

int Quit::execute(ProgramState& ps)
{
	ps.running = false;
	return 0;
}

int execute(Program& p, ProgramState& ps)
{
	printer::lineBreak();
	int err_code = 0;
	ps.programcounter = 1;
	for (const auto& s : p) { // Iterate over program and execute statements, halting on error
		if (!s) continue; // This is just in case someone tries to run a (logically) deleted program
		if ((err_code = s->execute(ps))) break; // execute the statement and terminate the loop if the error code is nonzero
		++ps.programcounter;
	}
	return err_code;
}
