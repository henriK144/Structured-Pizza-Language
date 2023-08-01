#pragma once
#include "statement.hpp"
#include "parsetypes.hpp"
#include "tokens.hpp"
#include "interperrors.hpp"
#include <functional>
#include <vector>
#include <utility>

using StatementAssembler = std::function<std::unique_ptr<Statement>(TokenList)>;

template <typename T, typename U>
using AssocList = std::vector<std::pair<T,U>>;
// Ordinarily we'd use a std::map, but their underlying structure requires the elements to be ordered

// Defines the language's grammar

class Grammar
{
private:
	AssocList<Signature, StatementAssembler> contents;

public:
	Grammar() = default;
	Grammar(const Grammar&) = default;


	Grammar& addSignature(const Signature& s, const StatementAssembler& a);
	AssocList<Signature, StatementAssembler>::iterator findSigPair(const TokenList& tl);
	bool validSignature(const TokenList& tl); // checks if a token list corresponds to a statement signature
	std::unique_ptr<Statement> makeStatement(const TokenList& tl); // forms a tokenlist into a statement
};

namespace grammars { // Invoke these functions to construct and return a grammar

	Grammar SPL_1();
	Grammar SPL_1_1();
	Grammar SPL_2();

	// etc...
}