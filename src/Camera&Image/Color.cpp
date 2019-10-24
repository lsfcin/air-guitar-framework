#include "Color.h"

bool Color::isColor(int color) {
	switch(color) {
		case RED:
			return isRed();
			break;

		case GREEN:
			return isGreen();
			break;

		case BLUE:
			return isBlue();
			break;

		case ORANGE:
			return isOrange();
			break;

		case BLACK:
			return isBlack();
			break;

		case YELLOW:
			return isYellow();
			break;

		case WHITE:
			return isWhite();
			break;

		default:
			return false;
			break;
	}
}

bool Color::isRed() {
	return (r > (g + 60) && r > (b + 60));
}

bool Color::isOrange() {
	return (r > (g + 35) && g > (b + 35));
}

bool Color::isBlue() {
	return (b > (r + 40) && b > (g + 40));
}

bool Color::isGreen() {
	return (g > (r + 10) && g > (b + 10));
}

bool Color::isBlack() {
	return (r < 100 && g < 100 && b < 100);
}

bool Color::isYellow() {
	return (r >= g && g + 40 > r && b + 50 < g);
}

bool Color::isWhite() {
	return (r > 140 && g > 140 && b > 140);
}

int Color::getR() {
	return r;
}

int Color::getG() {
	return r;
}

int Color::getB() {
	return r;
}