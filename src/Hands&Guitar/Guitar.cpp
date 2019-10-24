#include "Guitar.h"

Guitar::Guitar(int nFrets, int armSize, int bodySize,
               float followVelocityArm,
			         float followVelocityBody,
			         float followVelocityZ,
			         float jitterReductionInXY,
			         float jitterReductionInZ,
			         int minRelevance,
			         int maxHandsPointsStored) 
               : nFrets(nFrets), armSize(armSize), bodySize(bodySize),
                 followVelocityArm(followVelocityArm),
                 followVelocityBody(followVelocityBody),
                 followVelocityZ(followVelocityZ),
                 jitterReductionInXY(jitterReductionInXY),
                 jitterReductionInZ(jitterReductionInXY),
                 minRelevance(minRelevance),
                 maxHandsPointsStored(maxHandsPointsStored) {
	arm.x = -1;
	arm.y = -1;
	body.x = -1;
	body.y = -1;

	updated = false;
}

void Guitar::updateLastHandsPoints(HandsPoints handsPoints) {

	if(handsPoints.left.relevance > minRelevance &&
	   handsPoints.right.relevance > minRelevance) {

		   arm.relevance = handsPoints.left.relevance;
		   body.relevance = handsPoints.right.relevance;

		if(lastHandsPoints.size() >= maxHandsPointsStored) {
			vector<HandsPoints>::iterator it = lastHandsPoints.begin();
			lastHandsPoints.erase(it);
		}

		lastHandsPoints.push_back(handsPoints);
		followHands(handsPoints);
		updated = true;
	} else {
		updated = false;
	}
}

void Guitar::followHands(HandsPoints handsPoints) {
	
	Point2D::followPoint(arm, handsPoints.left, followVelocityArm, jitterReductionInXY);
	Point2D::followPoint(body, handsPoints.right, followVelocityBody, jitterReductionInXY);
}

Pick Guitar::playIntensity() {
	Pick pick;
	pick.intensity = 0;
	pick.distance = 0;
	int size = lastHandsPoints.size();
  static bool played = false;

	if(updated) {
		if(size > 2) {
			Point2D last = lastHandsPoints.at(size - 1).right;
			Point2D previous = lastHandsPoints.at(size - 3).right;

      Point2D point1 = Point2D::findPoint(body, arm, -(this->armSize + this->bodySize));
			Point2D point2 = lastHandsPoints.at(size - 1).left;
      Point2D point3 = Point2D::findPoint(body, arm, -this->bodySize / 2);

			pick.distance = Point2D::distance(point2, point3);
      pick.distance /= this->armSize;

			pick.intensity = 0;

			if(previous.y < last.y && played == false) {

				float intersection = Point2D::intersect(previous, last, point1, point2);
				if(intersection >= 0 && intersection <= 1) {
					pick.intensity = Point2D::distance(last, previous);
          played = true;
				}
      } else if(previous.y > last.y) {
        played = false;
      }

      if(pick.distance > 1) pick.intensity = 0;
		}
	}

	return pick;
}