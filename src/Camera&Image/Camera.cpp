#include "Camera.h"

Camera::Camera() {
	if(!initialize()) {
		printf("Couldn't initialize camera!!\n");
		system("pause");
		exit(0);
	}
}

bool Camera::initialize() {
    
	char *vconf = "files/camera/camera.xml";
	char *cparam_name = "files/camera/camera_para.dat";
	ARParam cparam;
	ARParam  wparam;

	if( arVideoOpen( vconf ) < 0 ) {
		printf("Camera configuration openning error!!\n");
		return false;
	}

	if( arVideoInqSize(&resolution.width, &resolution.height) < 0 ) {
		printf("Camera resolution load error!!\n");
		return false;
	}

    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error!!\n");
		return false;
    }

	arParamChangeSize( &wparam, resolution.width, resolution.height, &cparam );
    arInitCparam( &cparam );
    arParamDisp( &cparam );
	arVideoCapStart();

	return true;
}

Image * Camera::grabFrame() {
	ARUint8 *dataPtr = NULL;

	dataPtr = (ARUint8 *)arVideoGetImage();

  this->lastFrame = new Image(this->resolution, 4, (char *) dataPtr);
	//TODO esse 4 tá bizarro, tem que sair daí
	return lastFrame;
}

Image * Camera::getLastFrame() {
  return this->lastFrame;
}

Resolution Camera::getResolution() {
	return this->resolution;
}