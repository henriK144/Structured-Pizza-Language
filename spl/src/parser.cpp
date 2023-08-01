#include "parser.hpp"
#include "interperrors.hpp"
#include <iterator>

using parser::Scanner;

Scanner::Scanner(std::string& sourcefile)
: loc(Location(1,1)), charptr(sourcefile.begin()), START(sourcefile.begin()), END(sourcefile.end())
{ ; }

Scanner::Scanner(std::string& sourcefile, std::string::iterator initpos, Location initloc)
: loc(initloc), charptr(initpos), START(sourcefile.begin()), END(sourcefile.end())
{ ; }

Scanner& Scanner::operator++() 
{ 
	if (charptr != END) {
		++loc.character; 
		if (*charptr == '\n') {
			++loc.line; 
			loc.character = 1;
		}
		++charptr; 
	}
	return *this;
}

Scanner& Scanner::advance()
{
	if (charptr != END) {
		++loc.character; 
		if (*charptr == '\n') {
			++loc.line; 
			loc.character = 1;
		}
		++charptr; 
	}
	return *this;
}

Scanner& Scanner::advanceWhile(const CharPredicate& cp)
{
	while (cp(*charptr) && charptr != END) {
		advance(); 
	}
	return *this;
}

Scanner& Scanner::advanceWhile(char ch)
{
	while (*charptr == ch && charptr != END) {
		advance(); 
	}
	return *this;
}

Scanner& Scanner::advanceUntil(const CharPredicate& cp)
{
	using FAIL = BadTokenize;
	auto begin_loc = this->loc;

	do {
		advance(); 
	} while (!cp(*charptr) && charptr != END);

	if (charptr == END)  {
		throw FAIL(EOF_WHILE_PARSING, begin_loc, "Reached EOF while parsing");
	}

	return *this;
}

Scanner& Scanner::advanceUntil(char ch)
{
	return advanceUntil(matches_char(ch));
}

char Scanner::operator*() { return *charptr; }
char Scanner::getChar() { return *charptr; }
std::string::iterator& Scanner::device() { return charptr; }

Scanner& Scanner::stamp(std::string::iterator& it) { it = charptr; return *this; }
Scanner& Scanner::stampNext(std::string::iterator& it) { it = std::next(charptr); return *this; }
Location Scanner::getLocation() { return loc; }

Scanner& Scanner::cutOut(std::string& text, std::string::iterator& frontIt, std::string::iterator& backIt)
{
	charptr = text.erase(frontIt, backIt);
	START = text.begin();
	END = text.end();
	return *this;
}

bool Scanner::atEOF() { return charptr == END; }

RawText parser::preprocess(RawText& raw)
{
	using FAIL [[maybe_unused]] = BadPreprocess;

	RawText rtext = raw; 
	
	char parenCloser = '\0'; // this avoids recognition of #s when they are inside a pair of parentheses
	bool inComment = false;

	Scanner pp_scan(rtext);
	auto comment_begin = rtext.begin();
	auto comment_end = rtext.begin();

	while (!pp_scan.atEOF()) {
		if (inComment) { // get to the end of the comment and remove it

			pp_scan.advanceUntil('\n');
			pp_scan.stamp(comment_end);
			pp_scan.cutOut(rtext, comment_begin, comment_end);
			++pp_scan;
			inComment = false;

		} else { 
			
			if (*pp_scan == '#' && !parenCloser) { // if a comment has begun, then flag it 
				pp_scan.stamp(comment_begin);
				inComment = true;
				continue;
			} else if (containsKey(parenPairs, *pp_scan) && !parenCloser) { // if a paren has begun, flag it
				parenCloser = parenPairs.at(*pp_scan);
			} else if (containsValue(parenPairs, *pp_scan) && parenCloser == *pp_scan) { // if a paren has ended, lower the flag
				parenCloser = '\0';
			}

			++pp_scan;
		}
	}

	// Note that if parenCloser is non-null at this point, then an unmatched parenthesis 
	// is probably screwing with the preprocessor and causing it to skip over a comment. 
	// This will cause a tokenization error when the tokenizer hits eof while expecting
	// a closing parenthesis, at which point a BadTokenize will be thrown.
	
	return rtext;
}

