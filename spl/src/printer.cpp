#include "printer.hpp"
#include "program.hpp"
#include <vector>

void printer::showPizza(Pizza p)
{
	std::vector<std::string> words;
	bool base_details = p.toppings.empty() || BDETAIL_FLAG;

	for (auto t : p.toppings) {
		words.push_back(detranslate(t.topping, topDetrans) + detranslate(t.position, posDetrans)); 
	}

	if (p.cheese != Cheese::MOZZARELLA || base_details) words.push_back(detranslate(p.cheese, cheeseDetrans));
	if (p.sauce != Sauce::TOMATO || base_details) words.push_back(detranslate(p.sauce, sauceDetrans));
	if (p.crust != Crust::STANDARD || base_details) words.push_back(detranslate(p.crust, crustDetrans));

	for (auto w = words.begin(); w != words.end(); ++w) {
		std::cout << *w;
		std::cout << (w == std::prev(words.end()) ? "." : ", ");
	}
	
}

void printer::showPizzaWithInfo(Pizza p)
{
	showPizza(p);
	std::cout << "Gluten-Free: " << btow(glutenFreeHuh(p)) << '\n';
	std::cout << "Dairy-Free: " << btow(dairyFreeHuh(p)) << '\n';
	std::cout << "Vegetarian: " << btow(vegetarianHuh(p)) << '\n';
	std::cout << "Vegan: " << btow(veganHuh(p)) << '\n';
}

void printer::showOrder(const PizzaOrder& po, unsigned int ordinal)
{
	if (ordinal) {
		if (po.name.empty()) {
			std::cout << "#" << ordinal << ": ";
		} else {
			std::cout << "#" << ordinal << " " << "\"" << po.name << "\"" << ": ";
		}
	} else {
		if (po.name.empty()) {
			;
		} else {
			std::cout << "\"" << po.name << "\"" << ": ";
		}
	}
	
	showPizza(po.pizza);
	std::cout << " " << "[" << po.votes << " votes]" << '\n';

}

void printer::showOrderWithInfo(const PizzaOrder& po, unsigned int ordinal)
{
	showOrder(po, ordinal);
	std::cout << "Gluten-Free: " << btow(glutenFreeHuh(po.pizza)) << '\n';
	std::cout << "Dairy-Free: " << btow(dairyFreeHuh(po.pizza)) << '\n';
	std::cout << "Vegetarian: " << btow(vegetarianHuh(po.pizza)) << '\n';
	std::cout << "Vegan: " << btow(veganHuh(po.pizza)) << '\n';
}

void printer::showSessionInfo(const OrderSession& os, bool pizza_deets)
{
	if (!os.session_name.empty()) {
		std::cout << os.session_name << ": ";
	}
	std::cout << os.orders.size() << " pizzas ordered.\n\n";
	for (unsigned int i = 0; i < os.orders.size(); ++i) {
		if (pizza_deets) {
			showOrderWithInfo(os.orders.at(i), i + 1);
		} else {
			showOrder(os.orders.at(i), i + 1);
		}
		std::cout << '\n';
	}
}

void printer::showTopOrders(const OrderSession& os, int n)
{
	std::cout << "Top " << n << " orders:\n";
	auto scoreboard = os.orders;
	auto nn = static_cast<unsigned int>(n);
	std::sort(scoreboard.begin(), scoreboard.end(), operator>);
	for (unsigned int i = 0; i < scoreboard.size() && i < nn; ++i) {
		showOrder(scoreboard.at(i), 0);
	}
}

void printer::reportSuccess()
{
	std::cout << "Statements executed successfully." << '\n';
}

void printer::reportError()
{
	std::cout << "Error encountered, execution halted." << '\n';
}

void printer::reportRuntimeError(const std::string& txt, ProgramState& ps)
{
	std::cout << "Statement #" << ps.programcounter << ": " << txt << '\n';
}

void printer::reportRuntimeError(const char* txt, ProgramState& ps)
{
	std::cout << "Statement #" << ps.programcounter << ": " << txt << '\n';
}

void printer::reportInterpError(BadInterp& error, ProgramState& ps)
{
	std::cout << error.report() << '\n';
}

void printer::lineBreak()
{
	std::cout << std::string(30, LINE_CHAR) << '\n';
}

void printer::REPLineBreak()
{
	std::cout << std::string(30, LINE_CHAR_R) << '\n' << printer::PROMPT;
}

void printer::copy(const std::string& txt)
{
	std::cout << txt << '\n';
}

void printer::copy(const char* txt)
{
	std::cout << txt << '\n';
}
