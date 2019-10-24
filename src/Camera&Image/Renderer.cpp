#include "Renderer.h"

Renderer::Renderer(char* modelVertexURL, char* modelTexURL, unsigned int& camTexID, 
                   Resolution camResolution, int nFrets, int armSize, int bodySize) : 
                                                 nFrets(nFrets), armSize(armSize), bodySize(bodySize) {

  this->initializeTexture(camTexID, camResolution);
  this->initialize3DModel(modelVertexURL, modelTexURL);
}

void Renderer::initialize3DModel(char* model_url, char* tex_url) {
  Load3DS(model_url, &model3D);
  LoadTexture(tex_url, model3D.id_texture);
}

void Renderer::initializeTexture(unsigned int& texID, Resolution& resolution) {

  GLuint generatedID;
  int widthPowerOf2 = calcPowerOf2(resolution.width);
	int heightPowerOf2 = calcPowerOf2(resolution.height);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &generatedID);
	glBindTexture(GL_TEXTURE_2D, generatedID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, widthPowerOf2, heightPowerOf2, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, 0);

  texID = generatedID;
}

void Renderer::renderImage(Image* image, const unsigned int& texID) {
  int width = image->getResolution().width;
  int height = image->getResolution().height;
  int widthPowerOf2 = calcPowerOf2(width);
	int heightPowerOf2 = calcPowerOf2(height);
  float xRatio = (float)(image->getResolution().width)/widthPowerOf2;
	float yRatio = (float)(image->getResolution().height)/heightPowerOf2;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image->getData());

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0000f, 0.0000f);
		glVertex3f(0.0f, 0.0f, 0.0f);

		glTexCoord2f(0.0000f, yRatio);
		glVertex3f(0.0f, height, 0.0f);

		glTexCoord2f(xRatio, yRatio);
		glVertex3f(width, height, 0.0f);

		glTexCoord2f(xRatio, 0.0000f);
		glVertex3f(width, 0.0f, 0.0f);
	glEnd();

	glBindTexture( GL_TEXTURE_2D, 0 );
	glDisable( GL_TEXTURE_2D );
}

void Renderer::renderColorGroup(ColorGroup& colorGroup) {
	int color = colorGroup.color;
	switch(color) {
		case RED:
			glColor3f(0.8, 0.0, 0.0);
			break;

		case GREEN:
			glColor3f(0.0, 0.8, 0.0);
			break;

		case BLUE:
			glColor3f(0.0, 0.0, 0.8);
			break;

		case YELLOW:
			glColor3f(0.9, 0.8, 0.0);
			break;

		case ORANGE:
			glColor3f(0.8, 0.5, 0.0);
			break;

		case BLACK:
			glColor3f(0.1, 0.1, 0.1);
			break;

		case WHITE:
			glColor3f(0.8, 0.8, 0.8);
			break;

		default:
			break;
	}

	for(int i = 0; i < colorGroup.pixelsPositions.size(); i++) {
		glVertex2d(colorGroup.pixelsPositions.at(i).x, 
				       colorGroup.pixelsPositions.at(i).y);
	}
}

void Renderer::renderRealGroup(ColorGroup& colorGroup, Image* image) {
  for(int i = 0; i < colorGroup.pixelsPositions.size(); i++) {
    int x = colorGroup.pixelsPositions.at(i).x;
    int y = colorGroup.pixelsPositions.at(i).y;

    Color color = image->getPointColor(x, y);
    glColor3f((float)(color.getR())/255 + 0.3f, 
              (float)(color.getG())/255 + 0.2f,
              (float)(color.getB())/255 - 0.1f);
		glVertex2d(x, y);
	}
}

void Renderer::renderHandsGroups(vector<ColorGroup> hands) {
  glPointSize(2);
	glBegin(GL_POINTS);
	  if(hands.size() > 0)renderColorGroup(hands.at(0));
	  if(hands.size() > 1)renderColorGroup(hands.at(1));
  glEnd();
}

void Renderer::renderRealHands(vector<ColorGroup> hands, Image* image) {
  glPointSize(2);
	glBegin(GL_POINTS);
    if(hands.size() > 0)renderRealGroup(hands.at(0), image);
	  if(hands.size() > 1)renderRealGroup(hands.at(1), image);
  glEnd();
}

void Renderer::renderMarkedPoints(Image* image) {
	glPointSize(2);
	glBegin(GL_POINTS);

    int width = image->getResolution().width;
    int height = image->getResolution().height;
    int* markedPointsMap = image->getMarkedPointsMap();

    for(int i = 0; i < width; i++) {
		  for(int j = 0; j < height; j++) {
			  int mark = markedPointsMap[j * width + i];
			  if(mark > 0) {
				  switch(mark) {
					  case RED:
						  glColor3f(0.8, 0.0, 0.0);
						  break;

					  case GREEN:
						  glColor3f(0.0, 0.8, 0.0);
						  break;

					  case BLUE:
						  glColor3f(0.0, 0.0, 0.8);
						  break;

					  case YELLOW:
						  glColor3f(1.0, 0.8, 0.0);
						  break;

					  case ORANGE:
						  glColor3f(0.8, 0.5, 0.0);
						  break;

					  case BLACK:
						  glColor3f(1, 1, 1);
						  break;

					  case WHITE:
						  glColor3f(0.8, 0.8, 0.8);
						  break;

					  default:
						  break;
				  }
				  glVertex2d(i, j);
			  }
		  }
	  }
	glEnd();
}

