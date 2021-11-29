#define _USE_MATH_DEFINES
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
using namespace std;

//zmienne ogolne
typedef float point3[3];
typedef float point9[9];
int testedObject = 2; //rysowany obiekt 
static int x_pos_old = 0;       // pozycje kursora myszy
static int delta_x = 0;
static int y_pos_old = 0;
static int delta_y = 0;
static GLfloat pix2angle;     // przelicznik pikseli na stopnie 

//zmienne obiektu
int n = 100;//ilosc punktow jajka
float scale = 3.0;//wielkosc obiektu
point3** points;//siatka punktow
point3** vectors;//wektory punktow powierzchni jajka

//zmienne obrotu obiektu
static GLint statusMiddle = 0;//stan srdkowego klawisza
float rViewer = 10;//R wokol punktu obserwowanego 
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };//pozycja punktu widzenia
static GLfloat azymuth = 0;    //katy obserwacji punktu obserwowanego
static GLfloat elevation = 0;

//punkty swietlne
static GLint statusLeft = 0;       // stan klawisza lewego
static GLint statusRight = 0;	   // stan klawisza prawego
float rLight = 10;				   //odleglosc zrodla swiatla 
static GLfloat lightRedAngles[] = { 0.0, 0.0 };//dane swiatla czerwonego
GLfloat redLightPosition[] = { 10.0, 10.0, 10.0, 1.0 };
static GLfloat lightBlueAngles[] = { 0.0, 0.0 };//dane swiatla niebieskiego
GLfloat blueLightPosition[] = { -10.0, 10.0, 10.0, 1.0 };

void Axes(void)
{
	point3  x_min = { -50.0, 0.0, 0.0 };//wspolrzedne koncow osi
	point3  x_max = { 50.0, 0.0, 0.0 };
	point3  y_min = { 0.0, -50.0, 0.0 };
	point3  y_max = { 0.0,  50.0, 0.0 };
	point3  z_min = { 0.0, 0.0, -50.0 };
	point3  z_max = { 0.0, 0.0,  50.0 };

	glColor3f(1.0f, 0.0f, 0.0f);//os x
	glBegin(GL_LINES);
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);//os y
	glBegin(GL_LINES);
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);//os z
	glBegin(GL_LINES);
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

