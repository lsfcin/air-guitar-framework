#ifndef __GUITAR_H__
#define __GUITAR_H__

#include <vector>
#include <GL/glut.h>
#include "Resolution.h"
#include "Point2D.h"
#include "HandsPoints.h"
#include "SoundModule.h"

using namespace std;

class Guitar {
public:
	Guitar(int nFrets, int armSize, int bodySize,
         float followVelocityArm = 0.7,
		     float followVelocityBody = 0.1,
		     float followVelocityZ = 0.1,
		     float jitterReductionInXY = 2,
		     float jitterReductionInZ = 10,
		     int minRelevance = 200,
		     int maxHandsPointsStored = 100);

	~Guitar(){}

	void updateLastHandsPoints(HandsPoints handsPoints);

	void followHands(HandsPoints handsPoints);
	Pick playIntensity();

  Point2D getArm() {return this->arm;}
  Point2D getBody() {return this-> body;}
  float getFollowVelocityZ() {return this->followVelocityZ;}
  float getJitterReductionInXY() {return this->jitterReductionInXY;}
  float getJitterReductionZ() {return this->jitterReductionInZ;}
  void setJitterReductionInXY(float jitterReductionInXY) {
    this->jitterReductionInXY = jitterReductionInXY;
  }
  void setJitterReductionInZ(float jitterReductionInZ) {
    this->jitterReductionInZ = jitterReductionInZ;
  }
  int getNFrets() {
    return this->nFrets;
  }
  void setNFrets(int nFrets) {
    this->nFrets = nFrets;
  }

  int getMinRelevance() {
    return this->minRelevance;
  }

private:
  int nFrets;   //number of frets
  int armSize;  //size of guitar's arm
  int bodySize; //size of guitar's body

	Point2D arm;
	Point2D body;

	vector<HandsPoints> lastHandsPoints;

	float followVelocityArm;
	float followVelocityBody;
	float followVelocityZ;

	float jitterReductionInXY;
	float jitterReductionInZ;

	int minRelevance;

	int maxHandsPointsStored;

	bool updated;
};

#endif //__GUITAR_H__