TokenSkeleton parser::tokenize(RawText& raw)
{
	using FAIL = BadTokenize;

	TokenSkeleton tstream;
	Scanner scan(raw);
	
	auto token_begin = raw.begin();
	auto token_end = token_begin;
	TokenData token_dt;
	TokenType token_tp;

	auto start_token = [&](Scanner& sc, TokenType tt) 
	{ 
		sc.stamp(token_begin);
		token_dt.loc = sc.getLocation();
		token_tp = tt;
	};

	auto terminate_token = [&](Scanner& sc)
	{
		sc.stamp(token_end);
		token_dt.str = std::string(token_begin, token_end);
		tstream.emplace_back(token_tp, token_dt);
	};

	scan.advanceWhile(isSpace); // skip leading whitespace

	while (!scan.atEOF()) { 

		if (isAlpha(*scan)) { // get keyword with skip_to_whitespace_or_semicolon

			start_token(scan, TokenType::KEYWORD);
			scan.advanceUntil(whitespaceOrSemicolon);
			terminate_token(scan);

		} else if (containsKey(parenPairs, *scan)) { // get int/string/pizza with skip_to_char (closeParen)

			start_token(scan, parenTypes.at(*scan));
			scan.advanceUntil(matches_char(parenPairs.at(*scan))), scan.advance();
			terminate_token(scan);

		} else if (*scan == ';') { // get delimiter by setting token_end to token_begin + 1

			start_token(scan, TokenType::DELIMITER);
			scan.advance();
			terminate_token(scan);

		} else { // get malformed token with skip_to_whitespace_or_semicolon, then throw bad tokenize: "unrecognized token"

			start_token(scan, TokenType::UNRECOGNIZED);
			scan.advanceUntil(whitespaceOrSemicolon);
			terminate_token(scan);

			throw FAIL(
				UNRECOGNIZED_TOKEN, 
				token_dt.loc, 
				"Unrecognized token \"" + tstream.back().data.str + "\""
			);

		}

		scan.advanceWhile(isSpace);
	}

	return tstream;
}

Token parser::lexToken(TokenPrototype& tokprot)
{
	using namespace std::literals;
	using FAIL = BadLex;

	Token tkn = {tokprot.type, std::monostate{ }, tokprot.data};
	std::string token_content = tokprot.data.str;

	switch (tokprot.type) {

		case TokenType::KEYWORD:
			tkn.value = translate(token_content, keyTrans); // get keyword from token_content
			if (std::get<Keyword>(tkn.value) == Keyword::BADPARSE) {
				throw FAIL (
					INVALID_KEYWORD,
					tokprot.data.loc,
					tkn.data.str,
					std::string("Invalid keyword \"" + token_content + "\"")
				);
			}
			break;

		case TokenType::INT:
			strip(token_content);
			try {
				tkn.value = std::stoi(token_content);
			} catch (std::invalid_argument&) {
				throw FAIL (
					INVALID_INT,
					tokprot.data.loc,
					tkn.data.str,
					std::string("Invalid integer \"" + token_content + "\" - unrecognized digit(s)")
				);
			} catch (std::out_of_range&) {
				throw FAIL (
					INVALID_INT,
					tokprot.data.loc,
					tkn.data.str,
					std::string("Invalid integer \"" + token_content + "\" - value causes overflow or underflow")
				);
			}
			break;

		case TokenType::STRING:
			strip(token_content); // get string from token_content
			if (!isASCIIstr(token_content)) {
				throw FAIL (
					INVALID_STRING,
					tokprot.data.loc,
					tkn.data.str,
					std::string("Invalid string \"" + token_content + "\" (contains non-ASCII characters)")
				);
			}
			tkn.value = token_content;
			break;

		case TokenType::PIZZA:
			//tkn.value = interpretPizza(token_content); // get pizza from token_content
			try {
				tkn.value = interpretPizza(token_content);
			} catch (CharMismatch& cm) {
				throw FAIL (
					EXPECTED_DIFFERENT_TOKEN,
					tokprot.data.loc,
					tkn.data.str,
					std::string("Expected a " + cm.first) + std::string(" before the next " + cm.second)
					// this would be less ugly with string literals so consider using those (?)
				);
			} catch (StringMismatch& sm) {
				throw FAIL (
					UNRECOGNIZED_PIZZA_ELEMENT,
					tokprot.data.loc,
					tkn.data.str,
					std::string(sm.first + "\"" + sm.second + "\"")
				);
			} catch (ErrantChar& ec) {
				throw FAIL(
					BAD_PIZZA_LITERAL,
					tokprot.data.loc,
					tkn.data.str,
					std::string(ec.first + "'" + std::string(1, ec.second) + "'")
				);
			} catch (std::string& s) {
				throw FAIL (
					ILL_FORMED_PIZZA,
					tokprot.data.loc,
					tkn.data.str,
					s
				);
			}	
			break;

		case TokenType::PIZZAELEMENT:
			//tkn.value = parseElement(token_content); // get pizza element from token_content
			try {
				tkn.value = parseElement(token_content);
			} catch (StringMismatch& sm) {
				throw FAIL(
					UNRECOGNIZED_PIZZA_ELEMENT,
					tokprot.data.loc,
					tkn.data.str,
					std::string(sm.first + "\"" + sm.second + "\"")
				);
			} catch (std::string& s) {
				throw FAIL(
					ILL_FORMED_PIZZA,
					tokprot.data.loc,
					tkn.data.str,
					s
				);
			}	
			break;

		case TokenType::DELIMITER:
			tkn.value = Delimiter{ };
			break;

		default:
			break;
	}

	if (std::holds_alternative<std::monostate>(tkn.value)) {
		throw FAIL (
			INVALID_TOKEN_TYPE,
			tokprot.data.loc,
			tkn.data.str,
			"Invalid token type (This should not happen; it is quite likely an error on the part of whoever implemented the interpreter. Consider letting them know.)"
		);
	}

	return tkn;

}

