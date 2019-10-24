#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <windows.h>
#include <video.h>

#include "Resolution.h"
#include "Image.h"

class Camera {
public:
	Camera();
	virtual ~Camera() {}

	bool initialize();

	Image * grabFrame();
  Image * getLastFrame();

	Resolution getResolution();

private:
	Resolution resolution;
  Image* lastFrame;
};

#endif //__CAMERA_FRAME_GRABBER_H__