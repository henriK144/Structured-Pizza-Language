#pragma once

#include "parsetypes.hpp"
#include "statement.hpp"
#include "tokens.hpp"
#include "grammar.hpp"
#include <map>
#include <functional>

// Defines the functions that parse raw text into tokens and their values

namespace parser {

	template<typename T>
	using Predicate = std::function<bool(T)>;
	using CharPredicate = Predicate<char>;

	template<typename T>
	using Mismatch = std::pair<T,T>; // for errors
	using CharMismatch = Mismatch<char>;
	using StringMismatch = Mismatch<std::string>;
	using ErrantChar = std::pair<std::string, char>;

	class Scanner // An object used to analyze and interpret program source code
	{
	private:
		Location loc;
		std::string::iterator charptr;

		std::string::iterator START;
		std::string::iterator END;

	public:
		Scanner(std::string& sourcefile); // Puts the scanner at the beginning of the code
		Scanner(std::string& sourcefile, std::string::iterator initpos, Location initloc); // Puts the scanner at initpos
		
		Scanner& operator++(); // Moves to the next character
		Scanner& advance(); // Same as operator++()
		Scanner& advanceWhile(const CharPredicate& cp); // Moves forward while the cp is true on the character being scanned
		Scanner& advanceWhile(char ch); // Moves forward while the character being scanned is ch
		Scanner& advanceUntil(const CharPredicate& cp); // Moves forward until the cp is true on the character being scanned
		Scanner& advanceUntil(char ch); // Moves forward until the character being scanned is ch

		// Note that skip_until always advances the scanner by at least one; this is primarily so that the effect of 
		// "skip until you find char x" is "skip until you find the next instance of char x after this one"
		// the same principle applies for getting to the whitespace/semicolon at the end of a keyword
		// Conversely, skip_while uses a standard while loop instead of a do-while, so that
		// "skip while you have whitespace" is "if there happens to be whitespace then gloss over it, otherwise do nothing"
		
		char operator*(); // Returns the character being scanned
		char getChar(); // Same as operator*
		std::string::iterator& device(); // returns a reference to the underlying string iterator for low level control
		Scanner& stamp(std::string::iterator& it); // Places it at the current scan location
		Scanner& stampNext(std::string::iterator& it); // Places it at the current scan location + 1
		Scanner& cutOut(std::string& text, 
						std::string::iterator& frontIt, 
						std::string::iterator& backIt); // Cuts out the text between the Its
		Location getLocation(); // Returns the current (line, character) location in the source code
		bool atEOF(); // Returns true iff the scanner is at EOF (i.e. the string's end iterator)
	};

	RawText preprocess(RawText& raw); // Removes comments (more preprocessor features like macros may be added in the future)

	TokenSkeleton tokenize(RawText& raw); // Turns text into a list of prototypes to be lexed

	Token lexToken(TokenPrototype& tokprot); // Turns a token prototype into an actual token
	TokenList lex(TokenSkeleton& tokskel); // Turns a list of token prototypes into a list of actual tokens

	std::unique_ptr<Statement> parseStatement(TokenList& toks); // Forms a sub-list of tokens into a statement
	Program parse(TokenList& toklst); // Forms an entire program's list of tokens into a list of statements

	Program interpret(RawText raw); // Does all of the above steps, converting raw text into an executable program
	// (It takes its input by value, leaving the original unmodified)

	PizzaElementTextList tokenizePizza(RawText& raw); // Converts text into a list of pizza elements
	PizzaElement parseElement(PizzaElementText& elemtext); // Converts text representing an element into the actual element
	PizzaElementList parseElements(PizzaElementTextList& pzetl);
	Pizza parsePizza(PizzaElementList& elements); // Combines a list of pizza elements into an actual pizza
	Pizza interpretPizza(RawText raw); // Does all of the above steps (also passes by value)

	inline Grammar grammar = grammars::SPL_1(); // The grammar which is currently being used

	inline CharPredicate isSpace = [](char ch) // (the C versions are scuffed)
	{
		return ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' || ch == '\v'; 
	};

	inline CharPredicate isAlpha = [](char ch)
	{
		return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z');
	};

	inline CharPredicate isASCII = [](char ch) 
	{ 
        return static_cast<unsigned char>(ch) <= 127; 
    };

    inline CharPredicate isPizzaPunct = [](char ch)
    {
    	return ch == '{' || ch == '}' || ch == ',';
    };

