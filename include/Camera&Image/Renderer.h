#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <stdlib.h>
#include <GL/glut.h>

#include "Image.h"
#include "3DSLoader.h"

#define PI 3.14159265

class Renderer {
public:
  Renderer(char* modelVertexURL, char* modelTexURL, unsigned int& camTexID, 
           Resolution camResolution, int nFrets, int armSize, int bodySize);

  void initializeTexture(unsigned int& texID, Resolution& resolution);
  void initialize3DModel(char* model_url, char* tex_url);

  void renderImage(Image* image, const unsigned int& texID);
  void renderColorGroup(ColorGroup& colorGroup);
  void renderRealGroup(ColorGroup& colorGroup, Image* image);
	void renderHandsGroups(vector<ColorGroup> hands);
  void renderRealHands(vector<ColorGroup> hands, Image* image);
	void renderMarkedPoints(Image* image);
  void renderAxis(Point2D& point);
  void renderFrets(Point2D& arm, Point2D& body, int pointSize = 7, float R = 0.8f, float G = 0.6f, float B = 0.f);
  void renderPoint(Point2D& point, int size = 1, float r = 0.f, float g = 0.8f, float b = 0.f);
  void renderBitmapString(int x, int y, void* font, const char* string, float r = 0.f, float g = 0.8f, float b = 0.f);
  void renderGuitarModel(Point2D& arm, Point2D& body,
                         float followVelocityZ, float jitterReductionInZ,
                         float leftHandProximity, float rightHandProximity, 
                         Resolution& resolution);

  void switchToOrthoView(Resolution resolution);
	void switchToPerspectiveView(Resolution resolution);

  int calcPowerOf2(int n);

  int getNFrets() {
    return this->nFrets;
  }
  void setNFrets(int nFrets) {
    this->nFrets = nFrets;
  }

private:
  	obj_type model3D;
    int nFrets;           //number of frets
    int armSize;          //size of guitar's arm
    int bodySize;         //size of guitar's body
};

#endif //__RENDERER_H__