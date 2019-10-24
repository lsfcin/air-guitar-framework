#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

#include <GL/glut.h>

#include "win32timer.h"

#include "3DSLoader.h"
#include "Camera.h"
#include "Guitar.h"
#include "SoundModule.h"
#include "Renderer.h"

#define FPS 30 //define a velocidade máxima do vídeo

win32timer frameTimer;

Camera* camera;
Guitar* guitar;
SoundModule* soundModule;
Renderer* renderer;
unsigned int camTexID;

bool isNextFrame() {
	return (frameTimer.getMilliseconds() > (1000 / FPS));
}

//Por enquanto do teclado a gente usa só o esc mesmo
void keyboard(unsigned char key, int x, int y)
{
	int currentSoundIndex;
	int currentEffect;
	int currentNFrets;

	switch (key)
	{
	case 0x1B:
		exit(0);
		break;

	case 's':
	case 'S':
		currentSoundIndex = soundModule->getSoundIndex();
		soundModule->setSoundIndex(++currentSoundIndex);
		break;

	case 'e':
	case 'E':
		currentEffect = soundModule->getEffectMode();
		soundModule->setEffectMode(++currentEffect);
		break;

	case '+':
		currentNFrets = guitar->getNFrets();
		if (currentNFrets < 25) {
			guitar->setNFrets(++currentNFrets);
			renderer->setNFrets(currentNFrets);
			soundModule->setNFrets(currentNFrets);
		}
		break;

	case '-':
		currentNFrets = guitar->getNFrets();
		if (currentNFrets > 1){
			guitar->setNFrets(--currentNFrets);
			renderer->setNFrets(currentNFrets);
			soundModule->setNFrets(currentNFrets);
		}
		break;

	case ' ':
		soundModule->managePlayback(PLAY_PAUSE);
		break;

	case 8:
		soundModule->managePlayback(RESET);
		break;
	}
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1:
		soundModule->setToneMapPosition(0);
		break;

	case GLUT_KEY_F2:
		soundModule->setToneMapPosition(1);
		break;

	case GLUT_KEY_F3:
		soundModule->setToneMapPosition(2);
		break;

	case GLUT_KEY_F4:
		soundModule->setToneMapPosition(3);
		break;

	case GLUT_KEY_F5:
		soundModule->setToneMapPosition(4);
		break;

	case GLUT_KEY_F6:
		soundModule->setToneMapPosition(5);
		break;

	case GLUT_KEY_F7:
		soundModule->setToneMapPosition(6);
		break;

	case GLUT_KEY_F8:
		soundModule->setToneMapPosition(7);
		break;

	case GLUT_KEY_F9:
		soundModule->setToneMapPosition(8);
		break;

	case GLUT_KEY_F10:
		soundModule->setToneMapPosition(9);
		break;

	case GLUT_KEY_F11:
		soundModule->setToneMapPosition(10);
		break;

	case GLUT_KEY_F12:
		soundModule->setToneMapPosition(11);
		break;

	case GLUT_KEY_UP:
		soundModule->managePlayback(VOLUME_UP);
		break;

	case GLUT_KEY_DOWN:
		soundModule->managePlayback(VOLUME_DOWN);
		break;

	default:
		break;
	}
}

