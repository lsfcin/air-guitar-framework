#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <queue>
#include "Resolution.h"
#include "Point2D.h"
#include "HandsPoints.h"

using namespace std;

#define DISTANCE_TOL 2

typedef struct ColorGroup
{
	vector<Point2D> pixelsPositions;
	int color;

} ColourGroup;

class Image
{
public:
	Image();
	Image(Resolution resolution, int nChannels, char *data);
	~Image(){
		if(markedPointsMap) {
			delete [] markedPointsMap;
		}
	}

	HandsPoints findHandsPoints(int colorLeft, int colorRight);
	void markPoint(float x, float y, int mark);
	void markPoint(Point2D point, int mark);
	bool isPointMarked(float x, float y);
	bool isPointMarked(Point2D point);

	Color getPointColor(Point2D point);
	Color getPointColor(int x, int y);

	Resolution getResolution();
	int getNChannels();
	char* getData();
	int* getMarkedPointsMap();
  vector<ColorGroup> getHandsGroups();

	void makeColorGroup(ColorGroup &colorGroup);
	void makeColorGroups(int color);

  Point2D findColorGroupBottom(ColorGroup &colorGroup);
	Point2D findColorGroupCenter(ColorGroup &colorGroup);

private:
	Resolution resolution;
	int nChannels;
	char *data;

	int *markedPointsMap;

	vector<ColorGroup> colorGroups;
};


#endif // __IMAGE_H__