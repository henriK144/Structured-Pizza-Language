#pragma once
#include <exception>
#include <string>
#include "tokens.hpp"

using namespace std::string_literals;

enum ErrorID // An enumeration of what particular errors can occur
{
	EOF_WHILE_PARSING = 1,
	UNRECOGNIZED_TOKEN,
	INVALID_STATEMENT,
	INVALID_KEYWORD,
	INVALID_INT,
	INVALID_STRING, 
	INVALID_TOKEN_TYPE,

	WRONG_PIZZA_COMPONENT,
	UNRECOGNIZED_PIZZA_ELEMENT,
	BAD_PIZZA_LITERAL,
	ILL_FORMED_PIZZA,
	DUPLICATE_TOPPING,
	AMBIGUOUS_NONE,
	MISSING_DELIMITER,
	EXPECTED_DIFFERENT_TOKEN
};

class BadInterp: public std::exception // constify all the token ptrs innit
{
protected:
	ErrorID error_id;
	Location loc;
	std::string tok_content;
	std::string message;

	BadInterp(ErrorID e, Location l, const std::string tkctnt, const std::string& msg) : error_id{e}, loc{l}, tok_content{tkctnt}, message{msg} {}
	BadInterp(ErrorID e, Location l, const std::string& msg) : BadInterp(e, l, "", msg) {}
public:
	virtual const char* what() { return "call report() for details"; }
	virtual std::string report() = 0;
	// In general, the report will be something like [Error type] + BadInterp::report()
	// Where report will likely be the contents of message and perhaps a hint for debugging
};

inline std::string BadInterp::report() 
{
	std::string reportMsg;

	reportMsg += " error ";
	if (!tok_content.empty()) reportMsg += std::string("with token " + tok_content + " ");
	reportMsg += "at line ";
	reportMsg += std::to_string(loc.line); 
	reportMsg += ", character ";
	reportMsg += std::to_string(loc.character); 
	reportMsg += " : ";
	reportMsg += message;
	reportMsg += " (Error ID ";
	reportMsg += std::to_string(static_cast<int>(error_id));
	reportMsg += ")";

	return reportMsg;
}

class BadPreprocess: public BadInterp
{
public:
	BadPreprocess(ErrorID e, Location l, const std::string& msg) : BadInterp(e, l, msg) {}
	virtual std::string report()
	{ return "Preprocessing" + BadInterp::report(); }
};

class BadTokenize: public BadInterp
{
public:
	BadTokenize(ErrorID e, Location l, const std::string& msg) : BadInterp(e, l, msg) {}
	virtual std::string report()
	{ return "Tokenization" + BadInterp::report(); }
};

class BadParse: public BadInterp
{
public:
	BadParse(ErrorID e, Location l, const std::string& t, const std::string& msg) : BadInterp(e, l, t, msg) {}
	virtual std::string report()
	{ return "Parsing" + BadInterp::report(); }
};

class BadLex: public BadInterp
{
public:
	BadLex(ErrorID e, Location l, const std::string& t, const std::string& msg) : BadInterp(e, l, t, msg) {}
	virtual std::string report()
	{ return "Lexing" + BadInterp::report(); }
};

class BadPizzaTokenize: public BadInterp
{
public:
	BadPizzaTokenize(ErrorID e, Location l, const std::string& msg) : BadInterp(e, l, msg) {}
	virtual std::string report()
	{ return "Pizza Tokenization" + BadInterp::report(); }
};

class BadPizzaParse: public BadInterp
{
public:
	BadPizzaParse(ErrorID e, Location l, const std::string& t, const std::string& msg) : BadInterp(e, l, t, msg) {}
	virtual std::string report()
	{ return "Pizza Parsing" + BadInterp::report(); }
};
