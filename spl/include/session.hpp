#pragma once
#include <string>
#include <vector>
#include "order.hpp"

// Defines the logic and operations for a pizza ordering session

class OrderSession
{
public: // This makes the implementation less of a headache
	std::vector<PizzaOrder> orders;
	std::string session_name;

public:
	OrderSession();
	OrderSession(int n);
	OrderSession(const std::string& n);
	OrderSession(const std::string& nm, int nmb);

	void add(const PizzaOrder& p);
	void rename(const std::string& n);

	std::vector<PizzaOrder>::iterator locateIt(int pizza_number); 
	std::vector<PizzaOrder>::iterator locateIt(const std::string& pizza_name);
	std::vector<PizzaOrder>::iterator locateIt(Pizza pizza_replica);

	unsigned int locateIndex(int pizza_number);
	unsigned int locateIndex(const std::string& pizza_name);
	unsigned int locateIndex(Pizza pizza_replica);

	bool located(int pizza_number); 
	bool located(const std::string& pizza_name);
	bool located(Pizza pizza_replica);

	PizzaOrder& locate(int pizza_number); // handle the try/catch stuff at runtime with located
	PizzaOrder& locate(const std::string& pizza_name);
	PizzaOrder& locate(Pizza pizza_replica);

	void vote(int pizza_number, int amount=1);
	void vote(const std::string& pizza_name, int amount=1);
	void vote(Pizza pizza_replica, int amount=1);

	//std::vector<PizzaOrder> tally(int winners);
	void resetVotes();
	void reset();
};