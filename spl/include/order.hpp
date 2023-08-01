#pragma once
#include <string>
#include "pizza.hpp"

// Describes the structure of a pizza order

struct PizzaOrder
{
	Pizza pizza;
	int votes;
	std::string name;

	PizzaOrder(Pizza p) : pizza{p}, votes{0} {}
	PizzaOrder(Pizza p, const std::string& n) : pizza{p}, votes{0}, name{n} {}
};

inline bool operator<(const PizzaOrder& po1, const PizzaOrder& po2) {
	return po1.votes < po2.votes; 
}

inline bool operator>(const PizzaOrder& po1, const PizzaOrder& po2) {
	return po1.votes > po2.votes; 
}
