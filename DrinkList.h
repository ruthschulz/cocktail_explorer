#ifndef DRINKLIST_H_
#define DRINKLIST_H_

#pragma once
#include "Drink.h"
#include <utility>

class DrinkList
{
	std::map<std::string, Drink, cicompare> drinks;
public:
	std::map<std::string, Drink, cicompare> possibleDrinks;
	std::set<std::string> ingredients;
	std::set<std::string> selectedIngr;
	std::map<char, int> alphabetInfo;


public:
	DrinkList();
	~DrinkList();
	int initDrinks(void);
	int setAllPossibleDrinks(void);
	int setPossibleAlphabetDrinks(int letter);
	int setPossibleDrinks(void);
	void setAlphabet(void);
	void clearSelected(void);
	char findLetter(int letterNum);
	void addSelectedIngredient(std::string name);
	void removeSelectedIngredient(std::string name);
};

#endif
