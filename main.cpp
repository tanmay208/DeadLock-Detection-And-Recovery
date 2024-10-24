#include<GL/glut.h>

#include "assets.cpp"

int Object::size = 0;
int Arrow::size = 0;

Engine *e1;

void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	
	e1->draw();

	glFlush();
}

void idle(){
	e1->idle();
}

void keyboard(unsigned char key, int x, int y){
	e1->keyboard(key, x, y);
}

void mouse(int button, int state, int x, int y){
	e1->mouse(button, state, x, y);
}

int main(int argc, char *argv[]){
	
	e1 = new Engine();
	e1->init();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(900, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Deadlock Detection and Recovery");

	glClearColor(0.94,0.92,0.81,0.0);
	glOrtho(0, 900, 0, 600, -1, 1);

	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	glutMainLoop();

}