TokenList parser::lex(TokenSkeleton& tokskel)
{
	TokenList tlist;

	for (auto& bone : tokskel) {
		tlist.push_back(lexToken(bone));
	}

	return tlist;
}

std::unique_ptr<Statement> parser::parseStatement(TokenList& toks)
{
	using FAIL = BadParse;

	if (grammar.validSignature(toks)) {
		return grammar.makeStatement(toks);
	} else {
		throw FAIL(
			INVALID_STATEMENT,
			toks.front().data.loc,
			toks.front().data.str,
			"Invalid statement (You may be missing a semicolon, or the keywords might be in the wrong order.)"
		);
	}
}

Program parser::parse(TokenList& toklst)
{
	using FAIL = BadParse;

	Program prog;
	TokenListList statements;

	auto stmt_begin = toklst.begin();
	auto stmt_end = toklst.begin();

	while (stmt_begin != toklst.end()) {
	    stmt_end = std::find_if(stmt_begin, toklst.end(), [](Token t) { return t.type == TokenType::DELIMITER; });

	    if (stmt_end == toklst.end()) {
	    	throw FAIL(
	    		MISSING_DELIMITER,
	    		stmt_begin->data.loc,
	    		stmt_begin->data.str,
	    		"Expected a statement delimiter (;) before reaching EOF"
	    	);
	    }

	    statements.emplace_back(stmt_begin, stmt_end); // note that the delimiter is not actually part of the statement
	    stmt_begin = ++stmt_end;
	}

	for (auto& toksgmt : statements) { // we'll handle empty statements by appending a nullptr in place of a Statement*
		if (!toksgmt.empty()) {
			prog.push_back(std::move(parseStatement(toksgmt)));
		}
	} 

	return prog;
}

Program parser::interpret(RawText raw)
{
	if (raw.back() != '\n') raw += '\n'; // append a newline character, just like C++ does

	if (meetsPredicate(raw, isSpace)) return Program { }; // File is empty, produce empty program

	auto phase_1 = preprocess(raw);
	auto phase_2 = tokenize(phase_1);
	auto phase_3 = lex(phase_2);
	auto phase_4 = parse(phase_3);
	return phase_4;
	
}

// Pizza Stuff //

PizzaElementTextList parser::tokenizePizza(RawText& raw)
{
	strip(raw);
	PizzaElementTextList petl;
	Scanner scan(raw);
	
	auto elem_begin = raw.begin();
	auto elem_end = elem_begin;
	char expects = '{';

	if (meetsPredicate(raw, isSpace)) goto short_circuit; 

	// Moreover, we need to be checking that there is never a non-whitespace character between:
	// - The start and the first { (a)
	// - a } and the subsequent , (b)
	// - a , and the subsequent { (c)
	// if there is, throw something along the lines of "unexpected character x"
	// in essence, we can write
	// if (expects == '{' && !isSpace(*scan) || expects == ',' && !isSpace(*scan))

	while (!scan.atEOF()) {
		
		if (*scan == expects) {
			switch (expects) {
				case '{':
					scan.stamp(elem_begin);
					expects = '}';
					break;
				case '}':
					scan.stampNext(elem_end);
					petl.emplace_back(elem_begin, elem_end);
					expects = ',';
					break;
				case ',':
					expects = '{';
					break;	
			}
		} else if (isPizzaPunct(*scan) && *scan != expects) {
			throw CharMismatch(expects, *scan); 
			// throw the revelant information from here back up to lexToken,
			// then catch it there and use it to construct the full BadLex to rethrow
		} else if ((expects == '{' && !isSpace(*scan)) || (expects == ',' && !isSpace(*scan))) {
			throw ErrantChar("Unexpected character ", *scan);
		}	

		scan.advance();
	}

	if (expects == '{') throw std::string("Expected a '{' before the end of the pizza specifier");
	if (expects == '}') throw std::string("Expected a '}' before the end of the pizza specifier");

	short_circuit:

	return petl;
}

