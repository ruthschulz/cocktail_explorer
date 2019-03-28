#include "Draw.h"



Draw::Draw()
{
}


Draw::~Draw()
{
}

/*
 * void setVisibleDrinks(int ball)
 * sets up an array of balls that will be displayed
 * based on similarity to the center ball, which is an argument
 * for this function.
 * also sets up an array of similarities.
 */
int Draw::setVisibleDrinks(std::string name, std::map<std::string, Drink, cicompare>& possibleDrinks)
{
	// first element of movingButton is the center ball
	int g, a, b, z, count, pos, m, check;
	double count2, number, number2, limit, percent;
	int numb;
	int numVisibleDrinks{ 0 };

	if (movingButtons.size() > 0) {
		if (name == movingButtons.begin()->text) {
			// already center ball, don't make any changes
			return movingButtons.size();
		}
	}
	if (name == "null") {
		// no name, clear moving buttons
		movingButtons.clear();
		return movingButtons.size();
	}

	// new ball as center ball, need to make new set of moving balls, 
	// possibly with positions from old set of moving balls
	// first step is add center ball to newButtons
	limit = 1.0;
	numb = 0;
	check = 0;
	std::vector<DynamicBall> newButtons;
	while ((numb < 10) || (numb > 30)) {
		newButtons.clear();
		std::map<std::string,Drink,cicompare>::iterator tmpDrink = possibleDrinks.find(name);
		if (tmpDrink == possibleDrinks.end()) {
			return movingButtons.size();
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
					if (itDrinks->second.visible == 1) {
						// set position to old position
						std::vector<DynamicBall>::iterator itThisButton = movingButtons.begin();
						while (itThisButton->text != itDrinks->second.name)
							itThisButton++;
						newButtons.insert(newButtons.end(), *itThisButton);
					}
					else {
						// new ball
						DynamicBall newBall = DynamicBall(0.0, 0.0, 0.0, 0.2,
							(double)itDrinks->second.numIngredients / 5.0,
							0.0, 1.0 - ((double)itDrinks->second.numIngredients / 5.0),
							0.0, 0.0, 0.0, 0.0, itDrinks->second.name, itDrinks->second.textIngredients, 0);
						newButtons.insert(newButtons.end(), newBall);
					}
				}
			}
		}
		//set the number of visible drinks
		numVisibleDrinks = newButtons.size();
		numb = numVisibleDrinks;
		if (possibleDrinks.size() < 10) {
			if (possibleDrinks.size() == numVisibleDrinks)
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
	for (std::vector<DynamicBall>::iterator it = movingButtons.begin(); it != movingButtons.end(); it++) {
		std::map<std::string, Drink, cicompare>::iterator itDrink1 = possibleDrinks.find(it->text);
		if (itDrink1 != possibleDrinks.end()) {
			itDrink1->second.visible = 0;
		}
	}
	movingButtons.clear();
	for (std::vector<DynamicBall>::iterator it = newButtons.begin(); it != newButtons.end(); it++) {
		movingButtons.insert(movingButtons.end(), *it);
		// and set to visible in possibledrinks map
		std::map<std::string, Drink, cicompare>::iterator itDrink1 = possibleDrinks.find(it->text);
		if (itDrink1 != possibleDrinks.end()) {
			itDrink1->second.visible = 1;
		}
	}
	otherAttractions.clear();
	//determine attractions between the visible objects that are not the center
	for (std::vector<DynamicBall>::iterator it1 = movingButtons.begin(); it1 != movingButtons.end(); it1++) {
		std::map<std::string, Drink, cicompare>::iterator itDrink1 = possibleDrinks.find(it1->text);
		for (std::vector<DynamicBall>::iterator it2 = it1; it2 != movingButtons.end(); it2++) {
			std::map<std::string, Drink, cicompare>::iterator itDrink2 = possibleDrinks.find(it2->text);
			if (it1 != it2) {
				int numSame = 0;
				int numIngr1 = 0;
				int numIngr2 = 0;
				for (std::vector<std::string>::iterator itIngr1 = itDrink1->second.ingredient.begin();
					itIngr1 != itDrink1->second.ingredient.end(); itIngr1++) {
					numIngr2 = 0;
					for (std::vector<std::string>::iterator itIngr2 = itDrink2->second.ingredient.begin();
						itIngr2 != itDrink2->second.ingredient.end(); itIngr2++) {
						if (*itIngr1 == *itIngr2) {
							numSame++;
						}
						numIngr2++;
					}
					numIngr1++;
				}
				otherAttractions[std::make_pair(it1->text, it2->text)] = numSame / (double)(itDrink2->second.numIngredients);
				otherAttractions[std::make_pair(it2->text, it1->text)] = numSame / (double)(itDrink2->second.numIngredients);
			}
		}
	}
	
	//std::cout << "Number of Visible Drinks: " << movingButtons.size() << std::endl;

	//for (std::vector<DynamicBall>::iterator it = movingButtons.begin(); it != movingButtons.end(); it++) {
	//	std::cout << it->text << ", ";
	//}

	//std::cout << "Number of attractions: " << otherAttractions.size() << std::endl;
	//for (std::map<std::pair<std::string, std::string>, double>::iterator it = otherAttractions.begin();
	//	it != otherAttractions.end(); it++) {
	//	std::cout << it->first.first << " and " << it->first.second << ":" << it->second << ";";
	//}
	
	return numVisibleDrinks;
}

