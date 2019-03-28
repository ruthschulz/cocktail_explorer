#ifndef DYNAMICBALL_H_
#define DYNAMICBALL_H_

#pragma once
#include "Ball.h"
class DynamicBall
{
public:
	double posx{ 0.0 };			// x position
	double posy{ 0.0 };			// y position
	double posz{ 0.0 };			// z position
	double size{ 0.0 };			// size of the ball (radius)
	double red{ 0.0 };			// amount of red of ball
	double green{ 0.0 };		// amount of green of ball
	double blue{ 0.0 };			// amount of blue of ball
	double textSize{ 0.0 };			// size of the ball (radius)
	double textRed{ 0.0 };			// amount of red of ball
	double textGreen{ 0.0 };		// amount of green of ball
	double textBlue{ 0.0 };			// amount of blue of ball
	std::string text{ "" };	// the name of the drink
	std::vector<std::string> nextText;	// the ingredients of the drink
	int selected{ 0 };		// if the ball is selected
	double oldposx{ 0.0 };		// last x position
	double oldposy;		// last y position
	double force{ 0.0 };		// force on the ball
	double forcetheta{ 0.0 };	// direction of the force
	double velx{ 0.0 };			// x velocity
	double vely{ 0.0 };			// y velocity
	double accx{ 0.0 };			// x acceleration
	double accy{ 0.0 };			// y acceleration
	double mass{ 0.1 };			// mass of the ball
	int center{ 0 };			// boolean determining if center ball ( 1 = centerball, 0 = not centerball)

public:
	DynamicBall() {};
	DynamicBall(double x, double y, double z, double s, double r, double g, double b, double tS,
		double tR, double tG, double tB, std::string t, std::vector<std::string> nT, int sel) :
		posx(x), posy(y), posz(z), size(s), red(r), green(g), blue(b), textSize(tS),
		textRed(tR), textGreen(tG), textBlue(tB), text(t), nextText(nT), selected(sel) {};
	~DynamicBall() {};
};

#endif
