#pragma once
#include <iostream>
#include <string>
#include "session.hpp"
#include "parsetypes.hpp"
#include "interperrors.hpp"
#include "syntax.hpp"

// Defines the printing functions for formatted output of a pizza, session data, interpreter errors, etc

struct ProgramState;

namespace printer { 

	void showPizza(Pizza p); // Prints a human-readable description of a pizza
	// e.g. "Ham, Pepperoni, Pineapple, Pesto Base, Thin Crust."

	void showPizzaWithInfo(Pizza p); // Prints a human-readable description of a pizza with added detail
	// e.g. "Spinach, Onion, Asiago Cheese, No Mozzarella Cheese, Olive Oil Base, Standard Crust" 
	// Gluten-Free: No
	// Dairy-Free: No
	// Vegetarian: Yes
	// Vegan: No

	void showOrder(const PizzaOrder& po, unsigned int ordinal); // Prints a human-readable description of a pizza order
	// e.g. "#15 "Oops! All Mushrooms": Mushrooms, Portobello Mushrooms, Double Cheese. [5 votes]"
	// or   "#6: Pepperoni, Anchovies, BBQ Base. [-2 votes]"

	void showOrderWithInfo(const PizzaOrder& po, unsigned int ordinal); // Prints a human-readable description of a pizza order
	// with added detail about the pizza

	void showSessionInfo(const OrderSession& os, bool pizza_deets); // Prints a human-readable description of the currently active session
	// e.g. "MathNews Prod Night 04/03/2024: 23 pizzas ordered."
	// *list all of the pizzas*

	void showTopOrders(const OrderSession& os, int n); // Prints the top n pizza orders, or all of them if os has fewer than n orders

	void reportSuccess(); // Reports that no runtime errors have occurred
	void reportError(); // Reports that a runtime error was encountered (which is recorded in statement.cpp)
	void reportRuntimeError(const std::string& txt, ProgramState& ps); // Prints txt on its own line
	void reportRuntimeError(const char* txt, ProgramState& ps);
	void reportInterpError(BadInterp& error, ProgramState& ps); // Reports an interpret-time error in detail
	void lineBreak(); // Prints out a solid line for better readability
	void REPLineBreak(); // Same but in a different style for the REPL

	void copy(const std::string& txt);
	void copy(const char* txt);

	inline bool BDETAIL_FLAG = false;
	inline char LINE_CHAR = '-';
	inline char LINE_CHAR_R = '*';
	inline std::string PROMPT = "> ";

	// now we need reverse transtables, uhmazin
	template<typename T>
	using DetransTable = std::map<T, std::string>; // A transtable is a mapping from strings to keyword types (T)

	template<typename T>
	std::string detranslate(T t, const DetransTable<T>& dtbl)
	{
		try {
			return dtbl.at(t); // Look up the actual keyword and return it
		} catch (const std::out_of_range&) {
			return "[ERROR - NOT FOUND]"; // Return error string
		}
	}

	inline DetransTable<Crust> crustDetrans
	{
		{Crust::STANDARD, "Standard Crust"}, // not explicitly stated
		{Crust::THINCRUST, "Thin Crust"},
		{Crust::THICKCRUST, "Thick Crust"},
		{Crust::GLUTENFREE, "Gluten-Free Crust"}
	};

	inline DetransTable<Sauce> sauceDetrans
	{
		{Sauce::NONE, "No Sauce"},
		{Sauce::TOMATO, "Tomato Sauce"}, // not explicitly stated
		{Sauce::PESTO, "Pesto Base"},
		{Sauce::OLIVEOIL, "Olive Oil Base"},
		{Sauce::BBQ, "BBQ Base"}
	};

	inline DetransTable<Cheese> cheeseDetrans
	{
		{Cheese::NONE, "No Cheese"}, 
		{Cheese::MOZZARELLA, "Mozzarella Cheese"},
		{Cheese::DOUBLEMOZZARELLA, "Double Mozzarella Cheese"},
		{Cheese::TRIPLEMOZZARELLA, "Triple Mozzarella Cheese"},
		{Cheese::DAIRYFREE, "Dairy-Free Cheese"}
	};	
	
	inline DetransTable<Topping> topDetrans
	{
		{Topping::PEPPERONI, "Pepperoni"},
		{Topping::BACON, "Bacon"},
		{Topping::SOPPRESSATA, "Soppressata"},
		{Topping::SAUSAGE, "Sausage"},
		{Topping::ANCHOVIES, "Anchovies"},
		{Topping::CHICKEN, "Chicken"},
		{Topping::HAM, "Ham"},
		{Topping::BEEF, "Beef"},
		{Topping::CHORIZO, "Chorizo"},

		{Topping::ARTICHOKE, "Artichoke"},
		{Topping::BLACKOLIVES, "Black Olives"},
		{Topping::GREENOLIVES, "Green Olives"},
		{Topping::BROCCOLI, "Broccoli"},
		{Topping::BRUSCHETTA, "Bruschetta"},
		{Topping::GARLIC, "Garlic"},
		{Topping::GREENPEPPERS, "Green Peppers"},
		{Topping::ROASTEDREDPEPPERS, "Roasted Red Peppers"},
		{Topping::HOTPEPPERS, "Hot Peppers"},
		{Topping::HOTHONEY, "Hot Honey"},
		{Topping::MUSHROOMS, "Mushrooms"}, 
		{Topping::PORTOBELLOMUSHROOMS, "Portobello Mushrooms"},
		{Topping::SPANISHONION, "Onion"},
		{Topping::REDONION, "Red Onion"},
		{Topping::PINEAPPLE, "Pineapple"},
		{Topping::PLANTBASEDPEPPERONI, "Plant-Based Pepperoni"},
		{Topping::SPINACH, "Spinach"},
		{Topping::TOMATO, "Tomato"},
		{Topping::ZUCCHINI, "Zucchini"},

		{Topping::ASIAGO, "Asiago Cheese"},
		{Topping::CHEDDAR, "Cheddar Cheese"},
		{Topping::FETA, "Feta Cheese"},
		{Topping::GOATCHEESE, "Goat Cheese"},
		{Topping::PARMESAN, "Parmesan Cheese"}
	};

	inline DetransTable<ToppingPosition> posDetrans
	{
		{ToppingPosition::LEFT, " (Left)"},
		{ToppingPosition::RIGHT, " (Right)"},
		{ToppingPosition::ALL, ""},
	};

	inline std::string btow(bool b) { return b ? "Yes" : "No"; } // btow means "bool to word"
}
