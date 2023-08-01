#include "session.hpp"
#include <algorithm>

OrderSession::OrderSession() 
{ ; }

OrderSession::OrderSession(int n) 
{ 
	orders.reserve(n);
}

OrderSession::OrderSession(const std::string& n) : session_name{n}
{ ; }

OrderSession::OrderSession(const std::string& nm, int nmb) : session_name{nm}
{
	orders.reserve(nmb);
}

void OrderSession::add(const PizzaOrder& po) { orders.push_back(po); }
void OrderSession::rename(const std::string& n) { session_name = n; }

std::vector<PizzaOrder>::iterator OrderSession::locateIt(int pizza_number)
{
	if (pizza_number > (int)orders.size() || pizza_number <= 0) {
		return orders.end(); // out of bounds 
	} else {
		return orders.begin() + pizza_number - 1;
	}
} 

std::vector<PizzaOrder>::iterator OrderSession::locateIt(const std::string& pizza_name)
{
	return std::find_if(
		orders.begin(),
		orders.end(),
		[&pizza_name](const PizzaOrder& po) { return po.name == pizza_name; });
}

std::vector<PizzaOrder>::iterator OrderSession::locateIt(Pizza pizza_replica)
{
	return std::find_if(
		orders.begin(),
		orders.end(),
		[&pizza_replica](const PizzaOrder& po) { return po.pizza == pizza_replica; });
}

unsigned int OrderSession::locateIndex(int pizza_number) { return pizza_number; }
unsigned int OrderSession::locateIndex(const std::string& pizza_name) { return locateIt(pizza_name) - orders.begin(); }
unsigned int OrderSession::locateIndex(Pizza pizza_replica) { return locateIt(pizza_replica) - orders.begin(); }

bool OrderSession::located(int pizza_number) { return locateIt(pizza_number) != orders.end(); }
bool OrderSession::located(const std::string& pizza_name) { return locateIt(pizza_name) != orders.end(); }
bool OrderSession::located(Pizza pizza_replica) { return locateIt(pizza_replica) != orders.end(); }

PizzaOrder& OrderSession::locate(int pizza_number) { return *locateIt(pizza_number); }
PizzaOrder& OrderSession::locate(const std::string& pizza_name) { return *locateIt(pizza_name); }
PizzaOrder& OrderSession::locate(Pizza pizza_replica) { return *locateIt(pizza_replica); }

void OrderSession::vote(int pizza_number, int amount) { locate(pizza_number).votes += amount; }
void OrderSession::vote(const std::string& pizza_name, int amount) { locate(pizza_name).votes += amount; }
void OrderSession::vote(Pizza pizza_replica, int amount) { locate(pizza_replica).votes += amount; }
/*
std::vector<PizzaOrder> OrderSession::tally(int winners)
{
	std::vector<PizzaOrder> winnerList(orders);
	std::sort(winnerList.begin(), winnerList.end(), operator>);
	winnerList.resize(winners, defaultPizza);
	return winnerList;
}
*/
void OrderSession::resetVotes()
{
	for (auto& order : orders) {
		order.votes = 0;
	}
}

void OrderSession::reset() { orders.clear(); }