#include "DrinkList.h"



DrinkList::DrinkList()
{
}


DrinkList::~DrinkList()
{
}

/*
 * void initDrinks (void)
 *
 * reads in drinks from a file (drinks.txt)
 * stores these drinks in an array, with the ingredients and amounts
 * then closes the file
 * file is of the format:
 * name of drink
 * ingredient: amount
 * ...
 * final ingredient: amount
 * end:
 * ...
 * last
 */
int DrinkList::initDrinks(void)
{
	std::ifstream a_file("drinks.txt", std::ifstream::in);
	int index{ 0 };
	if (a_file.is_open()) {
		while (a_file) {
			Drink newDrink;
			getline(a_file, newDrink.name);
			if (newDrink.name != "last") {
				std::string tmpName;
				int tmpNum;
				newDrink.validDrink = 1;
				getline(a_file, tmpName, ':');
				while (tmpName != "end") {
					newDrink.ingredient.insert(newDrink.ingredient.end(), tmpName);
					ingredients.insert(tmpName);
					a_file >> tmpNum;
					a_file >> std::ws;
					newDrink.amount.insert(newDrink.amount.end(), tmpNum);
					getline(a_file, tmpName, ':');
				}
				a_file >> std::ws;
				newDrink.numIngredients = newDrink.ingredient.size();
				std::vector<double>::iterator it1 = newDrink.amount.begin();
				std::string tmpString;
				int first = 1;
				for (std::vector<std::string>::iterator it = newDrink.ingredient.begin(); it != newDrink.ingredient.end(); it++) {
					if (first == 1) {
						first = 0;
						tmpString = std::to_string((int)*it1) + "ml " + *it;
						it1++;
						if (it1 == newDrink.amount.end())
							newDrink.textIngredients.insert(newDrink.textIngredients.end(), tmpString);
					}
					else {
						first = 1;
						tmpString = tmpString + ", " + std::to_string((int)*it1) + "ml " + *it;
						it1++;
						if (it1 != newDrink.amount.end())
							tmpString = tmpString + ",";
						newDrink.textIngredients.insert(newDrink.textIngredients.end(), tmpString);
					}
				}
				drinks[newDrink.name] = newDrink;
			}
			else {
				getline(a_file, newDrink.name);
			}
		}
		a_file.close();

		std::cout << "Number of Drinks: " << drinks.size() << std::endl;

		//for (std::map<std::string, Drink>::iterator it = drinks.begin(); it != drinks.end(); it++) {
		//	std::cout << it->first << ", ";
		//}

		std::cout << std::endl << "Number of Ingredients: " << ingredients.size() << std::endl;
		//for (std::set<std::string>::iterator it = ingredients.begin(); it != ingredients.end(); it++) {
		//	std::cout << *it << ", ";
		//}
	}
	return drinks.size();
}

/*
 * void setAllPossibleDrinks(void)
 *
 * compiles a posssible drinks array with all drinks
 */
int DrinkList::setAllPossibleDrinks(void)
{
	possibleDrinks.clear();
	for (std::map<std::string, Drink, cicompare>::iterator it = drinks.begin(); it != drinks.end(); it++) {
		if (it->second.validDrink == 1) {
			possibleDrinks[it->second.name] = it->second;
		}
	}

	std::cout << "Number of Possible Drinks: " << possibleDrinks.size() << std::endl;

	for (std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.begin(); it != possibleDrinks.end(); it++) {
		std::cout << it->first << ", ";
	}

	return possibleDrinks.size();
}

/*
 * void setPossibleAlphabetDrinks(void)
 *
 * compiles a posssible drinks array starting with letter
 */
int DrinkList::setPossibleAlphabetDrinks(int letter)
{
	possibleDrinks.clear();
	for (std::map<std::string, Drink, cicompare>::iterator it = drinks.begin(); it != drinks.end(); it++) {
		if ((tolower(it->first[0]) == ('a' + letter)) && (it->second.validDrink==1)) {
			possibleDrinks[it->second.name] = it->second;
		}
	}
	std::cout << "Number of Possible Drinks: " << possibleDrinks.size() << std::endl;

	for (std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.begin(); it != possibleDrinks.end(); it++) {
		std::cout << it->first << ", ";
	}
	return possibleDrinks.size();
}

/*
 * void setPossibleDrinks(void)
 *
 * compiles a posssible drinks array based on selected ingredients
 */
int DrinkList::setPossibleDrinks(void)
{
	int a, i, j, g;
	int numSelected, count;

	possibleDrinks.clear();
	j = 0;
	if (selectedIngr.size() > 0) {
		g = 0;
		for (std::map<std::string, Drink, cicompare>::iterator it = drinks.begin(); it != drinks.end(); it++) {
			if (it->second.validDrink) {
				count = 0;
				for (std::vector<std::string>::iterator itIngr = it->second.ingredient.begin();
					itIngr != it->second.ingredient.end(); itIngr++) {
					if (selectedIngr.find(*itIngr) != selectedIngr.end()) {
						count++;
					}
				}
				if (count == selectedIngr.size()) {
					possibleDrinks[it->first] = it->second;
				}
			}
		}
	}
	return possibleDrinks.size();
}


void DrinkList::setAlphabet(void)
{
	int i;
	int count;

	alphabetInfo.clear();
	std::cout << "set alphabet: ";
	std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.begin();
	for (i = 0; i < 26; i++) {
		count = 0;
		while ((it != possibleDrinks.end()) && (('a'+i) == tolower(it->first[0]))) {
			count++;
			it++;
		}
		// set count for letter
		if (count > 0)
			alphabetInfo['a'+i] = count;
	}
	std::cout << "Alphabet info size: " << alphabetInfo.size();
}

void DrinkList::clearSelected(void)
{
	selectedIngr.clear();
}

char DrinkList::findLetter(int letterNum)
{
	int i, count, letter;
	letter = 0;
	count = 0;
	if (letterNum <= alphabetInfo.size()) {
		std::map<char, int>::iterator it = alphabetInfo.begin();
		for (int i = 0; i < letterNum; i++) {
			it++;
		}
		return it->first;
	}
	else {
		return -1;
	}
}

void DrinkList::addSelectedIngredient(std::string name)
{
	selectedIngr.insert(selectedIngr.end(), name);
}

void DrinkList::removeSelectedIngredient(std::string name)
{
	selectedIngr.erase(name);
}