void Renderer::renderAxis(Point2D& point) {
  glLineWidth(1.f);
  glColor3f(0.4f, 0.4f, 0.4f);
  glBegin(GL_LINES);
    glVertex2d(point.x,  500);
    glVertex2d(point.x, -500);
    glVertex2d( 500, point.y);
    glVertex2d(-500, point.y);
  glEnd();
}

void Renderer::renderFrets(Point2D& arm, Point2D& body, int pointSize, float R, float G, float B) {
  
  int fretSize = this->armSize / this->nFrets;
  int baseDistance = this->bodySize / 2 + pointSize / 2;

  Point2D temp1 = Point2D::findPoint(body, arm, -(this->armSize + baseDistance));
  Point2D temp2 = Point2D::findPoint(body, arm, -baseDistance);
  Point2D temp3 = Point2D::findPoint(body, arm,  baseDistance - 10);
  
	glLineWidth(2);
	glBegin(GL_LINES);
    glColor3f(0.7f, 0.7f, 0.7f);
		glVertex2d(temp2.x, temp2.y);
		glVertex2d(temp3.x, temp3.y);

    glColor3f(R/2, G/2, B/2);
		glVertex2d(temp1.x, temp1.y);
    glColor3f(0.7f, 0.7f, 0.7f);
		glVertex2d(temp3.x, temp3.y);
	glEnd();

  for(int i = 0; i < this->nFrets + 1; ++i) {
     Point2D point = Point2D::findPoint(body, arm, -(baseDistance + fretSize * i));
     renderPoint(point, pointSize, R, G, B);
  }
}

void Renderer::renderPoint(Point2D& point, int size, float r, float g, float b) {
	glPointSize(size);
	glColor3f(r, g, b);
	glBegin(GL_POINTS);
		glVertex2d(point.x, point.y);
	glEnd();
}

void Renderer::renderGuitarModel(Point2D& arm, Point2D& body,
                                 float followVelocityZ, float jitterReductionInZ,
                                 float leftHandProximity, float rightHandProximity,
                                 Resolution& resolution) {

	/*(resolution.width * 0.9) é uma distância no eixo z equivalente ao plano do viewport, 
	ou seja, é uma posição de z em que posso movimentar o x e y da guitarra como se fossem pontos 2D mesmo...*/
	float b = arm.x - body.x;
	float h = arm.y - body.y;

	float tangent = h/b;

	float angleZ = atanf(tangent) * 180.0 / PI;
	
	if (arm.x > body.x) {
		angleZ += 180;
	}
	
	glRotatef(179 - angleZ, 0.f, 0.f, 1.f);

	glTranslatef(-(resolution.width/2), -3.f, -(resolution.width * 0.9));

	float x = (resolution.width/2) - body.x;
	float y = (resolution.height/2)- body.y;

	angleZ = angleZ * PI /180;

	float locationX = cos(angleZ) * x + sin(angleZ) * y;
	float locationY =-sin(angleZ) * x + cos(angleZ) * y;

	static float locationZ = 0;
	float pointsSum = leftHandProximity + rightHandProximity;

	/*Essa é a linha de código campeã em constantes inexplicáveis, mas por favor 
	não mexam nelas, foi uma combinação maldita pra encontrar esse resultado razoável...
	Essas constantes definem basicamente uma função para definir o quão longe a guitarra está da câmera*/
	float tempLocationZ = log(pointsSum) * 80 + pointsSum * 0.01 - (resolution.width * 2.2);
	
	if(abs(abs(locationZ) - abs(tempLocationZ)) > jitterReductionInZ) {
		locationZ = tempLocationZ * followVelocityZ;
	}
	
	float factorZ = -(locationZ / (resolution.width * 0.9)) + 1;

	glTranslatef(locationX * factorZ, -(locationY * factorZ), locationZ);

	glRotatef(90.f, 1.f, 0.f, 0.f);

	//glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  RenderObject(model3D);
	//glDisable(GL_TEXTURE_2D);
}

void Renderer::renderBitmapString(int x, int y, void* font, const char* string, float r, float g, float b) { 
  const char* c;
  glColor3f(r, g, b);
  glRasterPos2f(x, y);
  for (c = string; *c != '\0'; c++) 
  glutBitmapCharacter(font, *c);
}

void Renderer::switchToOrthoView(Resolution resolution) {
	glDisable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, resolution.width, resolution.height, 0, -1.0, 1.0);
}

void Renderer::switchToPerspectiveView(Resolution resolution) {
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)resolution.width / (GLfloat)resolution.height, 10.0f, 10000.0f);
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
}

int Renderer::calcPowerOf2(int n) {
	int powerOf2 = 2;
	while(n > 1)
	{
		n = n / 2;
		powerOf2 = powerOf2 * 2;
	}
	return powerOf2;
}