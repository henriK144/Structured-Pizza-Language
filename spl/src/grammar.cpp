#include "grammar.hpp"

AssocList<Signature, StatementAssembler>::iterator Grammar::findSigPair(const TokenList& tl)
{
	return std::find_if(
		contents.begin(), 
		contents.end(), 
		[&](auto& pair) { return patternMatch(tl, pair.first); }
	);
}

Grammar& Grammar::addSignature(const Signature& s, const StatementAssembler& a)
{
	contents.emplace_back(s, a);
	return *this;
}

bool Grammar::validSignature(const TokenList& tl)
{
	return findSigPair(tl) != contents.end();
}

std::unique_ptr<Statement> Grammar::makeStatement(const TokenList& tl)
{
	auto x = findSigPair(tl);
	return (x->second)(tl);
}

/* Definition of the current Grammars */

Grammar grammars::SPL_1() { 

	using FAIL = BadParse; // Throw a BadParse if invalid

	Grammar g;
	constexpr int expectedPizzas = 30;

	g

	.addSignature({Keyword::START, Keyword::SESSION}, 
	[](const TokenList& tl) { // START SESSION
		return std::unique_ptr<Statement>(new StartSession("", expectedPizzas)); 
	})
	.addSignature({Keyword::START, Keyword::SESSION, Keyword::AS, TokenType::STRING}, 
	[](const TokenList& tl) { // START SESSION AS "string"
		return std::unique_ptr<Statement>(new StartSession(std::get<std::string>(tl[3].value), expectedPizzas));
	})
	.addSignature({Keyword::NAME, Keyword::SESSION, TokenType::STRING}, 
	[](const TokenList& tl) { // NAME SESSION "string"
		return std::unique_ptr<Statement>(new NameSession(std::get<std::string>(tl[2].value)));
	})
	.addSignature({Keyword::END, Keyword::SESSION},
	[](const TokenList& tl) { // END SESSION
		return std::unique_ptr<Statement>(new EndSession());
	})
	.addSignature({Keyword::SAVE, Keyword::SESSION, Keyword::TO, TokenType::STRING},
	[](const TokenList& tl) { // SAVE SESSION TO "string"
		return std::unique_ptr<Statement>(new SaveSession(std::get<std::string>(tl[3].value)));
	})
	.addSignature({Keyword::LOAD, Keyword::SESSION, Keyword::FROM, TokenType::STRING},
	[](const TokenList& tl) { // LOAD SESSION FROM "string"
		return std::unique_ptr<Statement>(new LoadSession(std::get<std::string>(tl[3].value)));
	})



	.addSignature({Keyword::ADD, Keyword::PIZZA, TokenType::PIZZA}, 
	[](const TokenList& tl) { // ADD PIZZA [pizza]
		return std::unique_ptr<Statement>(new AddPizza(std::get<Pizza>(tl[2].value), ""));
	})
	.addSignature({Keyword::ADD, Keyword::PIZZA, TokenType::PIZZA, Keyword::AS, TokenType::STRING},
	[](const TokenList& tl) { // ADD PIZZA [pizza] AS "string"
		return std::unique_ptr<Statement>(new AddPizza(std::get<Pizza>(tl[2].value), std::get<std::string>(tl[4].value)));
	})
	.addSignature({Keyword::REMOVE, Keyword::PIZZA, SignatureToken::PSPEC}, 
	[](const TokenList& tl) { // REMOVE PIZZA <pizza specifier>
		return std::unique_ptr<Statement>(new RemovePizza(toktospec(tl[2])));
	})
	.addSignature({Keyword::VIEW, Keyword::PIZZA, SignatureToken::PSPEC}, 
	[](const TokenList& tl) { // VIEW PIZZA <pizza specifier>
		return std::unique_ptr<Statement>(new ViewPizza(toktospec(tl[2]), false, false));
	})
	.addSignature({Keyword::VIEW, Keyword::PIZZA}, 
	[](const TokenList& tl) { // VIEW PIZZA
		return std::unique_ptr<Statement>(new ViewPizza(0, false, true));
	})
	.addSignature({Keyword::VIEW, Keyword::PIZZA, SignatureToken::PSPEC, Keyword::DETAILS}, 
	[](const TokenList& tl) { // VIEW PIZZA <pizza specifier> DETAILS
		return std::unique_ptr<Statement>(new ViewPizza(toktospec(tl[2]), true, false));
	})
	.addSignature({Keyword::VIEW, Keyword::PIZZA, Keyword::DETAILS}, 
	[](const TokenList& tl) { // VIEW PIZZA DETAILS
		return std::unique_ptr<Statement>(new ViewPizza(0, true, true));
	})


	.addSignature({Keyword::VOTE, Keyword::FOR, Keyword::PIZZA, SignatureToken::PSPEC}, 
	[](const TokenList& tl) { // VOTE FOR PIZZA <pizza specifier>
		return std::unique_ptr<Statement>(new VotePizza(toktospec(tl[3]), 1));
	})
	.addSignature({Keyword::VOTE, Keyword::FOR, Keyword::PIZZA, SignatureToken::PSPEC, TokenType::INT}, 
	[](const TokenList& tl) { // VOTE FOR PIZZA <pizza specifier> (int)
		return std::unique_ptr<Statement>(new VotePizza(toktospec(tl[3]), std::get<int>(tl[4].value)));
	})
	.addSignature({Keyword::SUBVERT, Keyword::DEMOCRACY, Keyword::FOR, Keyword::PIZZA, SignatureToken::PSPEC}, 
	[](const TokenList& tl) { // SUBVERT DEMOCRACY FOR PIZZA <pizza specifier>
		return std::unique_ptr<Statement>(new VotePizza(toktospec(tl[4]), 9999));
	})

	.addSignature({Keyword::SELECT, Keyword::TOP, TokenType::INT, Keyword::PIZZA},
	[](const TokenList& tl) { // SELECT TOP (int) PIZZA
		return std::unique_ptr<Statement>(new SelectTopPizza(std::get<int>(tl[2].value)));
	})

	.addSignature({Keyword::RESET, Keyword::SESSION, Keyword::VOTES}, 
	[](const TokenList& tl) { // RESET SESSION VOTES
		return std::unique_ptr<Statement>(new ResetSessionVotes());
	})
	.addSignature({Keyword::RESET, Keyword::SESSION}, 
	[](const TokenList& tl) { // RESET SESSION
		return std::unique_ptr<Statement>(new ResetSession());
	})


	.addSignature({Keyword::ALTER, Keyword::PIZZA, SignatureToken::PSPEC, Keyword::ADD, 
		Keyword::TOPPING, TokenType::PIZZAELEMENT},
	[](const TokenList& tl) { // ALTER PIZZA <pizza specifier> ADD TOPPING {pizza element}
		if (std::holds_alternative<ToppingArrangement>(std::get<PizzaElement>(tl[5].value))) {
			return std::unique_ptr<Statement>(new AlterPizzaAdd(toktospec(tl[2]), std::get<ToppingArrangement>(std::get<PizzaElement>(tl[5].value))));
		} else {
			throw FAIL(WRONG_PIZZA_COMPONENT, tl[5].data.loc, tl[5].data.str, "Expected a topping arrangement");
		}
	})
	.addSignature({Keyword::ALTER, Keyword::PIZZA, SignatureToken::PSPEC, Keyword::REMOVE, 
		Keyword::TOPPING, TokenType::PIZZAELEMENT}, 
	[](const TokenList& tl) { // ALTER PIZZA <pizza specifier> REMOVE TOPPING {pizza element}
		if (std::holds_alternative<ToppingArrangement>(std::get<PizzaElement>(tl[5].value))) {
			return std::unique_ptr<Statement>(new AlterPizzaRemove(toktospec(tl[2]), std::get<ToppingArrangement>(std::get<PizzaElement>(tl[5].value))));
		} else {
			throw FAIL(WRONG_PIZZA_COMPONENT, tl[5].data.loc, tl[5].data.str, "Expected a topping arrangement");
		}
	})
	.addSignature({Keyword::ALTER, Keyword::PIZZA, SignatureToken::PSPEC, Keyword::SET, 
		Keyword::CRUST, TokenType::PIZZAELEMENT}, 
	[](const TokenList& tl) { // ALTER PIZZA <pizza specifier> SET CRUST {pizza element}
		if (std::holds_alternative<Crust>(std::get<PizzaElement>(tl[5].value))) {
			return std::unique_ptr<Statement>(new AlterPizzaSetCrust(toktospec(tl[2]), std::get<Crust>(std::get<PizzaElement>(tl[5].value))));
		} else {
			throw FAIL(WRONG_PIZZA_COMPONENT, tl[5].data.loc, tl[5].data.str, "Expected a crust");
		}
	})
	.addSignature({Keyword::ALTER, Keyword::PIZZA, SignatureToken::PSPEC, Keyword::SET, 
		Keyword::SAUCE, TokenType::PIZZAELEMENT}, 
	[](const TokenList& tl) { // ALTER PIZZA <pizza specifier> SET SAUCE {pizza element}
		if (std::holds_alternative<Sauce>(std::get<PizzaElement>(tl[5].value))) {
			return std::unique_ptr<Statement>(new AlterPizzaSetSauce(toktospec(tl[2]), std::get<Sauce>(std::get<PizzaElement>(tl[5].value))));
		} else {
			throw FAIL(WRONG_PIZZA_COMPONENT, tl[5].data.loc, tl[5].data.str, "Expected a sauce");
		}
	})
	.addSignature({Keyword::ALTER, Keyword::PIZZA, SignatureToken::PSPEC, Keyword::SET, 
		Keyword::CHEESE, TokenType::PIZZAELEMENT}, 
	[](const TokenList& tl) { // ALTER PIZZA <pizza specifier> SET CHEESE {pizza element}
		if (std::holds_alternative<Cheese>(std::get<PizzaElement>(tl[5].value))) {
			return std::unique_ptr<Statement>(new AlterPizzaSetCheese(toktospec(tl[2]), std::get<Cheese>(std::get<PizzaElement>(tl[5].value))));
		} else {
			throw FAIL(WRONG_PIZZA_COMPONENT, tl[5].data.loc, tl[5].data.str, "Expected a cheese");
		}
	})


	.addSignature({Keyword::QUIT}, 
	[](const TokenList& tl) { // QUIT
		return std::unique_ptr<Statement>(new Quit());
	});
	

	return g;
}

Grammar grammars::SPL_1_1()
{
	Grammar g = SPL_1();

	// add new features...

	return g;
}

Grammar grammars::SPL_2()
{
	Grammar g = SPL_1_1();

	// add even more new features...

	return g;
}