/*
 * void openingBalls (void)
 *
 * the opening screen for the interface
 */
void Draw::openingBalls(void)
{
	Ball newBall;
	buttons.clear();
	int j = 0;
	newBall.posx = x4;
	newBall.size = 0.7;
	newBall.red = 1.0;
	buttons.insert(buttons.end(), newBall);
	next = j;
	j++;
	newBall.posx = x4 - 0.28;
	newBall.size = 0.0;
	newBall.red = 0.0;
	newBall.text = "enter";
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	j++;
	newBall.posx = x4 - 1.2;
	newBall.posy = 2.0;
	newBall.textRed = 1.0;
	newBall.textGreen = 1.0;
	newBall.textBlue = 1.0;
	newBall.text = "Cocktail Explorer";
	newBall.textSize = 2.0;
	buttons.insert(buttons.end(), newBall);
	j++;
}

/*
 * void choicesBalls(void)
 *
 * the initial choices of how to choose the drink
 * ie select ingredients, view all, make your own
 */
void Draw::choicesBalls(void)
{
	Ball newBall;
	buttons.clear();
	int j = 0;
	newBall.text = "View All Drinks";
	newBall.size = 0.3;
	newBall.red = 0.5;
	newBall.blue = 0.5;
	newBall.posx = x1;
	newBall.posy = yinit - (j + 2);
	newBall.textRed = 1.0;
	newBall.textGreen = 1.0;
	newBall.textBlue = 1.0;
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	next2 = j;
	j++;
	newBall.posy = yinit - (j + 2);
	newBall.text = "Search by ingredients";
	buttons.insert(buttons.end(), newBall);
	next = j;
	j++;
	newBall.posy = yinit - (j + 2);
	newBall.text = "Search by starting letter";
	buttons.insert(buttons.end(), newBall);
	next4 = j;
	j++;
	newBall.text = "Back to Start";
	newBall.blue = 0.5;
	newBall.red = 0.0;
	newBall.posx = x1;
	newBall.posy = yinit - 7.3;
	newBall.size = 0.3;
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	back = j;
	j++;
}

/*
 * void showDrinks(void)
 *
 */
