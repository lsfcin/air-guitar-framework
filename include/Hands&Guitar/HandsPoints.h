#ifndef __HANDS_H__
#define __HANDS_H__

#include "Point2D.h"

class HandsPoints {
public:
	HandsPoints(){}
	HandsPoints(Point2D left, Point2D right) : left(left), right(right) {}

	Point2D left;
	Point2D right;

};

#endif //__HANDS_H__