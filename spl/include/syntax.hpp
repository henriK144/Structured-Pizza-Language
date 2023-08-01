#pragma once
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include <map>
#include <algorithm>

inline void convertToUppercase(std::string& s) // converts a string to uppercase in place
{
	for (char& c : s) { 
		c = static_cast<char>(std::toupper(static_cast<unsigned char>(c))); 
	};
} 

// Defines the language keywords and translation tables from strings into keywords

enum class Keyword
{ // The special words recognized by the program as defining a statement
	BADPARSE = 0,

	PIZZA,
	SESSION,

	ADD,
	ALTER,
	AS,
	CHEESE,
	CRUST,
	DEMOCRACY,
	DETAILS,
	END,
	FOR,
	FROM,
	LOAD,
	NAME,
	QUIT,
	REMOVE,
	RESET,
	SAUCE,
	SAVE,
	SELECT,
	SET,
	START,
	SUBVERT,
	TO,
	TOP,
	TOPPING,
	VIEW,
	VOTE,
	VOTES
};

class Delimiter // The semicolon which separates statements 
{}; 

template<typename T>
using TransTable = std::map<std::string, T>; // A transtable is a mapping from strings to keyword types (T)

// we should maybe use a concept to constrain this and the function below
// but for now the duck typing suffices

template<typename T>
T translate(std::string s, const TransTable<T>& tbl)
{ // Translates a string into a keyword type
	convertToUppercase(s); // Keywords are not case-sensitive; they are converted to uppercase by the interpreter
	try {
		return tbl.at(s); // Look up the actual keyword and return it
	} catch (std::out_of_range&) {
		return static_cast<T>(0); // Return whatever enumerator is zero (i.e. T::UNSPECIFIED or T::BADPARSE)
	}
}

inline TransTable<Keyword> keyTrans
{
	{"PIZZA", Keyword::PIZZA},
	{"SESSION", Keyword::SESSION},

	{"ADD", Keyword::ADD},
	{"ALTER", Keyword::ALTER},
	{"AS", Keyword::AS},
	{"CHEESE", Keyword::CHEESE},
	{"CRUST", Keyword::CRUST},
	{"DEMOCRACY", Keyword::DEMOCRACY},
	{"DETAILS", Keyword::DETAILS},
	{"END", Keyword::END},
	{"FOR", Keyword::FOR},
	{"FROM", Keyword::FROM},
	{"LOAD", Keyword::LOAD},
	{"NAME", Keyword::NAME},
	{"QUIT", Keyword::QUIT},
	{"REMOVE", Keyword::REMOVE},
	{"RESET", Keyword::RESET},
	{"SAUCE", Keyword::SAUCE},
	{"SAVE", Keyword::SAVE},
	{"SELECT", Keyword::SELECT},
	{"SET", Keyword::SET},
	{"START", Keyword::START},
	{"SUBVERT", Keyword::SUBVERT},
	{"TO", Keyword::TO},
	{"TOP", Keyword::TOP},
	{"TOPPING", Keyword::TOPPING},
	{"VIEW", Keyword::VIEW},
	{"VOTE", Keyword::VOTE},
	{"VOTES", Keyword::VOTES}
};

inline TransTable<Crust> crustTrans
{
	{"STANDARD", Crust::STANDARD},
	{"THINCRUST", Crust::THINCRUST},
	{"THICKCRUST", Crust::THICKCRUST},
	{"GLUTENFREE", Crust::GLUTENFREE}
};

inline TransTable<Sauce> sauceTrans
{
	{"NONE", Sauce::NONE},
	{"TOMATO", Sauce::TOMATO},
	{"PESTO", Sauce::PESTO},
	{"OLIVEOIL", Sauce::OLIVEOIL},
	{"BBQ", Sauce::BBQ}, {"BARBECUE", Sauce::BBQ}
};

inline TransTable<Cheese> cheeseTrans
{
	{"NONE", Cheese::NONE},
	{"MOZZARELLA", Cheese::MOZZARELLA},
	{"DOUBLEMOZZARELLA", Cheese::DOUBLEMOZZARELLA},
	{"TRIPLEMOZZARELLA", Cheese::TRIPLEMOZZARELLA},
	{"DAIRYFREE", Cheese::DAIRYFREE}
};

inline TransTable<Topping> topTrans
{
	{"PEPPERONI", Topping::PEPPERONI},
	{"BACON", Topping::BACON},
	{"SOPPRESSATA", Topping::SOPPRESSATA},
	{"SAUSAGE", Topping::SAUSAGE},
	{"ANCHOVIES", Topping::ANCHOVIES},
	{"CHICKEN", Topping::CHICKEN},
	{"HAM", Topping::HAM},
	{"BEEF", Topping::BEEF},
	{"CHORIZO", Topping::CHORIZO},

	{"ARTICHOKE", Topping::ARTICHOKE},
	{"BLACKOLIVES", Topping::BLACKOLIVES}, {"OLIVES", Topping::BLACKOLIVES},
	{"GREENOLIVES", Topping::GREENOLIVES},
	{"BROCCOLI", Topping::BROCCOLI},
	{"BRUSCHETTA", Topping::BRUSCHETTA},
	{"GARLIC", Topping::GARLIC},
	{"GREENPEPPERS", Topping::GREENPEPPERS},
	{"ROASTEDREDREPPERS", Topping::ROASTEDREDPEPPERS},
	{"HOTPEPPERS", Topping::HOTPEPPERS},
	{"HOTHONEY", Topping::HOTHONEY},
	{"MUSHROOMS", Topping::MUSHROOMS}, 
	{"PORTOBELLOMUSHROOMS", Topping::PORTOBELLOMUSHROOMS},
	{"SPANISHONION", Topping::SPANISHONION},
	{"REDONION", Topping::REDONION}, {"ONION", Topping::REDONION},
	{"PINEAPPLE", Topping::PINEAPPLE},
	{"PLANTBASEDPEPPERONI", Topping::PLANTBASEDPEPPERONI},
	{"SPINACH", Topping::SPINACH},
	{"TOMATO", Topping::TOMATO},
	{"ZUCCHINI", Topping::ZUCCHINI},

	{"ASIAGO", Topping::ASIAGO},
	{"CHEDDAR", Topping::CHEDDAR},
	{"FETA", Topping::FETA},
	{"GOATCHEESE", Topping::GOATCHEESE},
	{"PARMESAN", Topping::PARMESAN}
};

inline TransTable<ToppingPosition> posTrans
{
	{"LEFT", ToppingPosition::LEFT},
	{"RIGHT", ToppingPosition::RIGHT},
	{"ALL", ToppingPosition::ALL}
};
