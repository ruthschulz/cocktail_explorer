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
int Draw::setVisibleDrinks(std::string name, DrinkList& drinks)
{
	// first element of movingButton is the center ball
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
	std::vector<DynamicBall> newButtons = drinks.setMovingBalls(name);

	for (std::vector<DynamicBall>::iterator it = newButtons.begin(); it != newButtons.end(); it++) {
		std::vector<DynamicBall>::iterator itThisButton = movingButtons.begin();
		while ((itThisButton != movingButtons.end()) && (itThisButton->text != it->text))
			itThisButton++;
		if (itThisButton != movingButtons.end()) {
			it->accx = itThisButton->accx;
			it->accx = itThisButton->accx;
			it->force = itThisButton->force;
			it->forcetheta = itThisButton->forcetheta;
			it->posx = itThisButton->posx;
			it->posy = itThisButton->posy;
			it->velx = itThisButton->velx;
			it->vely = itThisButton->vely;
			it->oldposx = itThisButton->oldposx;
			it->oldposy = itThisButton->oldposy;
		}
	}

	for (std::vector<DynamicBall>::iterator it = movingButtons.begin(); it != movingButtons.end(); it++) {
		drinks.setVisible(it->text, 0);
	}
	movingButtons.clear();
	for (std::vector<DynamicBall>::iterator it = newButtons.begin(); it != newButtons.end(); it++) {
		movingButtons.insert(movingButtons.end(), *it);
		// and set to visible in possibledrinks map
		drinks.setVisible(it->text,1);
	}
	otherAttractions.clear();
	//determine attractions between the visible objects that are not the center
	for (std::vector<DynamicBall>::iterator it1 = movingButtons.begin(); it1 != movingButtons.end(); it1++) {
		Drink drink1 = drinks.getDrink(it1->text);
		for (std::vector<DynamicBall>::iterator it2 = it1; it2 != movingButtons.end(); it2++) {
			Drink drink2 = drinks.getDrink(it2->text);
			if ((it1 != it2) && (drink1.name != "null") && (drink2.name != "null")) {
				int numSame = 0;
				int numIngr1 = 0;
				int numIngr2 = 0;
				for (std::vector<std::string>::iterator itIngr1 = drink1.ingredient.begin();
					itIngr1 != drink1.ingredient.end(); itIngr1++) {
					numIngr2 = 0;
					for (std::vector<std::string>::iterator itIngr2 = drink2.ingredient.begin();
						itIngr2 != drink2.ingredient.end(); itIngr2++) {
						if (*itIngr1 == *itIngr2) {
							numSame++;
						}
						numIngr2++;
					}
					numIngr1++;
				}
				otherAttractions[std::make_pair(it1->text, it2->text)] = numSame / (double)(drink2.numIngredients);
				otherAttractions[std::make_pair(it2->text, it1->text)] = numSame / (double)(drink1.numIngredients);
			}
		}
	}
	return buttons.size();
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
	newBall.posx = xcenter;
	newBall.size = 0.7;
	newBall.red = 1.0;
	buttons.insert(buttons.end(), newBall);
	next = j;
	j++;
	newBall.posx = xcenter - 0.3;
	newBall.size = 0.0;
	newBall.red = 0.0;
	newBall.text = "enter";
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	j++;
	newBall.posx = xcenter - 2.5;
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
 * ie select ingredients, view all, starting with letter
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
	newBall.posx = xleft;
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
	newBall.posx = xleft;
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
 * set up the static buttons when showing the drinks
 */
void Draw::showDrinks(bool drinksToShow, bool showLetters, char letter, DrinkList& drinks)
{
	Ball newBall;
	buttons.clear();
	int j = 0;
	if (drinksToShow) {
		newBall.text = "Random";
		newBall.size = 0.25;
		newBall.red = 0.5;
		newBall.blue = 0.5;
		newBall.posx = xleft + 2.4;
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
	newBall.posx = xleft + 0.5;
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
		int a = drinks.lastLetter(letter-'a');
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
		a = drinks.nextLetter(letter-'a');
		x = 'a' + a;
		std::string tmp2(1, x);
		newBall.text = tmp2;
		newBall.textSize = 1.0;
		newBall.posx = newBall.posx - 0.1;
		buttons.insert(buttons.end(), newBall);
		j++;
	}
}

/*
 * void showAlphabet(DrinkList& drinks)
 *
 * alphabet buttons for user to choose starting letter
 */
void Draw::showAlphabet(DrinkList& drinks)
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
		if (drinks.drinksWithLetter(i)){
			if (k < 7) {
				newBall.posx = xleft + k * 0.7;
				newBall.posy = yinit - 2.0;
			}
			else if (k < 14) {
				newBall.posx = xleft + (k - 7)*0.7;
				newBall.posy = yinit - 3.0;
			}
			else if (k < 21) {
				newBall.posx = xleft + (k - 14)*0.7;
				newBall.posy = yinit - 4.0;
			}
			else {
				newBall.posx = xleft + (k - 21)*0.7;
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
	newBall.posx = xleft;
	newBall.posy = yinit - 7.3;
	newBall.size = 0.3;
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	back = j;
	j++;
}

/*
 * int Draw::drawIngredients(int ingrSet, DrinkList& drinks)
 *
 * ingredients buttons for user to choose included ingredients
 */
int Draw::drawIngredients(int ingrSet, DrinkList& drinks)
{
	Ball newBall;
	buttons.clear();
	int j = 0;
	int start, remaining;
	int numIngredientsDisplayed = 0;

	std::vector<Ball> ingrBalls = drinks.setIngrBalls(ingrSet);
	numIngredientsDisplayed = ingrBalls.size();
	for (int i = 0; i < numIngredientsDisplayed; i++) {
		buttons.insert(buttons.end(), ingrBalls[i]);
	}
	start = 20 * ingrSet;
	remaining = drinks.numIngredients() - start;
	moreDrinks = -1;
	lastDrinks = -1;
	if (remaining > 20){
		newBall.posy = yinit;
		newBall.posx = xleft;
		newBall.size = 0.2;
		newBall.red = 0.5;
		newBall.blue = 1.0;
		newBall.textSize = 1.0;
		newBall.text = "more ingredients";
		newBall.textRed = 1.0;
		newBall.textBlue = 1.0;
		newBall.textGreen = 1.0;
		buttons.insert(buttons.end(), newBall);
		moreDrinks = buttons.size()-1;
	}
	if (ingrSet != 0) {
		newBall.posx = xleft;
		newBall.posy = yinit - 0.5;
		newBall.size = 0.2;
		newBall.red = 0.5;
		newBall.green = 0.0;
		newBall.blue = 1.0;
		newBall.textSize = 1.0;
		newBall.textRed = 1.0;
		newBall.textBlue = 1.0;
		newBall.textGreen = 1.0;
		newBall.text = "previous ingredients";
		buttons.insert(buttons.end(), newBall);
		lastDrinks = buttons.size()-1;
	}
	newBall.text = "Select ingredients";
	newBall.red = 0.0;
	newBall.posx = xleft;
	newBall.posy = yinit + 0.5;
	newBall.size = 0.0;
	newBall.textSize = 1.5;
	buttons.insert(buttons.end(), newBall);
	newBall.text = "View Drinks";
	newBall.blue = 0.5;
	newBall.red = 0.5;
	newBall.posx = xleft;
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
	newBall.posx = xleft + 0.5;
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
	newBall.posx = xleft + 0.5;
	newBall.posy = yinit - (1 + 12)*0.5 - 0.4;
	newBall.size = 0.0;
	newBall.textRed = 1.0;
	newBall.textBlue = 1.0;
	newBall.textGreen = 1.0;
	newBall.textSize = 0.8;
	buttons.insert(buttons.end(), newBall);
	newBall.text = "Main menu";
	newBall.blue = 0.5;
	newBall.posx = xleft;
	newBall.posy = yinit - 7.3;
	newBall.size = 0.3;
	newBall.textSize = 1.0;
	buttons.insert(buttons.end(), newBall);
	back = buttons.size()-1;
	return numIngredientsDisplayed;
}

int Draw::numButtons(void)
{
	return buttons.size();
}

int Draw::numMovingButtons(void)
{
	return movingButtons.size();
}

std::string Draw::firstVisibleDrinkName(void)
{
	return movingButtons.begin()->text;
}

/*
 * void moveBalls (void)
 *
 * for each of the balls, determine the new position from the forces acting
 * on the ball, and redraw the ball in this position
 */
void Draw::moveBalls(void)
{
	double attrforce, attrforce1, attrforcex, attrforcey; // components of the attraction force
	double dampforce;			// the damping force
	double repforce, repforce1, repforcex, repforcey; // components of the repulsive force
	double forcex, forcey;		// x and y components of the force on the ball
	double attrtheta, attrtheta1, damptheta, reptheta, reptheta1;	// directions of the force components
	double num;					// used when creating a new random number
	double distance, velocity;	// used when determining the force on the ball
	for (std::vector<DynamicBall>::iterator it = movingButtons.begin(); it != movingButtons.end(); it++) {
		if (it != movingButtons.begin()) {
			it->oldposx = it->posx;
			it->oldposy = it->posy;
			// initialise the repulsive force
			repforce = 0.0;
			reptheta = 0;
			// add up all of the repulsive forces
			for (std::vector<DynamicBall>::iterator it1 = movingButtons.begin();
				it1 != movingButtons.end(); it1++) {
				if (it != it1) {
					// first make sure that the balls are not in the same x or y position
					// if they are, add a random amount to the position (between -0.5 and 0.5)
					if (it->posx == it1->posx) {
						num = rand();
						num = (num / 32767) - 0.5;
						it->posx = it->posx + num;
					}
					if (it->posy == it1->posy) {
						num = rand();
						num = (num / 32767) - 0.5;
						it->posy = it->posy + num;
					}
					// determine the distance between the two balls
					distance = sqrt((it->posx - it1->posx) * (it->posx - it1->posx)
						+ (it->posy - it1->posy) * (it->posy - it1->posy));
					// determine the repulsive force and direction of the force
					repforce1 = e / (fabs(distance) * distance);
					reptheta1 = atan2(it->posy - it1->posy, it->posx - it1->posx);
					// determine the x and y components of the force
					repforcex = repforce * cos(reptheta) + repforce1 * cos(reptheta1);
					repforcey = repforce * sin(reptheta) + repforce1 * sin(reptheta1);
					// add to the sum of the repulsive forces so far
					repforce = sqrt((repforcex * repforcex) + (repforcey * repforcey));
					reptheta = atan2(repforcey, repforcex);
				}
			}
			// work out force, acceleration, velocity and position
			// determine attraction to center based on distance to center
			distance = sqrt((it->posx * it->posx) + (it->posy * it->posy));
			attrforce = -((k*otherAttractions[std::make_pair(firstVisibleDrinkName(), it->text)])*distance);
			attrtheta = atan2(it->posy, it->posx);
			//determine attraction to other balls based on distance
			for (std::vector<DynamicBall>::iterator it1 = movingButtons.begin();
				it1 != movingButtons.end(); it1++) {
				if (it != it1) {
					if ((otherAttractions[std::make_pair(firstVisibleDrinkName(), it->text)]) != 0) {
						// first make sure that the balls are not in the same x or y position
						// if they are, add a random amount to the position (between -0.5 and 0.5)
						if (it->posx == it1->posx) {
							num = rand();
							num = (num / 32767) - 0.5;
							it->posx = it->posx + num;
						}
						if (it->posy == it1->posy) {
							num = rand();
							num = (num / 32767) - 0.5;
							it->posy = it->posy + num;
						}
						// determine the distance between the two balls
						distance = sqrt((it->posx - it1->posx) * (it->posx - it1->posx)
							+ (it->posy - it1->posy) * (it->posy - it1->posy));
						// determine the repulsive force and direction of the force
						attrforce1 = -((k*otherAttractions[std::make_pair(movingButtons.begin()->text, it->text)])*distance);
						attrtheta1 = atan2(it->posy - it1->posy, it->posx - it1->posx);
						// determine the x and y components of the force
						attrforcex = attrforce * cos(attrtheta) + attrforce1 * cos(attrtheta1);
						attrforcey = attrforce * sin(attrtheta) + attrforce1 * sin(attrtheta1);
						// add to the sum of the repulsive forces so far
						attrforce = sqrt((attrforcex * attrforcex) + (attrforcey * attrforcey));
						attrtheta = atan2(attrforcey, attrforcex);
					}
				}
			}
			// determine damping force based on current velocity
			velocity = sqrt((it->velx * it->velx) + (it->vely * it->vely));
			dampforce = -(d*velocity);
			damptheta = atan2(it->vely, it->velx);
			// determine the x and y components of the force on the ball
			forcex = attrforce * cos(attrtheta) + dampforce * cos(damptheta) + repforce * cos(reptheta);
			forcey = attrforce * sin(attrtheta) + dampforce * sin(damptheta) + repforce * sin(reptheta);
			// set the force and direction for the ball
			it->force = sqrt((forcex * forcex) + (forcey * forcey));
			it->forcetheta = atan2(forcey, forcex);
			while (it->force > 1000) {
				it->force = it->force / 10;
			}
			// set the acceleration for the ball
			it->accx = it->force * cos(it->forcetheta) / it->mass;
			it->accy = it->force * sin(it->forcetheta) / it->mass;
			// set the velocity for the ball
			it->velx = it->velx + it->accx * timestep;
			it->vely = it->vely + it->accy * timestep;
			// set the new position of the ball
			it->posx = it->posx + it->velx * timestep;
			it->posy = it->posy + it->vely * timestep;
		}
		else {
			// put the center ball in the center position
			distance = sqrt((it->posx * it->posx) + (it->posy * it->posy));
			if (distance > 0.05) {
				attrforce = -(6 * k*distance);
				attrtheta = atan2(it->posy, it->posx);
				// determine the x and y components of the force on the ball
				forcex = attrforce * cos(attrtheta);
				forcey = attrforce * sin(attrtheta);
				// set the force and direction for the ball
				it->force = sqrt((forcex * forcex) + (forcey * forcey));
				it->forcetheta = atan2(forcey, forcex);
				while (it->force > 1000) {
					it->force = it->force / 10;
				}
				// set the acceleration for the ball
				it->accx = it->force * cos(it->forcetheta) / it->mass;
				it->accy = it->force * sin(it->forcetheta) / it->mass;
				// set the velocity for the ball
				it->velx = it->velx + it->accx * timestep;
				it->vely = it->vely + it->accy * timestep;
				// set the new position of the ball
				it->posx = it->posx + it->velx * timestep;
				it->posy = it->posy + it->vely * timestep;
				double newDistance = sqrt((it->posx * it->posx) + (it->posy * it->posy));
				if (newDistance > distance) {
					// when goes past center, slow down a bit to make it reach center quicker
					// and not shoot past too many times
					it->velx = it->velx*0.9;
					it->vely = it->vely*0.9;
				}
			}
			else {
				it->posx = 0.0;
				it->posy = 0.0;
			}
		}
	}
	//display();
}

DynamicBall Draw::getMovingBall(int index)
{
	return movingButtons[index];
}

Ball Draw::getBall(int index)
{
	return buttons[index];
}

void Draw::changeSelection(int index)
{
	if (buttons[index].selected == 1) {
		buttons[index].red = 1.0;
		buttons[index].blue = 0.0;
		buttons[index].selected = 0;
	}
	else {
		buttons[index].red = 0.0;
		buttons[index].blue = 1.0;
		buttons[index].selected = 1;
	}
}