void display() {
	if (isNextFrame()) {
		frameTimer.reset();
		Image* image = camera->grabFrame();

		if (image->getData() == NULL) {
			Sleep(2);
		}
		else {

			HandsPoints handsPoints = image->findHandsPoints(YELLOW, YELLOW);
			guitar->updateLastHandsPoints(handsPoints);
			Pick pick = guitar->playIntensity();
			soundModule->playPick(pick, EQUAL_TEMPERED);

			//Rendering
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//renderer->switchToOrthoView(camera->getResolution());
			//renderer->renderImage(image, camTexID);
			renderer->switchToPerspectiveView(camera->getResolution());
			renderer->renderGuitarModel(guitar->getArm(), guitar->getBody(),
				guitar->getFollowVelocityZ(), guitar->getJitterReductionZ(),
				guitar->getArm().relevance, guitar->getBody().relevance,
				camera->getResolution());//*/

			renderer->switchToOrthoView(camera->getResolution());

			if (handsPoints.left.relevance >= guitar->getMinRelevance()) {
				//renderer->renderHandsGroups(image->getHandsGroups());
				renderer->renderRealHands(image->getHandsGroups(), image);
				//renderer->renderMarkedPoints(image);
				renderer->renderAxis(handsPoints.left);
				renderer->renderAxis(handsPoints.right);
				renderer->renderFrets(guitar->getArm(), guitar->getBody(), 10, 0.f, 0.8f, 0.f);
				renderer->renderPoint(handsPoints.left, 10, 0.9f, 0.5f, 0.f);
				renderer->renderPoint(handsPoints.right, 10, 0.9f, 0.5f, 0.f);
				//renderer->renderPoint(guitar->getArm(), 6, 0.7f, 0.7f, 0.7f);
				//renderer->renderPoint(guitar->getBody(), 6, 0.7f, 0.7f, 0.7f);
			}

			renderer->renderBitmapString(10, 10, GLUT_BITMAP_HELVETICA_18, "tone ('F1' to 'F12' to change):", 0.7f, 0.5f, 0.f);
			char temp[4];
			for (int i = 0; i < DISTINCT_NOTES; i++) {
				itoa(i + 1, temp, 10);
				if (soundModule->getToneMapPosition(i)) {
					renderer->renderBitmapString(95 + i * 10, 10, GLUT_BITMAP_HELVETICA_18, temp, 1.f, 0.7f, 0.f);
				}
				else {
					renderer->renderBitmapString(95 + i * 10, 10, GLUT_BITMAP_HELVETICA_18, temp, 0.5f, 0.5f, 0.5f);
				}
			}

			//text rendering
			itoa(guitar->getNFrets(), temp, 10);
			renderer->renderBitmapString(10, 20, GLUT_BITMAP_HELVETICA_18, "frets num ('+' / '-' to change):", 0.7f, 0.5f, 0.f);
			renderer->renderBitmapString(92, 20, GLUT_BITMAP_HELVETICA_18, temp, 1.f, 0.7f, 0.f);
			renderer->renderBitmapString(10, 30, GLUT_BITMAP_HELVETICA_18, "sound index ('s' to change):", 0.7f, 0.5f, 0.f);
			itoa(soundModule->getSoundIndex(), temp, 10);
			renderer->renderBitmapString(89, 30, GLUT_BITMAP_HELVETICA_18, temp, 1.f, 0.7f, 0.f);
			renderer->renderBitmapString(10, 40, GLUT_BITMAP_HELVETICA_18, "effect mode ('e' to change):", 0.7f, 0.5f, 0.f);
			string effect;
			if (soundModule->getEffectMode() == NO_EFFECT) effect = "no effect";
			else if (soundModule->getEffectMode() == FRET_SLIDE) effect = "fret slide";
			else if (soundModule->getEffectMode() == FREE_SLIDE) effect = "free slide";
			else if (soundModule->getEffectMode() == VIBRATO) effect = "vibrato";
			renderer->renderBitmapString(89, 40, GLUT_BITMAP_HELVETICA_18, effect.c_str(), 1.f, 0.7f, 0.f);
			renderer->renderBitmapString(10, 50, GLUT_BITMAP_HELVETICA_18, "frequency: ", 0.7f, 0.5f, 0.f);
			string sound = soundModule->getNote(pick.distance);
			sound = sound + "Hz";
			renderer->renderBitmapString(43, 50, GLUT_BITMAP_HELVETICA_18, sound.c_str(), 1.f, 0.7f, 0.f);
			string state;
			if (!(soundModule->getPlaybackPlaying())) state = "press space bar to play/pause";
			else if (soundModule->getPlaybackPaused()) state = "paused";
			else state = "playing";
			float playback_volume = soundModule->getPlaybackVolume();
			string volume = "";
			for (int i = 0; i < 10; ++i) {
				if (playback_volume >= ((float)i) / 10)
					volume = volume + ":";
			}
			renderer->renderBitmapString(10, camera->getResolution().height - 10, GLUT_BITMAP_HELVETICA_18,
				"playback state:", 0.7f, 0.5f, 0.f);
			renderer->renderBitmapString(55, camera->getResolution().height - 10, GLUT_BITMAP_HELVETICA_18,
				state.c_str(), 1.f, 0.7f, 0.f);
			renderer->renderBitmapString(camera->getResolution().width / 2, camera->getResolution().height - 10,
				GLUT_BITMAP_HELVETICA_18, "volume (up / down to change):", 0.7f, 0.5f, 0.f);
			renderer->renderBitmapString(camera->getResolution().width / 2 + 85, camera->getResolution().height - 10,
				GLUT_BITMAP_HELVETICA_18, volume.c_str(), 1.f, 0.7f, 0.f);

			static int count = 0;
			++count;

			static int total_time = 0;
			total_time += frameTimer.getMilliseconds();

			int average = total_time / count;

			//printf("time: %d          \r", frameTimer.getMilliseconds());
			printf("time: %d          \r", average);

			glutSwapBuffers();
		}

		delete image;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv) {

	frameTimer.reset();

	camera = new Camera();

	int width = camera->getResolution().width * 3;
	int height = camera->getResolution().height * 3;

	int screenDiagonal = sqrt(pow((double)camera->getResolution().width, 2) +
		pow((double)camera->getResolution().height, 2));


	int nFrets = 4;
	int armSize = screenDiagonal * 0.5;
	int bodySize = screenDiagonal * 0.2;
	int minimunIntensity = 3;

	guitar = new Guitar(nFrets, armSize, bodySize, 0.7, 0.1, 0.1, 10, 10, 200, 100);
	soundModule = new SoundModule("files/guitar/sounds/sounds.txt",
		nFrets, minimunIntensity);
	soundModule->setSoundIndex(0);
	soundModule->setEffectMode(FRET_SLIDE);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - width) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - height) / 2);

	glutCreateWindow("Air Guitar Pro");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glShadeModel(GL_SMOOTH);

	//it must be instanciated after the glutInit
	renderer = new Renderer("files/guitar/models/guitar.3ds", "files/guitar/models/base.bmp",
		camTexID, camera->getResolution(), nFrets, armSize, bodySize);

	glutMainLoop();

	return(0);
}