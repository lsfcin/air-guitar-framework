#include "Image.h"

Image::Image() {
	int size = 320 * 240;

	data = NULL;

	markedPointsMap = (int*)malloc(size * sizeof(int));

	for(int i = 0; i < size; i++) {
		markedPointsMap[i] = 0;
	}
}

Image::Image(Resolution resolution, int nChannels, char *data) : 
			 resolution(resolution), nChannels(nChannels), data(data) {
	
	int size = resolution.height * resolution.width;

	data = NULL;

	markedPointsMap = (int*)malloc(size * sizeof(int));

	for(int i = 0; i < size; i++) {
		markedPointsMap[i] = 0;
	}
}

HandsPoints Image::findHandsPoints(int colorLeft, int colorRight) {

	Point2D left;
	Point2D right;

	if(colorLeft == colorRight) {
		makeColorGroups(colorLeft);

		ColorGroup group1;
		ColorGroup group2;

		for(int i = 0; i < colorGroups.size(); i++) {
			if(group1.pixelsPositions.size() < colorGroups.at(i).pixelsPositions.size()) {
				group2 = group1;
				group1 = colorGroups.at(i);				
			} else if(group2.pixelsPositions.size() < colorGroups.at(i).pixelsPositions.size()) {
				group2 = colorGroups.at(i);
			}
		}

    Point2D center1, center2, bottom1, bottom2;
    center1 = findColorGroupCenter(group1);
    center2 = findColorGroupCenter(group2);
    bottom1 = findColorGroupBottom(group1);
    bottom2 = findColorGroupBottom(group2);

    if(colorGroups.size() == 1) {
      left = findColorGroupCenter(group1);
			left.relevance = group1.pixelsPositions.size();
    } else if(colorGroups.size() >= 2) {
      if(center1.x < center2.x) {
        left = center1;
			  left.relevance = group1.pixelsPositions.size();
        right = center2;
			  right.relevance = group2.pixelsPositions.size();
      } else {
        left = center2;
			  left.relevance = group2.pixelsPositions.size();
        right = center1;
			  right.relevance = group1.pixelsPositions.size();
      }
    }
	}

	else {
		for(int i = 0; i < resolution.width; i++) {
			for(int j = 0; j < resolution.height; j++) {

				Color tempColor = getPointColor(i, j);

				if(tempColor.isColor(colorLeft)) {
					left.x += i;
					left.y += j;
					left.relevance++;
					markPoint(i, j, colorLeft);
				}
				
				if(tempColor.isColor(colorRight)) {
					right.x += i;
					right.y += j;
					right.relevance++;
					markPoint(i, j, colorRight);
				}
			}
		}

		if(left.relevance > 0) {
			left.x /= left.relevance;
			left.y /= left.relevance;
		}

		if(right.relevance > 0) {
			right.x /= right.relevance;
			right.y /= right.relevance;
		}
	}

	return HandsPoints(left, right);
}

void Image::markPoint(float x, float y, int mark) {
	Point2D point(x, y);
	markPoint(point, mark);
}

void Image::markPoint(Point2D point, int mark) {
	markedPointsMap[(int)(point.y * resolution.width + point.x)] = mark;
}

bool Image::isPointMarked(float x, float y) {
	Point2D point(x, y);
	return isPointMarked(point);
}

bool Image::isPointMarked(Point2D point) {
	bool marked = false;

	if(markedPointsMap[(int)(point.y * resolution.width + point.x)] > 0) {
		marked = true;
	}

	return marked;
}

Color Image::getPointColor(Point2D point) {
	
	int index = (point.y * this->getResolution().width + point.x) * 4;
	unsigned char r = this->getData()[index + 2];
	unsigned char g = this->getData()[index + 1];
	unsigned char b = this->getData()[index + 0];

	Color color(r, g, b);

	return color;
}

Color Image::getPointColor(int x, int y) {
	Point2D point;
	point.x = x;
	point.y = y;
	return getPointColor(point);
}

Resolution Image::getResolution() {
	return resolution;
}

int Image::getNChannels() {
	return nChannels;
}
char* Image::getData() {
	return data;
}
int* Image::getMarkedPointsMap() {
	return markedPointsMap;
}

