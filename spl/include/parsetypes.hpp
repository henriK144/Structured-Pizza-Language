#pragma once
#include <string>
#include <vector>
#include "tokens.hpp"

using RawText = std::string; // These typedefs make the stages of the parsing process more explicit
using TokenSkeleton = std::vector<TokenPrototype>;
using TokenList = std::vector<Token>;
using TokenListList = std::vector<TokenList>;

using PizzaElementText = RawText; // By "pizza element", I mean the raw text {Cheese:DOUBLEMOZZARELLA} or {RIGHT:HOTHONEY}
using PizzaElementTextList = std::vector<PizzaElementText>;
using PizzaElementList = std::vector<PizzaElement>; 
using PizzaData = TokenData;