void egg() {
	float u = 0, v = 0;
	float udiff = 1.0 / n, vdiff = 1.0 / n; //n - liczba punktow na powierzchni jajka
	glTranslated(0, (-(160 * pow(0.5, 4) - 320 * pow(0.5, 3) + 160 * pow(0.5, 2)) / 2) * (scale + 7) / 10, 0);//obnizenie środka figury do centrum ukladu wspolrzednych

	for (int i = 0; i <= n; i++) {//punkty powierzchni
		v = 0;//obliczenie poteg w celu ulatwienia kodu
		float u2 = pow(u, 2);
		float u3 = pow(u, 3);
		float u4 = pow(u, 4);
		float u5 = pow(u, 5);

		for (int ii = 0; ii <= n; ii++) {//obliczenie wspolrzednych punktow
			points[i][ii][0] = ((-90 * u5 + 225 * u4 - 270 * u3 + 180 * u2 - 45 * u) * cos(M_PI * v)) * (scale + 7) / 10;
			points[i][ii][1] = (160 * u4 - 320 * u3 + 160 * u2) * (scale + 7) / 10;
			points[i][ii][2] = ((-90 * u5 + 225 * u4 - 270 * u3 + 180 * u2 - 45 * u) * sin(M_PI * v)) * (scale + 7) / 10;
			v = v + vdiff;
		}
		u = u + udiff;
	}

	u = 0;
	for (int i = 0; i <= n; i++) {//wektory normalne
		v = 0;//obliczenie poteg w celu ulatwienia kodu
		float u2 = pow(u, 2);
		float u3 = pow(u, 3);
		float u4 = pow(u, 4);
		float u5 = pow(u, 5);

		for (int ii = 0; ii <= n; ii++) {
			point9 vector;
			vector[0] = (-450 * u4 + 900 * u3 - 810 * u2 + 360 * u - 45) * cos(M_PI * v);
			vector[1] = M_PI * (90 * u5 - 225 * u4 + 270 * u3 - 180 * u2 + 45) * sin(M_PI * v);
			vector[2] = 640 * u3 - 960 * u2 + 320 * u;
			vector[3] = 0;
			vector[4] = (-450 * u4 + 900 * u3 - 810 * u2 + 360 * u - 45) * sin(M_PI * v);
			vector[5] = -1 * M_PI * (90 * u5 - 225 * u4 + 270 * u3 - 180 * u2 + 45) * cos(M_PI * v);

			vector[6] = vector[2] * vector[5] - vector[4] * vector[3];//wektory 
			vector[7] = vector[4] * vector[1] - vector[0] * vector[5];
			vector[8] = vector[0] * vector[3] - vector[2] * vector[1];
			float vectorSize = sqrt(pow(vector[6], 2) + pow(vector[7], 2) + pow(vector[8], 2));//normalizacja wektora
			if (vectorSize == 0) {
				vectorSize = 1;
			}

			vectors[i][ii][0] = vector[6] / vectorSize;
			vectors[i][ii][1] = vector[7] / vectorSize;
			vectors[i][ii][2] = vector[8] / vectorSize;

			v = v + vdiff;
		}
		u = u + udiff;
	}

	for (int i = 0; i < n; i++) {//rysowanie 
		for (int ii = 0; ii < n; ii++) {
			glBegin(GL_TRIANGLES);//rysowanie pierwszego trojkata
			glNormal3fv(vectors[i][ii]);
			glVertex3f(points[i][ii][0], points[i][ii][1], points[i][ii][2]);
			glNormal3fv(vectors[i + 1][ii]);
			glVertex3f(points[i + 1][ii][0], points[i + 1][ii][1], points[i + 1][ii][2]);
			glNormal3fv(vectors[i + 1][ii + 1]);
			glVertex3f(points[i + 1][ii + 1][0], points[i + 1][ii + 1][1], points[i + 1][ii + 1][2]);
			glEnd();

			glBegin(GL_TRIANGLES);//rysowanie drugiego trojkata
			glNormal3fv(vectors[i][ii]);
			glVertex3f(points[i][ii][0], points[i][ii][1], points[i][ii][2]);
			glNormal3fv(vectors[i][ii + 1]);
			glVertex3f(points[i][ii + 1][0], points[i][ii + 1][1], points[i][ii + 1][2]);
			glNormal3fv(vectors[i + 1][ii + 1]);
			glVertex3f(points[i + 1][ii + 1][0], points[i + 1][ii + 1][1], points[i + 1][ii + 1][2]);
			glEnd();
		}
	}
}

void zadanie() {
	if (statusMiddle == 1) {//obracanie obiektu
		elevation += 0.01 * delta_y * pix2angle;
		azymuth += 0.01 * delta_x * pix2angle;
	}

	viewer[0] = rViewer * cos(azymuth) * cos(elevation);//obliczenie pozycji punktu widzenia
	viewer[1] = rViewer * sin(elevation);
	viewer[2] = rViewer * sin(azymuth) * cos(elevation);

	if (statusLeft == 1) {//przesuwanie czerwonego swiatla
		lightRedAngles[0] += delta_x * pix2angle / 100;
		lightRedAngles[1] += delta_y * pix2angle / 100;
	}

	if (statusRight == 1) {//przesuwanie niebieskieg swiatla
		lightBlueAngles[0] += delta_x * pix2angle / 100;
		lightBlueAngles[1] += delta_y * pix2angle / 100;
	}

	redLightPosition[0] = rLight * cos(lightRedAngles[0]) * cos(lightRedAngles[1]);//obliczenie pozycji czerwonego swiatla
	redLightPosition[1] = rLight * sin(lightRedAngles[1]);
	redLightPosition[2] = rLight * sin(lightRedAngles[0]) * cos(lightRedAngles[1]);

	blueLightPosition[0] = rLight * cos(lightBlueAngles[0]) * cos(lightBlueAngles[1]);//obliczenie pozycji niebieskiego swiatla
	blueLightPosition[1] = rLight * sin(lightBlueAngles[1]);
	blueLightPosition[2] = rLight * sin(lightBlueAngles[0]) * cos(lightBlueAngles[1]);



	glLightfv(GL_LIGHT0, GL_POSITION, redLightPosition);//aktualizacja pozycji zrodla swiatla
	glLightfv(GL_LIGHT1, GL_POSITION, blueLightPosition);

	switch (testedObject) {//wybranie obiektu do wyswietlania
	case 1:
		glColor3f(1.0f, 1.0f, 1.0f);
		glutSolidTeapot(scale);
		break;
	case 2:
		scale = 3.0;
		egg();
		break;
	}
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//deklaracja buforow
	glLoadIdentity();
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);//ustawienie pozycji punktu widzenia i pozycji punktu obserwowanego

	Axes();
	zadanie();

	glutSwapBuffers();//zmiana buforow i wyswietlanie
}