void Draw::showDrinks(bool drinksToShow, bool showLetters, char letter, std::map<char, int> &alphabetInfo)
{
	Ball newBall;
	buttons.clear();
	int j = 0;
	if (drinksToShow) {
		newBall.text = "Random";
		newBall.size = 0.25;
		newBall.red = 0.5;
		newBall.blue = 0.5;
		newBall.posx = x1 + 2.4;
		newBall.posy = yinit - 7.7;
		newBall.textRed = 1.0;
		newBall.textGreen = 1.0;
		newBall.textBlue = 1.0;
		newBall.textSize = 1.0;
		buttons.insert(buttons.end(), newBall);
		next = j;
		j++;
		newBall.text = "Next Drink";
		newBall.nextText.insert(newBall.nextText.end(), "(alphabetically)");
		newBall.posy = yinit - 6.3;
		newBall.textSize = 0.8;
		buttons.insert(buttons.end(), newBall);
		next2 = j;
		j++;
		newBall.text = "Last Drink";
		newBall.posy = yinit - 7.0;
		buttons.insert(buttons.end(), newBall);
		next3 = j;
		j++;
	}
	else {
		next = -1;
		next2 = -1;
		next3 = -1;
	}
	newBall.textRed = 1.0;
	newBall.textGreen = 1.0;
	newBall.textBlue = 1.0;
	newBall.textSize = 0.8;
	newBall.text = "Main menu";
	newBall.nextText.clear();
	newBall.blue = 0.5;
	newBall.red = 0.0;
	newBall.posx = x1 + 0.5;
	newBall.posy = yinit - 7.3;
	newBall.size = 0.3;
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	back = j;
	j++;

	if (showLetters) {
		newBall.text = "Drinks starting with";
		newBall.size = 0.0;
		newBall.posx = -5.8;
		newBall.posy = 1.35;
		newBall.textSize = 0.6;
		buttons.insert(buttons.end(), newBall);
		j++;
		newBall.text = "";
		newBall.blue = 0.5;
		newBall.red = 0.5;
		newBall.posx = -5.0;
		newBall.posy = 1.0;
		newBall.size = 0.25;
		buttons.insert(buttons.end(), newBall);
		lastDrinks = j;
		j++;
		newBall.size = 0.0;
		int a = ((letter-'a') +25)%26;
		while (alphabetInfo.find(a+'a') == alphabetInfo.end()){
			a = (a + 25) % 26;
		}
		char x = 'a' + a;
		std::string tmp(1, x);
		newBall.text = tmp;
		newBall.textSize = 1.0;
		newBall.posx = newBall.posx - 0.1;
		buttons.insert(buttons.end(), newBall);
		j++;
		newBall.text = "Drinks starting with";
		newBall.textSize = 0.6;
		newBall.size = 0.0;
		newBall.posx = 4.2;
		newBall.posy = 1.35;
		buttons.insert(buttons.end(), newBall);
		j++;
		newBall.text = "";
		newBall.blue = 0.5;
		newBall.red = 0.5;
		newBall.posx = 5.0;
		newBall.posy = 1.0;
		newBall.size = 0.25;
		buttons.insert(buttons.end(), newBall);
		moreDrinks = j;
		j++;
		newBall.size = 0.0;
		a = ((letter-'a') + 1) % 26;
		while (alphabetInfo.find(a+'a') == alphabetInfo.end()){
			a = (a + 1) % 26;
		}
		x = 'a' + a;
		std::string tmp2(1, x);
		newBall.text = tmp2;
		newBall.textSize = 1.0;
		newBall.posx = newBall.posx - 0.1;
		buttons.insert(buttons.end(), newBall);
		j++;
	}
}

void Draw::showAlphabet(std::map<char, int>& alphabetInfo)
{
	Ball newBall;
	buttons.clear();
	int j = 0;

	newBall.size = 0.3;
	newBall.red = 0.5;
	newBall.green = 0.0;
	newBall.blue = 0.5;
	newBall.textSize = 1.0;
	newBall.textRed = 1.0;
	newBall.textGreen = 1.0;
	newBall.textBlue = 1.0;

	int k = 0;
	for (int i = 0; i < 26; i++) {
		if (alphabetInfo.find(i+'a')!=alphabetInfo.end()){
			if (k < 7) {
				newBall.posx = x1 + k * 0.7;
				newBall.posy = yinit - 2.0;
			}
			else if (k < 14) {
				newBall.posx = x1 + (k - 7)*0.7;
				newBall.posy = yinit - 3.0;
			}
			else if (k < 21) {
				newBall.posx = x1 + (k - 14)*0.7;
				newBall.posy = yinit - 4.0;
			}
			else {
				newBall.posx = x1 + (k - 21)*0.7;
				newBall.posy = yinit - 5.0;
			}
			k++;
			newBall.text = "";
			newBall.size = 0.3;
			buttons.insert(buttons.end(), newBall);
			j++;
			newBall.size = 0.0;
			int x = 'a' + i;
			std::string tmp(1, x);
			newBall.text = tmp;
			newBall.posx = newBall.posx - 0.1;
			buttons.insert(buttons.end(), newBall);
			j++;
		}
	}

	newBall.textRed = 1.0;
	newBall.textGreen = 1.0;
	newBall.textBlue = 1.0;
	newBall.text = "Main menu";
	newBall.blue = 0.5;
	newBall.red = 0.0;
	newBall.posx = x1;
	newBall.posy = yinit - 7.3;
	newBall.size = 0.3;
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	back = j;
	j++;
}

