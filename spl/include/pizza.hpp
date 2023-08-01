#pragma once
#include <vector>
#include <algorithm>

// Defines the structure of a Pizza
// (It would be kind of nifty to add customization for different pizza purveyors, but
// by default this is based on Pizza Nova's menu)

enum class Crust
{
	UNSPECIFIED = 0,

	STANDARD,
	THINCRUST,
	THICKCRUST,
	GLUTENFREE
};

enum class Sauce
{
	UNSPECIFIED = 0,

	NONE,
	TOMATO,
	PESTO,
	OLIVEOIL,
	BBQ
};

enum class Cheese
{
	UNSPECIFIED = 0,

	NONE,
	MOZZARELLA,
	DOUBLEMOZZARELLA,
	TRIPLEMOZZARELLA,
	DAIRYFREE
};

enum class Topping
{
	UNSPECIFIED = 0,

	PEPPERONI, // Meats
	BACON,
	SOPPRESSATA,
	SAUSAGE,
	ANCHOVIES,
	CHICKEN,
	HAM,
	BEEF,
	CHORIZO,

	ARTICHOKE = 100, // Veggies
	BLACKOLIVES,
	GREENOLIVES,
	BROCCOLI,
	BRUSCHETTA,
	GARLIC,
	GREENPEPPERS,
	ROASTEDREDPEPPERS,
	HOTPEPPERS,
	HOTHONEY,
	MUSHROOMS,
	PORTOBELLOMUSHROOMS,
	SPANISHONION,
	REDONION,
	PINEAPPLE,
	PLANTBASEDPEPPERONI,
	SPINACH,
	TOMATO,
	ZUCCHINI,

	ASIAGO = 200, // Cheeses
	CHEDDAR,
	FETA,
	GOATCHEESE,
	PARMESAN
};

enum class ToppingPosition
{
	UNSPECIFIED = 0,
	
	LEFT,
	RIGHT,
	ALL
};

struct ToppingArrangement
{
	ToppingPosition position;
	Topping topping;
};

struct Pizza
{
	Crust crust;
	Sauce sauce;
	Cheese cheese;
	std::vector<ToppingArrangement> toppings;
};

inline Crust defaultCrust = Crust::STANDARD;
inline Sauce defaultSauce = Sauce::TOMATO;
inline Cheese defaultCheese = Cheese::MOZZARELLA;
inline Pizza defaultPizza = {defaultCrust, defaultSauce, defaultCheese, std::vector<ToppingArrangement>{}};
inline Pizza nullPizza = {Crust::UNSPECIFIED, Sauce::UNSPECIFIED, Cheese::UNSPECIFIED, std::vector<ToppingArrangement>{}};

template<typename T>
inline bool containsItem(const std::vector<T>& v, T item)
{
	return std::find(v.begin(), v.end(), item) != v.end();
}

template<typename T>
inline bool sameContents(const std::vector<T>& v1, const std::vector<T>& v2)
{ // Checks if the v1 and v2 are equal up to reordering
	return std::is_permutation(
		v1.begin(),
		v1.end(),
		v2.begin(),
		v2.end());
}

inline bool operator==(const ToppingArrangement& ta1, const ToppingArrangement& ta2)
{
	return (ta1.topping == ta2.topping) && (ta1.position == ta2.position);
}

inline bool operator==(const Pizza& p1, const Pizza& p2)
{
	return (p1.crust == p2.crust) 
		&& (p1.sauce == p2.sauce)
		&& (p1.cheese == p2.cheese) 
		&& sameContents(p1.toppings, p2.toppings);
}

inline bool glutenFreeHuh(const Pizza& p) 
{ 
	return p.crust == Crust::GLUTENFREE; 
}

inline bool dairyFreeHuh(const Pizza& p) 
{ 
	return p.cheese == Cheese::DAIRYFREE; 
}

inline bool vegetarianHuh(const Pizza& p) 
{
	return std::all_of(
		p.toppings.begin(), 
		p.toppings.end(), 
		[](const ToppingArrangement& t) { return static_cast<int>(t.topping) >= 100; }); 
}

inline bool veganHuh(const Pizza& p) 
{ 
	return dairyFreeHuh(p) && vegetarianHuh(p); 
}