void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal prawy klawisz
		x_pos_old = x;
		y_pos_old = y;
		statusRight = 1;        //ustawienie flagi przycisku   
	}
	else {
		statusRight = 0;		//ustawienie flagi przycisku  
	}

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal lewy klawisz
		x_pos_old = x;
		y_pos_old = y;
		statusLeft = 1;    //ustawienie flagi przycisku        
	}
	else {
		statusLeft = 0;		//ustawienie flagi przycisku  
	}

	if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal lewy klawisz
		x_pos_old = x;
		y_pos_old = y;
		statusMiddle = 1;    //ustawienie flagi przycisku        
	}
	else {
		statusMiddle = 0;		//ustawienie flagi przycisku  
	}

	RenderScene();
}

void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;//zmiana pozycji x
	x_pos_old = x;

	delta_y = y - y_pos_old;//zmiana pozycji y
	y_pos_old = y;

	glutPostRedisplay();
}

void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };//okreslenie parametrow materialu
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess = { 20.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);//ustawienie parametrow materialu
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };//pocztkoawa pozycja punktow swietlnych

	GLfloat att_constant = { 1.0 };//okreslenia parametrow swiatla
	GLfloat att_linear = { 0.05 };
	GLfloat att_quadratic = { 0.001 };
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };


	GLfloat redlight_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };//parametry swiatla czerwonego
	GLfloat redlight_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat bluelight_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };//parametry swiatla niebieskiego
	GLfloat bluelight_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);//ustawienie GL_LIGHT0 na czerwone zrodlo swiatla
	glLightfv(GL_LIGHT0, GL_DIFFUSE, redlight_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, redlight_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, redLightPosition);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);//ustawienie GL_LIGHT0 na niebieskie zrodlo swiatla
	glLightfv(GL_LIGHT1, GL_DIFFUSE, bluelight_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, bluelight_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, blueLightPosition);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);


	//uruchomienie oswietlenia
	glShadeModel(GL_SMOOTH); // wlaczenie lagodnego cieniowania
	glEnable(GL_LIGHTING);   // wlaczenie systemu oświetlenia sceny
	glEnable(GL_LIGHT0);     // wlaczenie zrodla czerwonego
	glEnable(GL_LIGHT1);	// wlaczenie zrodla niebieskiego
	glEnable(GL_DEPTH_TEST); // wlaczenie mechanizmu z-bufora

}

void keys(unsigned char key, int x, int y)
{
	if (key == 'c') {
		testedObject = 1;
	}
	if (key == 'j') {
		testedObject = 2;
	}

	RenderScene();
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;//usatwienie wielkosci do obracania obiektu
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1.0, 1.0, 100000.0);

	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void main(void)
{
	srand(time(NULL));
	points = new  point3 * [n + 1];//tablica punktow
	vectors = new point3 * [n + 1];//tablice
	for (int i = 0; i <= n; i++) {
		points[i] = new point3[n + 1];
		vectors[i] = new point3[n + 1];
	}

	cout << "Obsluga programu:\nc - czajnik\nj - jajko\nlewy przycisk myszy - aktywuje ruch czerwonego swiatla\nprawy przycisk myszy - altywuje ruch niebieskiego swiatla\nsrodkowy przycisk myszy - ruch obiektu" << endl;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Oświetlenie 3D");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	MyInit();
	glutMouseFunc(Mouse);//"lapanie" akcji na przyciskach myszy
	glutMotionFunc(Motion);//"lapanie" ruchu myszki
	glutKeyboardFunc(keys);//"lapanie" akcji na klawiaturze
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}