vector<ColorGroup> Image::getHandsGroups() {
  vector<ColorGroup> result;

  ColorGroup group1;
	ColorGroup group2;

	for(int i = 0; i < colorGroups.size(); i++) {
		if(group1.pixelsPositions.size() < colorGroups.at(i).pixelsPositions.size()) {
			group2 = group1;
			group1 = colorGroups.at(i);
		} else if(group2.pixelsPositions.size() < colorGroups.at(i).pixelsPositions.size()) {
			group2 = colorGroups.at(i);
		}
	}

  result.push_back(group1);
  result.push_back(group2);
  return result;
}

void Image::makeColorGroup(ColorGroup &colorGroup) {
	queue<Point2D> pixelsQueue;

	int color = colorGroup.color;
	Point2D pixel = colorGroup.pixelsPositions.at(0);

	pixelsQueue.push(pixel);

	while(!pixelsQueue.empty()) {
		pixel = pixelsQueue.front();
		for(int i = 0; i < DISTANCE_TOL * 4; i++) {
			
			int direction = i / DISTANCE_TOL;
			int distance = i % DISTANCE_TOL;

			int x = 0;
			int y = 0;

			switch(direction) {
				case 0:
					x = pixel.x + distance;
					y = pixel.y;
					break;

				case 1:
					x = pixel.x - distance;
					y = pixel.y;
					break;

				case 2:
					x = pixel.x;
					y = pixel.y + distance;
					break;

				case 3:
					x = pixel.x;
					y = pixel.y - distance;
					break;

				default:
					break;
			}


			if(x > 0 && x < resolution.width && y > 0 && y < resolution.height) {

				if(!isPointMarked(x, y)) {
					Color tempColor = getPointColor(x, y);
					if(tempColor.isColor(color)) {
						Point2D tempPoint(x, y);
						pixelsQueue.push(tempPoint);
						colorGroup.pixelsPositions.push_back(tempPoint);
						markPoint(x, y, color);
					}
				}
			}
		}
		pixelsQueue.pop();
	}
}

void Image::makeColorGroups(int color) {

	for(unsigned int i = 0; i < colorGroups.size(); i++) {
		colorGroups.at(i).pixelsPositions.clear();
	}
	colorGroups.clear();

	for(int i = 0; i < resolution.width; i++) {
		for(int j = 0; j < resolution.height; j++) {

			Point2D point(i, j);
			
			Color tempColor = getPointColor(point);

			if(!isPointMarked(point) && tempColor.isColor(color)) {
				ColorGroup colorGroup;
				colorGroup.color = color;
				colorGroup.pixelsPositions.push_back(point);

				makeColorGroup(colorGroup);

				colorGroups.push_back(colorGroup);
			}
		}
	}
}

Point2D Image::findColorGroupBottom(ColorGroup &colorGroup) {
  Point2D bottom(0.f, -500.f);
  Point2D center = findColorGroupCenter(colorGroup);

	int size = colorGroup.pixelsPositions.size();

	for(int i = 0; i < size; i++) {
    if(colorGroup.pixelsPositions.at(i).y > bottom.y) {
      bottom = colorGroup.pixelsPositions.at(i);
    }
	}
   
  bottom.x = (bottom.x + center.x) / 2;
  bottom.y = (bottom.y + center.y) / 2;

	return bottom;
}

Point2D Image::findColorGroupCenter(ColorGroup &colorGroup) {
	Point2D center;

	int size = colorGroup.pixelsPositions.size();

	for(int i = 0; i < size; i++) {
		center.x += colorGroup.pixelsPositions.at(i).x;
		center.y += colorGroup.pixelsPositions.at(i).y;
	}

	center.x /= size;
	center.y /= size;

	return center;
}

/*void takePicture(Image * image) {
	static int count = 0;
		count++;

		char temp[10];
		itoa(count, temp, 10);
		string x = temp;
		string imageFileUrl = "C:\\Documents and Settings\\Admin\\Desktop\\frame" + x + ".pnm";
		ofstream imageFile(imageFileUrl.c_str());

		imageFile << "P3\n320 240\n255\n";

		for(int i = 0; i < image->imageSize; i+=3)
		{
			unsigned char r = image->imageData[i + 0];
			unsigned char g = image->imageData[i + 1];
			unsigned char b = image->imageData[i + 2];
			int r2 = r;
			int g2 = g;
			int b2 = b;
			imageFile << b2 << " ";
			imageFile << g2 << " ";
			imageFile << r2 << "\n";
		}
		imageFile.close();
}//*/