PizzaElement parser::parseElement(PizzaElementText& elemtext)
{

	strip(elemtext);

	if (elemtext.empty()) throw std::string("Empty element");
	int c = std::count(elemtext.begin(), elemtext.end(), ':');

	if (c == 1) { // explicit element type is on the left
		
		auto colit = std::find(elemtext.begin(), elemtext.end(), ':');

		auto elem_part = std::string(elemtext.begin(), colit);
		stripWhitespace(elem_part);
		auto elem_part_u = convertedToUppercase(elem_part);

		auto elem_which = std::string(colit + 1, elemtext.end());
		stripWhitespace(elem_which);
		auto elem_which_u = convertedToUppercase(elem_which);

		PizzaElement var_elem = Crust::UNSPECIFIED;

		if (elem_part_u == "CRUST") {
			var_elem = translate<Crust>(elem_which_u, crustTrans);
		} else if (elem_part_u == "SAUCE") {
			var_elem = translate<Sauce>(elem_which_u, sauceTrans);
		} else if (elem_part_u == "CHEESE") {
			var_elem = translate<Cheese>(elem_which_u, cheeseTrans);
		} else if (elem_part_u == "LEFT" || elem_part_u == "RIGHT" || elem_part_u == "ALL") {
			var_elem = ToppingArrangement{
			translate<ToppingPosition>(elem_part_u, posTrans), 
			translate<Topping>(elem_which_u, topTrans)
			};
		} else {
			throw StringMismatch(elem_part, "is not a pizza component");
		}

		if (validElement(var_elem)) {
			return var_elem; 
		} else {
			throw StringMismatch("Unrecognized topping or element ", elem_which);
		}
		

	} else if (c == 0) { // perform type inference
		
		stripWhitespace(elemtext);
		auto elemtext_u = convertedToUppercase(elemtext);

		if (elemtext_u == "NONE") {
			throw std::string("Ambiguous \"NONE\" (Did you mean no sauce or no cheese?)");
		} else if (Crust e; validEnum(e = translate(elemtext_u, crustTrans))) {
			return e;
		} else if (Sauce e; validEnum(e = translate(elemtext_u, sauceTrans))) {
			return e;
		} else if (Cheese e; validEnum(e = translate(elemtext_u, cheeseTrans))) {
			return e;
		} else if (Topping e; validEnum(e = translate(elemtext_u, topTrans))) {
			return ToppingArrangement{ToppingPosition::ALL, e};
		} else {
			throw StringMismatch("Unrecognized topping or element ", elemtext);
		}

	} else {
		throw std::string("Too many colons in pizza element specifier");
	}

}

PizzaElementList parser::parseElements(PizzaElementTextList& pzetl)
{
	PizzaElementList plist;

	for (auto& el : pzetl) {
		plist.push_back(parseElement(el));
	}

	return plist;
}

Pizza parser::parsePizza(PizzaElementList& elements)
{
	Pizza canvas = nullPizza;

	for (PizzaElement& pze : elements) {
		if (std::holds_alternative<ToppingArrangement>(pze)) {
			if (containsItem(canvas.toppings, std::get<ToppingArrangement>(pze))) {
				throw std::string("Duplicate topping"); 
			} else {
				canvas.toppings.push_back(std::get<ToppingArrangement>(pze));
			} 
		} else if (std::holds_alternative<Cheese>(pze)) {
			if (canvas.cheese == Cheese::UNSPECIFIED) {
				canvas.cheese = std::get<Cheese>(pze); 
			} else { 
				throw std::string("Redefinition of cheese is not allowed");
			}
		} else if (std::holds_alternative<Sauce>(pze)) {
			if (canvas.sauce == Sauce::UNSPECIFIED) {
				canvas.sauce = std::get<Sauce>(pze); 
			} else { 
				throw std::string("Redefinition of sauce is not allowed");
			}
		} else if (std::holds_alternative<Crust>(pze)) {
			if (canvas.crust == Crust::UNSPECIFIED) {
				canvas.crust = std::get<Crust>(pze); 
			} else { 
				throw std::string("Redefinition of crust is not allowed");
			}
		} else {
			throw std::string("???");
		}
	}

	if (canvas.cheese == Cheese::UNSPECIFIED) canvas.cheese = Cheese::MOZZARELLA;
	if (canvas.sauce == Sauce::UNSPECIFIED) canvas.sauce = Sauce::TOMATO;
	if (canvas.crust == Crust::UNSPECIFIED) canvas.crust = Crust::STANDARD;

	return canvas;
}

Pizza parser::interpretPizza(RawText raw)
{
	auto phase_1 = tokenizePizza(raw);
	auto phase_2 = parseElements(phase_1);
	auto phase_3 = parsePizza(phase_2);

	return phase_3;
}
