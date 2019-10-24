#ifndef __POINT2D_H__
#define __POINT2D_H__

#include <math.h>
#include <GL/glut.h>
#include "Color.h"

class Point2D {
public:
	Point2D() : x(0), y(0), relevance(0) {}
	Point2D(int x, int y) : x(x), y(y), relevance(0) {}
	~Point2D(){}

	float x;
	float y;
	int relevance;

	static Point2D subtractPoints(Point2D &point1, Point2D &point2) {
		return Point2D(point1.x - point2.x, point1.y - point2.y);
	}

	static Point2D multiply(Point2D &point, float factor) {
		return Point2D(point.x * factor, point.y * factor);
	}

  static float dotProduct(Point2D &point1, Point2D &point2) {
    return (point1.x * point2.x) + (point1.y * point2.y);
  }

	static Point2D midPoint(Point2D &point1, Point2D &point2) {
		return Point2D((point1.x + point2.x) / 2, (point1.y + point2.y) / 2);
	}

	static Point2D findPoint(Point2D &point1, Point2D &point2, float distance) {
		Point2D result;

		result = subtractPoints(point1, point2);
		setNorm(result, distance);

		result.x += point1.x;
		result.y += point1.y;

		return result;
	}
	static float distance(Point2D &point1, Point2D &point2) {
		float distance = -1;
		Point2D distanceVector = subtractPoints(point1, point2);
		distance = getNorm(distanceVector);
		return distance;
	}
	static float getNorm(Point2D &point) {	
		return sqrt(point.x * point.x + point.y * point.y);
	}

	static void normalize(Point2D &point) {
		int norm = getNorm(point);

		point.x /= norm;
		point.y /= norm;
	}

	static void setNorm(Point2D &point, float newNorm) {
		normalize(point);
		
		point.x *= newNorm;
		point.y *= newNorm;
	}

	static void followPoint(Point2D &follower, Point2D &followed, float velocity, int jitterReductionTerm) {
		
		if(followed.x >= 0 && followed.y >= 0) {
			if(follower.x < 0 || follower.y < 0) {
				follower = followed;
			} else {
				Point2D distance = subtractPoints(follower, followed);
				
				if(abs(distance.x) > jitterReductionTerm ||
				   abs(distance.y) > jitterReductionTerm) {
					
					Point2D increment = multiply(distance, velocity);				

					follower.x -= increment.x;
					follower.y -= increment.y;
				}
			}
		}
	}
	
	//TODO esse troço tá estranho, tá funcionando mas tá bizarro
	static float intersect(Point2D &a, Point2D &b, Point2D &c, Point2D &d) {
		float result = -1;

    /*E = B-A = ( Bx-Ax, By-Ay )
    F = D-C = ( Dx-Cx, Dy-Cy ) 
    P = ( -Ey, Ex )
    h = ( (A-C) * P ) / ( F * P )//*/

    /*Point2D e = subtractPoints(b, a);
    Point2D f = subtractPoints(d, c);
    Point2D p(-e.y, e.x);
    float divisor = dotProduct(f, p);

    if(divisor != 0) result = (dotProduct(subtractPoints(a, c), p)) / divisor;//*/


		float det = (d.x - c.x) * (b.y - a.y)  -  (d.y - c.y) * (b.x - a.x);
		
		if(det != 0) {
			float s = ((d.x - c.x) * (c.y - a.y) - (d.y - c.y) * (c.x - a.x))/ det ;

			if(s > 0 && s < 1) {
				result = s;
			}
		}//*/
		
		return result;
	}
	static Point2D intersectionPoint(Point2D &p1, Point2D &p2, Point2D &p3, Point2D &p4) {

		float a1 = (float)(p1.y - p2.y) / (p1.x - p2.x + 0.0001f);
		float b1 = p1.y - a1 * p1.x;
		
		float a2 = (float)(p3.y - p4.y) / (p3.x - p4.x + 0.0001f);
		float b2 = p3.y - a2 * p3.x;

		float x = (float)(b2 - b1) / (a1 - a2 + 0.0001f);
		float y = a1 * x + b1;

		return	Point2D(x, y);
	}
};

#endif //__POINT2D_H__