#ifndef DRINK_H_
#define DRINK_H_

#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <iostream>

class Drink
{
public:
	std::string name{ "null" };							// the name of the drink
	std::vector<std::string> ingredient;		// array of the ingredients
	int numIngredients{ 0 };							// number of ingredients
	std::vector<double> amount;					// array of amounts of the ingredients
	std::vector<std::string> textIngredients;	// ingredients in the drink to display
	int validDrink{ 0 };								// 1 if valid, 0 if not valid
	int visible{ 0 };								// if the drink is visible

public:
	Drink() {};
	Drink(std::string _name, std::vector<std::string> _ingr, int _numIngr, std::vector<double> _amount,
		std::vector<std::string> _textIngr, int _validDrink, int _visible) : name (_name), 
		ingredient(_ingr), numIngredients(_numIngr), amount(_amount), textIngredients(_textIngr),
		validDrink(_validDrink), visible(_visible) {};
	~Drink() {};
};

// used to sort the drinks in alphabetical order, case invarient
struct cicompare
{
	bool operator()(std::string const& lhsIn, std::string const& rhsIn) const
	{
		char const* lhs = lhsIn.c_str();
		char const* rhs = rhsIn.c_str();
		for (; *lhs != '\0' && *rhs != '\0'; ++lhs, ++rhs)
		{
			if (tolower(*lhs) != tolower(*rhs))
			{
				return (tolower(*lhs) < tolower(*rhs));
			}
			else if (*lhs != *rhs)
			{
				if (*(lhs + 1) == '\0' && *(rhs + 1) == '\0')
				{
					return (*lhs < *rhs);
				}
			}
		}
		return (tolower(*lhs) < tolower(*rhs));
	}
};

#endif