int Draw::drawIngredients(int ingrSet, std::set<std::string>& ingredients, std::set<std::string>& selected)
{
	Ball newBall;
	buttons.clear();
	int j = 0;
	int start, remaining;
	int numIngredientsDisplayed = 0;

	start = 20 * ingrSet;
	remaining = ingredients.size() - start;
	moreDrinks = -1;
	lastDrinks = -1;
	std::set<std::string>::iterator it = ingredients.begin();
	for (int i = 0; i < start; i++)
		it++;
	if (remaining < 20) {
		numIngredientsDisplayed = remaining;
		for (int i = 0; i < remaining;i++) {
			newBall.text = *it;
			newBall.red = 1.0;
			if (i < remaining / 2) {
				newBall.posx = x1;
				newBall.posy = yinit - (i + 2)*0.5;
			}
			else {
				newBall.posx = x2;
				newBall.posy = yinit - ((i + 2) - (remaining / 2))*0.5;
			}
			newBall.size = 0.2;
			if (selected.find(*it) != selected.end()){
			//if (selected[i + start] == 1) {
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
			buttons.insert(buttons.end(), newBall);
			it++;
		}
	}
	else {
		numIngredientsDisplayed = 20;
		for (int i = 0; i < 20;i++) {
			newBall.text = *it;
			newBall.red = 1.0;
			if (i < 10) {
				newBall.posx = x1;
				newBall.posy = yinit - (i + 2)*0.5;
			}
			else {
				newBall.posx = x2;
				newBall.posy = yinit - ((i + 2) - (10))*0.5;
			}
			newBall.size = 0.2;
			if (selected.find(*it) != selected.end()) {
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
			buttons.insert(buttons.end(), newBall);
			it++;
		}

		newBall.posy = yinit;
		newBall.posx = x1;
		newBall.size = 0.2;
		newBall.red = 0.5;
		newBall.blue = 1.0;
		newBall.textSize = 1.0;
		newBall.text = "more ingredients";
		buttons.insert(buttons.end(), newBall);
		moreDrinks = numIngredientsDisplayed;
	}
	if (ingrSet != 0) {
		newBall.posx = x1;
		newBall.posy = yinit - 0.5;
		newBall.size = 0.2;
		newBall.red = 0.5;
		newBall.green = 0.0;
		newBall.blue = 1.0;
		newBall.textSize = 1.0;
		newBall.text = "previous ingredients";
		buttons.insert(buttons.end(), newBall);
		lastDrinks = buttons.size()-1;
	}
	else {
		lastDrinks = -1;
	}
	newBall.text = "Select ingredients";
	newBall.red = 0.0;
	newBall.posx = x1;
	newBall.posy = yinit + 0.5;
	newBall.size = 0.0;
	newBall.textSize = 1.5;
	buttons.insert(buttons.end(), newBall);
	newBall.text = "View Drinks";
	newBall.blue = 0.5;
	newBall.red = 0.5;
	newBall.posx = x1;
	newBall.posy = yinit - (1 + 12)*0.5;
	newBall.size = 0.3;
	newBall.textRed = 1.0;
	newBall.textBlue = 1.0;
	newBall.textGreen = 1.0;
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	next = buttons.size()-1;
	newBall.text = "with selected";
	newBall.blue = 0.0;
	newBall.red = 0.0;
	newBall.posx = x1 + 0.5;
	newBall.posy = yinit - (1 + 12)*0.5 - 0.2;
	newBall.size = 0.0;
	newBall.textRed = 1.0;
	newBall.textBlue = 1.0;
	newBall.textGreen = 1.0;
	newBall.textSize = 0.8;
	buttons.insert(buttons.end(), newBall);
	newBall.text = " ingredients";
	newBall.blue = 0.0;
	newBall.red = 0.0;
	newBall.posx = x1 + 0.5;
	newBall.posy = yinit - (1 + 12)*0.5 - 0.4;
	newBall.size = 0.0;
	newBall.textRed = 1.0;
	newBall.textBlue = 1.0;
	newBall.textGreen = 1.0;
	newBall.textSize = 0.8;
	buttons.insert(buttons.end(), newBall);
	newBall.text = "Main menu";
	newBall.blue = 0.5;
	newBall.posx = x1;
	newBall.posy = yinit - 7.3;
	newBall.size = 0.3;
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	back = buttons.size()-1;
	return numIngredientsDisplayed;
}

void Draw::setRandomDrinkCenter(std::map<std::string, Drink, cicompare>& possibleDrinks)
{
	int number;

	double num = rand();
	num = (num / 32767) * possibleDrinks.size();
	number = (int)num;
	std::map<std::string, Drink, cicompare>::iterator it = possibleDrinks.begin();
	for (int i = 0; i < number; i++) {
		it++;
	}
	setVisibleDrinks(it->first,possibleDrinks);
}