	inline CharPredicate matches_char(char ch)
	{
		return [=](char hc) { return ch == hc; };
	}

	inline CharPredicate negate(const CharPredicate& cp)
	{
		return [=](char hc) { return !cp(hc); };
	}

	inline CharPredicate conjoin(const CharPredicate& cp1, const CharPredicate& cp2)
	{
		return [=](char hc) { return cp1(hc) && cp2(hc); };
	}

	inline CharPredicate disjoin(const CharPredicate& cp1, const CharPredicate& cp2)
	{
		return [=](char hc) { return cp1(hc) || cp2(hc); };
	}

	inline CharPredicate whitespaceOrSemicolon = disjoin(isSpace, matches_char(';'));

	inline CharPredicate validSourceChar = conjoin(isASCII, negate(matches_char('\f')));

	inline std::map<char,char> parenPairs // The pairs of parentheses that can occur in spl source code
	{
		{'(', ')'},
		{'{', '}'},
		{'[', ']'},
		{'\"', '\"'}
	};

	inline std::map<char,TokenType> parenTypes // The types of parentheses that can occur in spl source code
	{
		{'(', TokenType::INT},
		{'{', TokenType::PIZZAELEMENT},
		{'[', TokenType::PIZZA},
		{'\"', TokenType::STRING}
	};

	template<typename T, typename U>
	inline bool containsKey(const std::map<T,U>& m, T t) // checks if a map contains a key
	{
		return std::any_of(m.begin(), m.end(), [=](auto pair) { return pair.first == t; });
	}

	template<typename T, typename U>
	inline bool containsKey(const std::map<T,U>& m, const Predicate<T>& tp) // checks if a map contains a key
	{
		return std::any_of(m.begin(), m.end(), [=](auto pair) { return tp(pair.first); });
	}

	template<typename T, typename U>
	inline bool containsValue(const std::map<T,U>& m, U u) // checks if a map contains a value
	{
		return std::any_of(m.begin(), m.end(), [&](auto pair) { return pair.second == u; });
	}

	template<typename T, typename U>
	inline bool containsValue(const std::map<T,U>& m, const Predicate<U>& up) // checks if a map contains a value
	{
		return std::any_of(m.begin(), m.end(), [&](auto pair) { return up(pair.second); });
	}

	inline void convertToUppercase(std::string& s) // converts a string to uppercase in place
	{
		for (char& c : s) { 
			c = static_cast<char>(std::toupper(static_cast<unsigned char>(c))); 
		};
	}

	inline std::string convertedToUppercase(std::string s) // converts a string to uppercase and returns it as a copy
	{
		for (char& c : s) { 
			c = static_cast<char>(std::toupper(static_cast<unsigned char>(c))); 
		};
		return s;
	}

	inline void stripl(std::string& s) // removes the first character of a string
	{
		s.erase(s.begin());
	}

	inline void stripr(std::string& s) // removes the last character of a string
	{
		s.pop_back();
	}

	inline void strip(std::string& s) // removes the first and last characters of a string
	{
		stripl(s);
		stripr(s);
	}

	inline void stripWhitespace(std::string& s) // Removes whitespace from the front and back of a string
	{
		while (!s.empty() && isSpace(s.front())) stripl(s);
		while (!s.empty() && isSpace(s.back())) stripr(s);
	}

	inline bool meetsPredicate(const std::string& s, const CharPredicate& cp)
	{
		return std::all_of(s.begin(), s.end(), cp);
	}

	inline bool isASCIIstr(const std::string& s)
	{
    	return meetsPredicate(s, isASCII);
	}

	template<typename E>
	inline bool validEnum(E e) // Returns true if the underlying enum's int value is nonzero
	{
		return static_cast<bool>(static_cast<int>(e));
	}

	inline bool validElement(const PizzaElement& pze) // Returns true if the element is not "UNSPECIFIED"
	{
		if (std::holds_alternative<ToppingArrangement>(pze)) { // pze is a topping arrangement
			return 
			validEnum(std::get<ToppingArrangement>(pze).topping) 
			&& 
			validEnum(std::get<ToppingArrangement>(pze).position);
		} else if (std::holds_alternative<Cheese>(pze)) { // pze is a base element
			return validEnum(std::get<Cheese>(pze));
		} else if (std::holds_alternative<Sauce>(pze)) { // pze is a base element
			return validEnum(std::get<Sauce>(pze));
		} else { // pze is a base element
			return validEnum(std::get<Crust>(pze));
		}
	}

}