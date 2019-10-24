#ifndef __COLOR_H__
#define __COLOR_H__

enum Colors {NOCOLOR, RED, GREEN, BLUE, ORANGE, BLACK, YELLOW, WHITE};

class Color {
public:
	Color(){}
	Color(int r, int g, int b) : r(r), g(g), b(b){}
	~Color(){}

	bool isColor(int color);
	bool isRed();
	bool isOrange();
	bool isBlue();
	bool isGreen();
	bool isBlack();
	bool isYellow();
	bool isWhite();

	int getR();
	int getG();
	int getB();

private:
	int r;
	int g;
	int b;

};


#endif // __COLOR_H__