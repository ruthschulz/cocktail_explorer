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

	//std::cout << "Number of Possible Drinks: " << possibleDrinks.size() << std::endl;

	//for (std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.begin(); it != possibleDrinks.end(); it++) {
	//	std::cout << it->first << ", ";
	//}

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
	return possibleDrinks.size();
}

/*
 * void setPossibleDrinks(void)
 *
 * compiles a posssible drinks array based on selected ingredients
 */
int DrinkList::setPossibleDrinks(void)
{
	int j, g;
	int count;

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
}

void DrinkList::clearSelected(void)
{
	selectedIngr.clear();
}

int DrinkList::findLetter(int letterNum)
{
	int count, letter;
	letter = 0;
	count = 0;
	if (letterNum <= alphabetInfo.size()) {
		std::map<char, int>::iterator it = alphabetInfo.begin();
		for (int i = 0; i < letterNum; i++) {
			it++;
		}
		return (it->first - 'a');
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

Drink DrinkList::getDrink(std::string name) 
{
	std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.find(name);
	if (it != possibleDrinks.end()) {
		return it->second;
	}
	else {
		return Drink();
	}
}

std::string DrinkList::getNextDrinkName(std::string name)
{
	std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.find(name);
	if (it != possibleDrinks.end()) {
		it++;
		if (it != possibleDrinks.end()) {
			return it->first;
		}
		else {
			return possibleDrinks.begin()->first;
		}
	}
	else {
		return "null";
	}
}

std::string DrinkList::getLastDrinkName(std::string name)
{
	if (possibleDrinks.size() == 1) {
		return name;
	}
	std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.find(name);
	if (it != possibleDrinks.end()) {
		if (it != possibleDrinks.begin()) {
			it--;
		}
		else {
			it = possibleDrinks.end();
			it--;
		}
		return it->first;
	}
	else {
		return "null";
	}
}

void DrinkList::setVisible(std::string name, int visible) 
{
	std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.find(name);
	if (it!=possibleDrinks.end())
		it->second.visible = visible;
}

std::vector<DynamicBall> DrinkList::setMovingBalls(std::string name) 
{
	// new ball as center ball, need to make new set of moving balls, 
	// possibly with positions from old set of moving balls
	// first step is add center ball to newButtons
	double limit = 1.0;
	int numb = 0;
	int check = 0;
	double percent = 0.0;
	std::vector<DynamicBall> newButtons;
	while ((numb < 10) || (numb > 30)) {
		newButtons.clear();
		std::map<std::string, Drink, cicompare>::iterator tmpDrink = possibleDrinks.find(name);
		if (tmpDrink == possibleDrinks.end()) {
			//return movingButtons.size();
			return newButtons;
		}
		DynamicBall newBall = DynamicBall(0.0, 0.0, 0.0, 0.2,
			(double)tmpDrink->second.numIngredients / 5.0,
			0.0, 1.0 - ((double)tmpDrink->second.numIngredients / 5.0),
			0.0, 0.0, 0.0, 0.0, name, tmpDrink->second.textIngredients, 0);
		newButtons.insert(newButtons.begin(), newBall);
		for (std::map<std::string, Drink, cicompare>::iterator itDrinks = possibleDrinks.begin();
			itDrinks != possibleDrinks.end(); itDrinks++) {
			// Determine visible objects and attractions for first center
			int numSame = 0;
			int numIngr1 = 0;
			int numIngr2 = 0;
			for (std::vector<std::string>::iterator itIngr1 = tmpDrink->second.ingredient.begin();
				itIngr1 != tmpDrink->second.ingredient.end(); itIngr1++) {
				numIngr2 = 0;
				for (std::vector<std::string>::iterator itIngr2 = itDrinks->second.ingredient.begin();
					itIngr2 != itDrinks->second.ingredient.end(); itIngr2++) {
					if (*itIngr1 == *itIngr2) {
						numSame++;
					}
					numIngr2++;
				}
				numIngr1++;
			}
			percent = ((double)numSame / (double)numIngr2 + (double)numSame / (double)numIngr1) / 2.0;
			if (percent > limit)
			{
				//if center ball place at head of newButtons, else at end
				if (itDrinks->first != name) {
					// new ball
					DynamicBall newBall = DynamicBall(0.0, 0.0, 0.0, 0.2,
						(double)itDrinks->second.numIngredients / 5.0,
						0.0, 1.0 - ((double)itDrinks->second.numIngredients / 5.0),
						0.0, 0.0, 0.0, 0.0, itDrinks->second.name, itDrinks->second.textIngredients, 0);
					newButtons.insert(newButtons.end(), newBall);
				}
			}
		}
		//set the number of visible drinks
		numb = newButtons.size();
		if (possibleDrinks.size() < 10) {
			if (possibleDrinks.size() == newButtons.size())
				numb = 10;
		}
		if (check == 1) {
			numb = 10;
		}
		if (numb > 30) {
			// start with limit at 1.0 so can decrease from there
			// if here, have jumped from <10 to >30 in one step
			// so go back to previous list and exit
			limit = limit + 0.05;
			check = 1;
		}
		else {
			limit = limit - 0.05;
		}

		if (limit <= 0) {
			limit = 0.05;
			check = 1;
		}
		if (limit > 1) {
			limit = 1.0;
			check = 1;
		}
	}

	return newButtons;
}

std::string DrinkList::setRandomDrinkCenter(void)
{
	int number;

	double num = rand();
	num = (num / 32767) * possibleDrinks.size();
	number = (int)num;
	std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.begin();
	for (int i = 0; i < number; i++) {
		it++;
	}
	return it->first;
}

int DrinkList::numPossibleDrinks(void)
{
	return possibleDrinks.size();
}

int DrinkList::numSelectedIngr(void)
{
	return selectedIngr.size();
}

int DrinkList::numIngredients(void)
{
	return ingredients.size();
}

int DrinkList::numLettersUsed(void)
{
	return alphabetInfo.size();
}

std::string DrinkList::firstDrinkName(void)
{
	return possibleDrinks.begin()->first;
}

int DrinkList::nextLetter(int letter)
{
	if (alphabetInfo.size() == 0) {
		return -1;
	}
	int number = (letter + 1) % 26;
	while (alphabetInfo.find(number + 'a') == alphabetInfo.end()) {
		number = (number + 1) % 26;
	}
	return number;
}

int DrinkList::lastLetter(int letter)
{
	if (alphabetInfo.size() == 0) {
		return -1;
	}
	int number = (letter + 25) % 26;
	while (alphabetInfo.find(number + 'a') == alphabetInfo.end()) {
		number = (number + 25) % 26;
	}
	return number;
}

bool DrinkList::drinksWithLetter(int letter)
{
	return (alphabetInfo.find(letter + 'a') != alphabetInfo.end());
}

std::vector<Ball> DrinkList::setIngrBalls(int ingrSet)
{
	std::vector<Ball> ingrBalls;
	Ball newBall;
	int j = 0;
	int start, remaining;
	int numIngredientsDisplayed = 0;

	start = 20 * ingrSet;
	remaining = ingredients.size() - start;
	std::set<std::string>::iterator it = ingredients.begin();
	for (int i = 0; i < start; i++)
		it++;
	if (remaining <= 20) {
		numIngredientsDisplayed = remaining;
		for (int i = 0; i < remaining;i++) {
			newBall.text = *it;
			newBall.red = 1.0;
			if (i < remaining / 2) {
				newBall.posx = xleft;
				newBall.posy = yinit - (i + 2)*0.5;
			}
			else {
				newBall.posx = xright;
				newBall.posy = yinit - ((i + 2) - (remaining / 2))*0.5;
			}
			newBall.size = 0.2;
			if (selectedIngr.find(*it) != selectedIngr.end()) {
				newBall.red = 0.0;
				newBall.blue = 1.0;
				newBall.selected = 1;
			}
			else {
				newBall.red = 1.0;
				newBall.blue = 0.0;
				newBall.selected = 0;
			}
			newBall.textRed = 1.0;
			newBall.textBlue = 1.0;
			newBall.textGreen = 1.0;
			newBall.textSize = 0.8;
			ingrBalls.insert(ingrBalls.end(), newBall);
			it++;
		}
	}
	else {
		numIngredientsDisplayed = 20;
		for (int i = 0; i < 20;i++) {
			newBall.text = *it;
			newBall.red = 1.0;
			if (i < 10) {
				newBall.posx = xleft;
				newBall.posy = yinit - (i + 2)*0.5;
			}
			else {
				newBall.posx = xright;
				newBall.posy = yinit - ((i + 2) - (10))*0.5;
			}
			newBall.size = 0.2;
			if (selectedIngr.find(*it) != selectedIngr.end()) {
				newBall.red = 0.0;
				newBall.blue = 1.0;
				newBall.selected = 1;
			}
			else {
				newBall.red = 1.0;
				newBall.blue = 0.0;
				newBall.selected = 0;
			}
			newBall.textRed = 1.0;
			newBall.textBlue = 1.0;
			newBall.textGreen = 1.0;
			newBall.textSize = 0.8;
			ingrBalls.insert(ingrBalls.end(), newBall);
			it++;
		}
	}

	return ingrBalls;
}

std::string DrinkList::getSelectedIngr(int index)
{
	if (index > selectedIngr.size()) {
		return "null";
	}
	std::set<std::string>::iterator it = selectedIngr.begin(); 
	int i = 0;
	while ((it != selectedIngr.end()) && (i < index)) {
		it++;
		i++;
	}
	return *it;
}

