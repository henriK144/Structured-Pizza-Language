#pragma once
#include <variant>
#include <vector>
#include <iostream>
#include "pizza.hpp"
#include "syntax.hpp"

// Defines the tokens that make up a statement

enum class TokenType
{
	UNRECOGNIZED = 0,
	
	KEYWORD,
	INT,
	STRING,
	PIZZA,
	PIZZAELEMENT,
	DELIMITER
};

using PizzaElement = std::variant<Crust, Sauce, Cheese, ToppingArrangement>;
using PizzaSpecifier = std::variant<int, std::string, Pizza>;
using TokenValue = std::variant<std::monostate, Keyword, int, std::string, Pizza, PizzaElement, Delimiter>;
// note that tokentype's underlying number is exactly the index of the corresponding type

struct Location // Used for error diagnostics
{ 
	int line;
	int character;
};
inline Location nullLocation = {0,0};

struct TokenData // Contains the location and original text of a token in the source code
{  
	Location loc;
	std::string str;
};
inline TokenData nullData = {nullLocation, ""};

struct TokenPrototype // A string along with the type it should tokenize into
{  
	TokenType type;
	TokenData data;

	TokenPrototype(TokenType& tt, TokenData& dt) {
		type = tt;
		data = dt;
	} // a stupid hack to fix some god-awful template instantiation error 
};

struct Token
{
	TokenType type;
	TokenValue value;
	TokenData data;
};

struct SignatureToken
{
	TokenType type;
	Keyword specific_kw; // only matters if this represents a keyword
	bool pizza_specifier; // only matters if this represents a general pizza specifier

	static constexpr bool PSPEC = true;

	SignatureToken(TokenType tt) : type{tt}, specific_kw{Keyword::BADPARSE}, pizza_specifier{false} {} 
	// specifies a generic token by just its type
	SignatureToken(Keyword kw) : type{TokenType::KEYWORD}, specific_kw{kw}, pizza_specifier{false} {} 
	// specifies a generic token by just a keyword
	SignatureToken(bool pspec) : type{TokenType::UNRECOGNIZED}, specific_kw{Keyword::BADPARSE}, pizza_specifier{true} {}
	// specifies a generic token as being a pizza specifier
};

using Signature = std::vector<SignatureToken>;

inline bool patternMatchT(const Token& lhs, const SignatureToken& rhs)
{ // Checks if a particular token matches a generic signature token
	if (rhs.pizza_specifier) {
		return lhs.type == TokenType::STRING || lhs.type == TokenType::INT || lhs.type == TokenType::PIZZA;
	} else if (lhs.type == TokenType::KEYWORD) {
		return std::get<Keyword>(lhs.value) == rhs.specific_kw;
	} else {
		return lhs.type == rhs.type;
	}
};

using TokenList = std::vector<Token>;
inline bool patternMatch(TokenList tl, Signature sgn)
{
	return std::equal(tl.begin(), tl.end(), sgn.begin(), sgn.end(), patternMatchT);
};

inline PizzaSpecifier toktospec(const Token& tk)
{
	switch (tk.type) {
		case TokenType::INT:
			return std::get<int>(tk.value);
		case TokenType::STRING:
			return std::get<std::string>(tk.value);
		case TokenType::PIZZA:
			return std::get<Pizza>(tk.value);
		default:
			return 0; // provided the signature matching works, this will never happen
	}
}

inline std::ostream& operator<<(std::ostream& os, Location lc)
{ // for debugging
	os << "(" << lc.line << ", " << lc.character <<  ")";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Token& tk)
{ // for debugging
	os << "Type #" << static_cast<int>(tk.type) << ", source location " << tk.data.loc << ", content is \"" << tk.data.str << '\"';
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const TokenPrototype& tkp)
{ // for debugging
	os << "Type #" << static_cast<int>(tkp.type) << ", source location " << tkp.data.loc << ", content is \"" << tkp.data.str << '\"';
	return os;
}