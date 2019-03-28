#ifndef BALL_H_
#define BALL_H_

#pragma once
#include <string>
#include <vector>

class Ball
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

public:
	Ball() {};
	Ball(double x, double y, double z, double s, double r, double g, double b, double tS,
		double tR, double tG, double tB, std::string t, std::vector<std::string> nT, int sel) : 
		posx(x), posy(y), posz(z), size(s), red(r), green(g), blue(b), textSize(tS), 
		textRed(tR), textGreen(tG), textBlue(tB), text(t), nextText(nT), selected(sel) {};
	~Ball() {};
};

#endif
