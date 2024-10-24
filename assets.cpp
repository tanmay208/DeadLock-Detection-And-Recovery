#include<iostream>

#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

#include "mathassets.cpp"

bool Example1, Example2, Example3;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

bool popup_loop, safe, bankers, play, deadlock, recovering, popup_final, bankersAgain;
std::vector<int> safeseq;

void drawCircle(float cx, float cy, float r, int num_segments) { 
	glBegin(GL_POLYGON); 
	for(int i = 0; i < num_segments; i++){ 
		float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
		float x = r * cosf(theta);
		float y = r * sinf(theta);
		glVertex2f(x + cx, y + cy);
	}
	glEnd(); 
}

void drawTextonWindow(const char *text, int length, float x, float y){
	//TEXT
	glMatrixMode( GL_PROJECTION ) ;
	glPushMatrix() ; // save
	glLoadIdentity();// and clear
	glMatrixMode( GL_MODELVIEW ) ;
	glPushMatrix() ;
	glLoadIdentity() ;

	glDisable( GL_DEPTH_TEST ) ; // also disable the depth test so renders on top

	glRasterPos2f( x, y) ; // center of screen. (-1,0) is center left.
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	char buf[300];
	sprintf(buf, "%s", text ) ;
	const char * p = buf ;
	do glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *p ); while( *(++p) ) ;

	glEnable( GL_DEPTH_TEST ) ; // Turn depth testing back on

	glMatrixMode( GL_PROJECTION ) ;
	glPopMatrix() ; // revert back to the matrix I had before.
	glMatrixMode( GL_MODELVIEW ) ;
	glPopMatrix() ;
}

void drawBigTextonWindow(const char *text, int length, float x, float y){
	//TEXT
	glMatrixMode( GL_PROJECTION ) ;
	glPushMatrix() ; // save
	glLoadIdentity();// and clear
	glMatrixMode( GL_MODELVIEW ) ;
	glPushMatrix() ;
	glLoadIdentity() ;

	glDisable( GL_DEPTH_TEST ) ; // also disable the depth test so renders on top

	glRasterPos2f( x, y) ; // center of screen. (-1,0) is center left.
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	char buf[300];
	sprintf(buf, "%s", text ) ;
	const char * p = buf ;
	do glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, *p ); while( *(++p) ) ;

	glEnable( GL_DEPTH_TEST ) ; // Turn depth testing back on

	glMatrixMode( GL_PROJECTION ) ;
	glPopMatrix() ; // revert back to the matrix I had before.
	glMatrixMode( GL_MODELVIEW ) ;
	glPopMatrix() ;
}

void drawPixel(float x, float y){
	//glColor3f(1.0,0.0,0.0);
	glPointSize(5);
	glBegin(GL_POINTS);
		glVertex2f(x, y);
	glEnd();
}

struct Vector2D{
	float x, y;
};

Vector2D worldToScreen(float x, float y){
	float worldWidth = 900, worldHeight = 600;
	float screenx = ((x/worldWidth)*2) - 1;
	float screeny = ((y/worldHeight)*2) - 1;
	Vector2D screen;
	screen.x = screenx;
	screen.y = screeny;
	return screen;
}

class Arrow{

private:
	VectorCouple arrowLine;

	float pax, pay, pbx, pby, slope, tax, tay, tbx, tby;

	float xd, yd;

	

public:

	static int size;
	int id;
	bool drawable, inverted;

	Arrow(VectorCouple line, float slope){
		VectorCouple temp;
		temp = getArrowHead(line.ax, line.ay, line.bx, line.by, slope);

		arrowLine = line;
		drawable = true;
		inverted = false;
		id = size++;

		pax = temp.ax;
		pay = temp.ay;
		pbx = temp.bx;
		pby = temp.by;

		temp = getArrowHead(line.bx, line.by, line.ax, line.ay, slope);

		tax = temp.ax;
		tay = temp.ay;
		tbx = temp.bx;
		tby = temp.by;

		this->slope = slope;
	}

	void draw(){
		glColor3f(0.4,0.4,0.4);

		glLineWidth(3);
		glBegin(GL_LINES);
			glVertex2f(arrowLine.ax, arrowLine.ay);
			glVertex2f(arrowLine.bx, arrowLine.by);
		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_TRIANGLES);
		if(!inverted){
			glVertex2f(pax, pay);
			glVertex2f(arrowLine.bx, arrowLine.by);
			glVertex2f(pbx, pby);
		} else {
			glVertex2f(tax, tay);
			glVertex2f(arrowLine.ax, arrowLine.ay);
			glVertex2f(tbx, tby);
		}
		glEnd();
	}

};

std::vector<Arrow> arrows;
GLfloat selectColor[3];

class Object{
public:
	static int size;

	float x, y;
	int id;
	int type;

	bool selected;

	std::string text;

	std::vector<int> arrs;
	std::vector<int> objs;

	Object(int type, std::string text, int x, int y){
		this->type = type;

		this->text = text;

		this->x = (float)x;
		this->y = (float)y;

		selected = false;

		id = size++;
	}

	void allocObject(Object *obj){
		VectorCouple line = getArrow(x, y, obj->x, obj->y);

		objs.push_back(obj->id);

		float slope = (obj->y - y)/(obj->x - x);

		Arrow temp(line, slope);
		arrows.push_back(temp);

		arrs.push_back(arrows[arrows.size()-1].id);
	}
};

class Resource : public Object{
private:

	int length;
	int breadth;

	int side;
	Vector2D screen;

public:

	Resource(std::string text, int x, int y) : Object(0, text, x, y){

		this->side = 2;

		length = 50;
		breadth = 50;

		selectColor[0] = 1.0; 
		selectColor[1] = 0.8;
		selectColor[2] = 0.0;

		if(side == 0)
			screen = worldToScreen(x-8, y+43);
		else if(side == 1)
			screen = worldToScreen(x+43, y-5);
		else if(side == 2)
			screen = worldToScreen(x-8, y-43);
		else
			screen = worldToScreen(x-43, y-5);
	}

	Resource(std::string text, int side, int x, int y) : Object(0, text, x, y){

		this->side = side;

		length = 50;
		breadth = 50;

		selectColor[0] = 1.0; 
		selectColor[1] = 0.8;
		selectColor[2] = 0.0;

		if(side == 0)
			screen = worldToScreen(x-8, y+40);
		else if(side == 1)
			screen = worldToScreen(x+43, y-5);
		else if(side == 2)
			screen = worldToScreen(x-8, y-46);
		else
			screen = worldToScreen(x-43, y-5);
	}

	//void allocateProcess(Process *p);

	void draw(){
		glColor3f(0.72, 0.72, 0.72);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		glRecti(x - length/2, y - breadth/2, x + length/2, y + breadth/2);
		glColor3f(0.49, 0.49, 0.49);
		glLineWidth(2);
		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
		glRecti(x - length/2, y - breadth/2, x + length/2, y + breadth/2);
		
		glPointSize(2);

		int temp_x = (x - length/2) + 30;
		glColor3f(0.25, 0.25, 0.25);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

		drawCircle(temp_x-5, y, 5, 8);

		if(selected){
			glColor3fv(selectColor);
			glLineWidth(4);
			glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
			glRecti(x - length/2 - 1, y - breadth/2 - 1, x + length/2 + 1, y + breadth/2 + 1);
		}

		glColor3f(0.2,0.2,0.2);

		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
	}
};

class Process : public Object{
private:
	int radius = 29, num_segments = 360;

public:

	Process(std::string text, int x, int y):Object(1, text, x, y){
		selectColor[0] = 1.0; 
		selectColor[1] = 0.8;
		selectColor[2] = 0.0;
	}

	void draw(){

		glColor3f(0.27, 0.35, 0.38);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		drawCircle(x, y, radius+2, num_segments);

		if(selected && play){
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glColor3fv(selectColor);
			glLineWidth(3);
			drawCircle(x, y, radius+4, num_segments);
		}

		glColor3f(0.36, 0.45, 0.49);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		drawCircle(x, y, radius, num_segments);

		
		glColor3f(1.0,1.0,1.0);
		Vector2D screen; 
		screen = worldToScreen(x-8, y-5);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
	}
};

class simLine{
public:
	float mx, my;
	float xd, yd;
	float slope;

	VectorCouple line;

	int arrowId;

	float total_dist, dist_covered;
	Object *obj2;

	bool stopAnim, rollback;

	simLine(Object *obj1, Object *obj2){

		for(int i = 0; i < obj1->objs.size(); i++){
			if(obj1->objs[i]==obj2->id)
				arrowId = obj1->arrs[i];
		}

		this->obj2 = obj2;
		
		line.ax = obj1->x;
		line.ay = obj1->y;
		line.bx = obj2->x;
		line.by = obj2->y;
		
		slope = (line.by - line.ay)/(line.bx - line.ax);

		if(line.ax != line.bx){
			xd = 0.2 * cos(atan(slope));
			yd = 0.2 * sin(atan(slope));

			if(((line.ay < line.by) || (line.ay > line.by) || (line.ay == line.by)) && line.ax > line.bx){
				xd *= -1;
				yd *= -1;
			}
		} else {
			xd = 0;
			if(line.by>line.ay){
				yd = 0.2;
			} else {
				yd = -0.2;
			}
		}
		
		mx = line.ax + xd;
		my = line.ay + yd;

		obj1->selected = true;
		stopAnim = false;
		rollback = true;
	}

	void draw(){
		glLineWidth(7);
		glBegin(GL_LINES);
			glVertex2f(line.ax, line.ay);
			glVertex2f(mx, my);
		glEnd();
	}

	void idle(){
		if(!stopAnim){

			total_dist = sqrt(pow((line.bx-line.ax),2)+pow((line.by-line.ay),2));
			dist_covered = sqrt(pow((mx-line.ax),2)+pow((my-line.ay),2));

			if((dist_covered/total_dist) < 1.1){
				mx += xd;
				my += yd;
			} else {
				stopAnim = true;
			}

			if((dist_covered/total_dist) > 0.8){
				obj2->selected = true;	
			}
			

			if((dist_covered/total_dist) > 0.7)
				arrows[arrowId].drawable = false;
			
			glutPostRedisplay();
		}

		if(!rollback){
			if((dist_covered/total_dist) < 0.8)
			obj2->selected = false;

			total_dist = sqrt(pow((line.bx-line.ax),2)+pow((line.by-line.ay),2));
			dist_covered = sqrt(pow((mx-line.ax),2)+pow((my-line.ay),2));

			if((dist_covered/total_dist) > 0.1){
				mx -= xd;
				my -= yd;
			} else {
				rollback = true;
			}

			if((dist_covered/total_dist) < 0.7)
				arrows[arrowId].drawable = true;
			
			glutPostRedisplay();
		}
	}

};

class Simulator{
private:
	std::vector<Object*> objs;
	std::vector<simLine> lines;

	bool *visited;
	int *cur_indexer;

	int current, dest, loopId, rollbackId;

	bool done, redAlert, rolling;

	std::vector<int> traversed;

public:
	Simulator(std::vector<Object*> objs){
		this->objs = objs;

		current = 0;
		dest = objs[current]->objs[0];

		visited = new bool[objs.size()];
		cur_indexer = new int[objs.size()];

		for(int i = 0; i<objs.size(); i++){
			visited[i] = false;
			cur_indexer[i] = 0;
		}

		visited[current] = true;
		cur_indexer[current] = 1;

		traversed.push_back(current);
		
		simLine temp(objs[current], objs[dest]);
		lines.push_back(temp);

		current = dest;
		redAlert = false;
		rolling = false;
	}

	void draw(){
		glColor3f(1.0, 0.8, 0.0);

		if(redAlert)
			glColor3f(0.81, 0.15, 0.15);

		for(int i = 0; i < lines.size(); i++)
			lines[i].draw();
	}

	void idle(){
		for(int i = 0; i < lines.size(); i++)
			lines[i].idle();

		if(lines[lines.size()-1].stopAnim && lines[lines.size()-1].rollback && !redAlert){
			if(visited[current] && rolling){
				safe = true;
				popup_loop = true;
			}

			if(visited[current] && !rolling){
				redAlert = true;
				selectColor[0] = 0.81; 
				selectColor[1] = 0.15;
				selectColor[2] = 0.15;
				popup_loop = true;
			}

			if(rolling){
				lines.pop_back();
				traversed.pop_back();
			}
			if(!redAlert){
				if(objs[current]->objs.size() > cur_indexer[current]){
					if (std::find(traversed.begin(), traversed.end(), current) == traversed.end()) {
					  traversed.push_back(current);
					}
					dest = objs[current]->objs[cur_indexer[current]++];

					simLine temp(objs[current], objs[dest]);
					lines.push_back(temp);

					current = dest;
					
					rolling = false;
				} else {
					lines[lines.size()-1].rollback = false;
					
					rolling = true;

					current = traversed[traversed.size()-1];
				}
			}
		}
	}
};

struct Bprocess{
	bool executed, exception;
	int id;
	int available;
	std::vector<int> res_n;
	std::vector<int> res_a;
	GLfloat color[4];
};

std::vector<Bprocess> processs;
std::vector<Object*> objs;

void initBankers(){
	for(int i = 0; i<objs.size();i++){
		if(objs[i]->type==1){
			Bprocess temp;
			temp.id = objs[i]->id;
			for(int j = 0; j < objs[i]->objs.size(); j++)
				temp.res_n.push_back(objs[i]->objs[j]);
			temp.available = -1;
			temp.executed = false;
			temp.exception = false;

			temp.color[0] = 0;
			temp.color[1] = 0;
			temp.color[2] = 0;
			temp.color[3] = 0.07;

			processs.push_back(temp);
		}
	}
	
	for(int i = 0; i < objs.size();i++){
		if(objs[i]->type==0) {
			for(int j = 0; j<objs[i]->objs.size(); j++){
				for(int x = 0; x<processs.size(); x++){
					if(processs[x].id == objs[i]->objs[j]){
						processs[x].res_a.push_back(objs[i]->id);
					}
				}
			}
		}
	}
}

class Bankers{
	float timer;
	int process;
	int available;
	GLfloat scolor[3];

	bool iter, cal, exception;
public:
	Bankers(){
		available = 0;
		timer = 0;
		process = 0;
		processs[process].available = available;
		
		scolor[0] = 0.34;
		scolor[1] = 0.34;
		scolor[2] = 0.34;

		iter = true;
		safe = false;

		cal = true;

		exception = false;

		safeseq.clear();
	}

	void draw(){

		if(iter){
			glColor3fv(scolor);
			glLineWidth(1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glRecti(520, 470-(40*process), 840, 510-(40*process));
		}

		if(safe){

		}

	}

	void idle(){

		if(iter)
			timer+=0.01;

		if(timer>8000 && cal){

			if(processs[process].res_n.size()<=available){
				available = available + processs[process].res_a.size();
						
				processs[process].res_a.clear();
				processs[process].res_n.clear();

				processs[process].executed = true;
				
				scolor[0] = 0.33;
				scolor[1] = 0.73;
				scolor[2] = 0.32;

				if(!processs[process].exception){
					processs[process].color[0] = 0.33;
					processs[process].color[1] = 0.73;
					processs[process].color[2] = 0.32;
					processs[process].color[3] = 0.3;
				}

				iter = false;

				for(int i = 0; i<processs.size(); i++){
					if(processs[i].res_n.size() <= available && !processs[i].executed){
						iter |= true;
					} else {
						iter |= false;
					}
				}

				if(!iter){
					safe = true;
					for(int i = 0; i<processs.size(); i++){
						if(!processs[i].executed){
							processs[i].color[0] = 0.81;
							processs[i].color[1] = 0.15;
							processs[i].color[2] = 0.15;
							processs[i].color[3] = 0.3;
						}
						safe &= processs[i].executed;
					}

					if(!safe)
						deadlock = true;
				}
					
				if(!processs[process].exception)
					safeseq.push_back(process);

			} else {
				scolor[0] = 0.81;
				scolor[1] = 0.15;
				scolor[2] = 0.15;
			}

			cal = false;
			
			glutPostRedisplay();
		}

		if(timer>12000){

			cal = true;

			scolor[0] = 0.34;
			scolor[1] = 0.34;
			scolor[2] = 0.34;

			timer=0;

			if(process == processs.size()-1){
				iter = false;
				for(int i = 0; i<processs.size(); i++){
					if(processs[i].res_n.size() <= available && !processs[i].executed){
						iter |= true;
					} else {
						iter |= false;
					}
				}
			}

			
			if(iter){
				process++;

				if(process>=processs.size() || processs[process].executed)
					process = 0;

				for(int i = process; i<processs.size(); i++){
					if(!processs[i].executed){
						process = i;
						break;
					}
				}
			
				processs[process].available = available;
			
			} else {
				safe = true;
				for(int i = 0; i<processs.size(); i++){
					if(!processs[i].executed){
						processs[i].color[0] = 0.81;
						processs[i].color[1] = 0.15;
						processs[i].color[2] = 0.15;
						processs[i].color[3] = 0.3;
					}
					safe &= processs[i].executed;
				}

				if(!safe)
					deadlock = true;
			}

			glutPostRedisplay();
		}
	}
};

class Ex2{
	private:

	Vector2D screen;

	std::string text;
	Bankers *banker;

public:
	Process *p0, *p1, *p2, *p3, *p4;
	Resource *r0, *r1, *r2, *r3, *r4;
	Simulator *sim;

	void init(){

		popup_loop = false;
		bankers = false;
		play = false;
		deadlock = false;

		objs.clear();
		arrows.clear();
		safeseq.clear();
		processs.clear();

		p0 = new Process("P0", 100, 250);
		r0 = new Resource("R0", 0, 100, 400);

		p1 = new Process("P1", 250, 250);
		r1 = new Resource("R1", 2, 100, 100);

		p2 = new Process("P2", 400, 250);
		r2 = new Resource("R2", 1, 250, 400);

		p3 = new Process("P3", 250, 100);
		r3 = new Resource("R3", 0, 400, 400);

		p4 = new Process("P4", 250, 520);
		r4 = new Resource("R5", 2, 400, 100);

		p0->allocObject(r0);
		r0->allocObject(p1);

		
		p1->allocObject(r2);
		p1->allocObject(r3);
		p1->allocObject(r4);
		r1->allocObject(p0);

		p2->allocObject(r4);
		r2->allocObject(p4);

		p3->allocObject(r1);
		r3->allocObject(p2);

		r4->allocObject(p3);

		objs.push_back(p0);
		objs.push_back(r0);
		objs.push_back(p1);
		objs.push_back(r1);
		objs.push_back(p2);
		objs.push_back(r2);
		objs.push_back(p3);
		objs.push_back(r3);
		objs.push_back(p4);
		objs.push_back(r4);

		sim = new Simulator(objs);

		initBankers();

		banker = new Bankers();

	}

	void draw(){
		for(int i=0; i<arrows.size();i++)
			if(arrows[i].drawable) arrows[i].draw();

		if(!bankers && play)
			sim->draw();
		
		if(bankers)
			banker->draw();

		p0->draw();
		p1->draw();
		p2->draw();
		p3->draw();
		p4->draw();
		
		r0->draw();
		r1->draw();
		r2->draw();
		r3->draw();
		r4->draw();

		glColor3f(0.7, 0.7, 0.7);
		glLineWidth(1);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

		glColor4f(0, 0, 0, 0.6);
		glRecti(520, 510, 840, 550);
		glColor3f(1,1,1);
		text = "Process";
		screen = worldToScreen(525, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Alloc.";
		screen = worldToScreen(615, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Need";
		screen = worldToScreen(705, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Avail.";
		screen = worldToScreen(775, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

		for(int i = 0; i<processs.size(); i++){
			glColor4fv(processs[i].color);
			glRecti(520, 470-(40*i), 840, 510-(40*i));
			glColor3f(0,0,0);
			text = "P" + patch::to_string(i);
			screen = worldToScreen(550, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (!bankers) ? "-" : patch::to_string(processs[i].res_a.size());
			screen = worldToScreen(635, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (!bankers) ? "-" : patch::to_string(processs[i].res_n.size());
			screen = worldToScreen(720, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (processs[i].available<0) ? "-" : patch::to_string(processs[i].available); 
			screen = worldToScreen(795, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(!bankers && play){
			glColor3f(1.0, 0.8, 0.0);
			glRecti(520, 60, 840, 100);
			glColor3f(0,0,0);
			text = "FINDING A CYCLE";
			screen = worldToScreen(600, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(bankers){
			glColor3f(0.34, 0.34, 0.34);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "BANKER'S ALGORITHM";
			screen = worldToScreen(600, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(safe){
			glColor3f(0,0,0);
			text = "Safe Sequence:";
			screen = worldToScreen(520, 260);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

			
			for(int i = 0; i<safeseq.size(); i++){
				glColor3f(0.15, 0.47, 0.05);

				text = "P"+patch::to_string(safeseq[i]);
				if(i!=(safeseq.size()-1))
					text += " ->";
				screen = worldToScreen(520+(i*50), 235);
				drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			}

			glColor3f(0.33, 0.74, 0.32);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "SYSTEM IS IN SAFE STATE";
			screen = worldToScreen(570, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(deadlock){
			glColor3f(0.81, 0.15, 0.15);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "SYSTEM IS IN DEADLOCK";
			screen = worldToScreen(585, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}


		if(popup_loop){
			glColor4f(0, 0, 0, 0.5);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glRecti(0, 0, 900, 600);

			glColor3f(1, 1, 1);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glRecti(450 - 290, 300 - 20, 450 + 290, 300 +20);

			glColor3f(0.4, 0.4, 0.4);
			glLineWidth(1);
			glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
			glRecti(450 - 290, 300 - 20, 450 + 290, 300 + 20);

			glColor3f(0,0,0);
			text = "System seems to be in DEADLOCK, press 'SPACE' to continue...";
			screen = worldToScreen(177, 295);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}
	}

	void idle(){
		if(!bankers && play)
			sim->idle();
		
		if(bankers)
			banker->idle();
	}

	void keyboard(unsigned char key, int x, int y){
		if(key==' '){
			if(!play){
				play = true;
			}
			if(popup_loop){
				bankers = true;
				popup_loop = false;
				for(int i = 0; i<arrows.size(); i++){
					arrows[i].drawable = true;
				}
				for(int i = 0; i<objs.size(); i++){
					objs[i]->selected = false;
				}
				glutPostRedisplay();
			}
		}
	}
};

class Ex1{
private:
	
	Vector2D screen;

	std::string text;
	Bankers *banker;

public:
	Process *p0, *p1, *p2;
	Resource *r0;

	Simulator *sim;
	void init(){

		popup_loop = false;
		bankers = false;
		play = false;
		deadlock = false;

		objs.clear();
		arrows.clear();
		safeseq.clear();
		processs.clear();

		p0 = new Process("P0", 90, 350);
		r0 = new Resource("R0", 0, 250, 350);

		p1 = new Process("P1", 400, 350);

		p2 = new Process("P2", 250, 200);

		p0->allocObject(r0);
		r0->allocObject(p1);
		p2->allocObject(r0);

		objs.push_back(p0);
		objs.push_back(r0);
		objs.push_back(p1);
		objs.push_back(p2);

		sim = new Simulator(objs);

		initBankers();

		banker = new Bankers();

	}

	void draw(){
		for(int i=0; i<arrows.size();i++)
			if(arrows[i].drawable) arrows[i].draw();

		if(!bankers && play)
			sim->draw();
		
		if(bankers)
			banker->draw();

		p0->draw();
		p1->draw();
		p2->draw();
		
		r0->draw();

		glColor3f(0.7, 0.7, 0.7);
		glLineWidth(1);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

		glColor4f(0, 0, 0, 0.6);
		glRecti(520, 510, 840, 550);
		glColor3f(1,1,1);
		text = "Process";
		screen = worldToScreen(525, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Alloc.";
		screen = worldToScreen(615, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Need";
		screen = worldToScreen(705, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Avail.";
		screen = worldToScreen(775, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

		for(int i = 0; i<processs.size(); i++){
			glColor4fv(processs[i].color);
			glRecti(520, 470-(40*i), 840, 510-(40*i));
			glColor3f(0,0,0);
			text = "P" + patch::to_string(i);
			screen = worldToScreen(550, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (!bankers) ? "-" : patch::to_string(processs[i].res_a.size());
			screen = worldToScreen(635, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (!bankers) ? "-" : patch::to_string(processs[i].res_n.size());
			screen = worldToScreen(720, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (processs[i].available<0) ? "-" : patch::to_string(processs[i].available); 
			screen = worldToScreen(795, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(!bankers && play){
			glColor3f(1.0, 0.8, 0.0);
			glRecti(520, 60, 840, 100);
			glColor3f(0,0,0);
			text = "FINDING A CYCLE";
			screen = worldToScreen(600, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(bankers){
			glColor3f(0.34, 0.34, 0.34);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "BANKER'S ALGORITHM";
			screen = worldToScreen(600, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(safe){
			glColor3f(0,0,0);
			text = "Safe Sequence:";
			screen = worldToScreen(520, 260);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

			
			for(int i = 0; i<safeseq.size(); i++){
				glColor3f(0.15, 0.47, 0.05);

				text = "P"+patch::to_string(safeseq[i]);
				if(i!=(safeseq.size()-1))
					text += " ->";
				screen = worldToScreen(520+(i*50), 235);
				drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			}

			glColor3f(0.33, 0.74, 0.32);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "SYSTEM IS IN SAFE STATE";
			screen = worldToScreen(570, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(deadlock){
			glColor3f(0.81, 0.15, 0.15);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "SYSTEM IS IN DEADLOCK";
			screen = worldToScreen(585, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}


		if(popup_loop){
			glColor4f(0, 0, 0, 0.5);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glRecti(0, 0, 900, 600);

			glColor3f(1, 1, 1);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glRecti(450 - 290, 300 - 20, 450 + 290, 300 +20);

			glColor3f(0.4, 0.4, 0.4);
			glLineWidth(1);
			glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
			glRecti(450 - 290, 300 - 20, 450 + 290, 300 + 20);

			glColor3f(0,0,0);
			text = "System seems to be in SAFE STATE, press 'SPACE' to continue..";
			screen = worldToScreen(177, 295);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}
	}

	void idle(){
		if(!bankers && play)
			sim->idle();
		
		if(bankers)
			banker->idle();
	}

	void keyboard(unsigned char key, int x, int y){
		if(key==' '){
			if(!play){
				play = true;
			}
			if(popup_loop){
				bankers = true;
				popup_loop = false;
				for(int i = 0; i<arrows.size(); i++){
					arrows[i].drawable = true;
				}
				for(int i = 0; i<objs.size(); i++){
					objs[i]->selected = false;
				}
				glutPostRedisplay();
			}
		}
	}
};

Bankers *banker;

class Recover{
public:

	float timer;
	bool step_one, step_two, step_three;

	Recover(){
		timer = 0;
		step_two = step_three = false;
		step_one = true;
	}

	void draw(){

	}

	void idle(){
		if(step_one || step_two || step_three)
			timer+=0.01;

		if(timer>12000 && step_one){

			objs[0]->selected = true;
			selectColor[0] = 0.81;
			selectColor[1] = 0.15;
			selectColor[2] = 1.0;

			step_one = false;
			step_two = true;
			timer = 0;

			glutPostRedisplay();
		}

		if(timer>18000 && step_two){

			arrows[5].drawable = false;
			arrows[0].drawable = false;

			step_two = false;
			step_three = true;
			timer = 0;

			glutPostRedisplay();
		}

		if(timer>20000 && step_three){
			arrows[4].inverted = true;

			objs[5]->objs.clear();
			objs[5]->objs.push_back(4);
			objs[4]->objs.clear();

			bankersAgain = true;
			//initBankers();
			processs[0].res_n.clear();
			processs[0].res_a.clear();
			processs[0].exception = true;

			processs[0].res_a.push_back(-1);
			processs[0].res_n.push_back(-1);

			processs[0].color[0] = 0.81;
			processs[0].color[1] = 0.15;
			processs[0].color[2] = 1;
			processs[0].color[3] = 0.3;

			processs[2].res_a.push_back(1);
			processs[2].res_n.clear();

			for(int i =1; i<processs.size(); i++){
				processs[i].executed = false;
				processs[i].color[0] = 0;
				processs[i].color[1] = 0;
				processs[i].color[2] = 0;
				processs[i].color[3] = 0.07;
			}

			bankers = true;
			recovering = false;

			step_three = false;
			timer = 0;

			banker = new Bankers();

			glutPostRedisplay();
		}
	}
};

class Ex3{
	private:

	//simLine *yLine;
	Vector2D screen;

	std::string text;
	
	Recover *recover;

public:
	Process *p0, *p1, *p2;
	Resource *r0, *r1, *r2;
	
	Simulator *sim;

	void init(){

		popup_final = false;
		recovering = false;

		popup_loop = false;
		bankers = false;
		play = false;
		deadlock = false;

		bankersAgain = false;

		objs.clear();
		arrows.clear();
		safeseq.clear();
		processs.clear();

		p0 = new Process("P0", 90, 300);
		r0 = new Resource("R0", 0, 180, 440);

		p1 = new Process("P1", 320, 440);
		r1 = new Resource("R1", 1, 410, 300);

		p2 = new Process("P2", 320, 150);
		r2 = new Resource("R2", 2, 180, 150);

		p0->allocObject(r0);
		r0->allocObject(p1);
		p1->allocObject(r1);
		r1->allocObject(p2);
		p2->allocObject(r2);
		r2->allocObject(p0);

		objs.push_back(p0);
		objs.push_back(r0);
		objs.push_back(p1);
		objs.push_back(r1);
		objs.push_back(p2);
		objs.push_back(r2);

		sim = new Simulator(objs);

		initBankers();

		banker = new Bankers();
		recover = new Recover();

	}

	void draw(){
		for(int i=0; i<arrows.size();i++)
			if(arrows[i].drawable) arrows[i].draw();

		if(!bankers && play && !recovering)
			sim->draw();
		
		if(bankers)
			banker->draw();

		p0->draw();
		p1->draw();
		p2->draw();
		
		r0->draw();
		r1->draw();
		r2->draw();

		glColor3f(0.7, 0.7, 0.7);
		glLineWidth(1);
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

		glColor4f(0, 0, 0, 0.6);
		glRecti(520, 510, 840, 550);
		glColor3f(1,1,1);
		text = "Process";
		screen = worldToScreen(525, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Alloc.";
		screen = worldToScreen(615, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Need";
		screen = worldToScreen(705, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		text = "Avail.";
		screen = worldToScreen(775, 525);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

		for(int i = 0; i<processs.size(); i++){
			glColor4fv(processs[i].color);
			glRecti(520, 470-(40*i), 840, 510-(40*i));
			glColor3f(0,0,0);
			text = "P" + patch::to_string(i);
			screen = worldToScreen(550, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (!bankers || processs[i].exception) ? "-" : patch::to_string(processs[i].res_a.size());
			screen = worldToScreen(635, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (!bankers || processs[i].exception) ? "-" : patch::to_string(processs[i].res_n.size());
			screen = worldToScreen(720, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			text = (processs[i].available<0 || processs[i].exception) ? "-" : patch::to_string(processs[i].available); 
			screen = worldToScreen(795, 485-(40*i));
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		/*glColor3f(0.34, 0.34, 0.34);
		glRecti(520, 120, 840, 160);
		glColor3f(1,1,1);
		text = "PROCESS TERMINATION";
		screen = worldToScreen(590, 135);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

		glColor3f(0.81, 0.15, 0.15);
		glRecti(520, 60, 840, 100);
		glColor3f(1,1,1);
		text = "RECOVERING FROM DEADLOCK";
		screen = worldToScreen(570, 75);
		drawTextonWindow(text.data(), text.size(), screen.x, screen.y);*/

		if(!bankers && play){
			glColor3f(1.0, 0.8, 0.0);
			glRecti(520, 60, 840, 100);
			glColor3f(0,0,0);
			text = "FINDING A CYCLE";
			screen = worldToScreen(600, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(bankers){
			glColor3f(0.34, 0.34, 0.34);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "BANKER'S ALGORITHM";
			screen = worldToScreen(600, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(safe){
			glColor3f(0,0,0);
			text = "Safe Sequence:";
			screen = worldToScreen(520, 260);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

			
			for(int i = 0; i<safeseq.size(); i++){
				glColor3f(0.15, 0.47, 0.05);

				text = "P"+patch::to_string(safeseq[i]);
				if(i!=(safeseq.size()-1))
					text += " ->";
				screen = worldToScreen(520+(i*50), 235);
				drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			}

			glColor3f(0.33, 0.74, 0.32);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "SYSTEM IS IN SAFE STATE";
			screen = worldToScreen(570, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(deadlock){
			glColor3f(0.81, 0.15, 0.15);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "SYSTEM IS IN DEADLOCK";
			screen = worldToScreen(585, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
			popup_final=true;
		}

		//mainmenu();


		if(popup_loop){
			glColor4f(0, 0, 0, 0.5);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glRecti(0, 0, 900, 600);

			glColor3f(1, 1, 1);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glRecti(450 - 290, 300 - 20, 450 + 290, 300 +20);

			glColor3f(0.4, 0.4, 0.4);
			glLineWidth(1);
			glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
			glRecti(450 - 290, 300 - 20, 450 + 290, 300 + 20);

			glColor3f(0,0,0);
			text = "System seems to be in DEADLOCK, press 'SPACE' to continue...";
			screen = worldToScreen(177, 295);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(popup_final){
			deadlock = false;

			glColor4f(0, 0, 0, 0.5);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glRecti(0, 0, 900, 600);

			glColor3f(1, 1, 1);
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glRecti(450 - 305, 300 - 20, 450 + 290, 300 +20);

			glColor3f(0.4, 0.4, 0.4);
			glLineWidth(1);
			glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
			glRecti(450 - 305, 300 - 20, 450 + 290, 300 + 20);

			glColor3f(0,0,0);
			text = "Press 'SPACE' to recover from DEADLOCK using PROCESS TERMINATION";
			screen = worldToScreen(155, 295);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}

		if(recovering){
			glRecti(520, 120, 840, 160);
			glColor3f(1,1,1);
			text = "PROCESS TERMINATION";
			screen = worldToScreen(590, 135);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

			glColor3f(0.81, 0.15, 0.15);
			glRecti(520, 60, 840, 100);
			glColor3f(1,1,1);
			text = "RECOVERING FROM DEADLOCK";
			screen = worldToScreen(570, 75);
			drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
		}
	}

	void idle(){
		if(!bankers && play && !recovering)
			sim->idle();
		
		if(bankers)
			banker->idle();

		if(recovering)
			recover->idle();
	}

	void keyboard(unsigned char key, int x, int y){

	}
};

void mainmenu(){
	Vector2D screen;
	std::string text;

	glColor3f(0.0, 0.24, 0.42);
	text = "SRM INSTITUTE OF SCIENCE & TECHNOLOGY";
	screen = worldToScreen(445 - 235, 500);
	drawBigTextonWindow(text.data(), text.size(), screen.x, screen.y);

	glColor3f(0,0,0);
	text = "Operating System Mini Project on:";
	screen = worldToScreen(450 - 180, 385);
	drawBigTextonWindow(text.data(), text.size(), screen.x, screen.y);

	glColor3f(0,0,0);
	text = "\"DEADLOCK DETECTION AND RECOVERY\"";
	screen = worldToScreen(450 - 238, 320);
	drawBigTextonWindow(text.data(), text.size(), screen.x, screen.y);

	glColor3f(0,0,0);
	text = "DONE BY:";
	screen = worldToScreen(385, 260);
	drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

	glColor3f(0,0,0);
	text = "TANMAY, ANSHIKA, ARIN, DIVYANSH";
	screen = worldToScreen(300, 230);
	drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
	glColor3f(0,0,0);
	text = "RA23110030010051/56/59/61";
	screen = worldToScreen(320, 210);
	drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

	glColor4f(0.4, 0.4, 0.4, 0.8);
	glRecti(100, 60, 250, 120);
	glColor3f(1,1,1);
	text = "Example #1";
	screen = worldToScreen(130, 85);
	drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

	glColor4f(0.4, 0.4, 0.4, 0.8);
	glRecti(370, 60, 520, 120);
	glColor3f(1,1,1);
	text = "Example #2";
	screen = worldToScreen(395, 85);
	drawTextonWindow(text.data(), text.size(), screen.x, screen.y);

	glColor4f(0.4, 0.4, 0.4, 0.8);
	glRecti(630, 60, 780, 120);
	glColor3f(1,1,1);
	text = "Example #3";
	screen = worldToScreen(660, 85);
	drawTextonWindow(text.data(), text.size(), screen.x, screen.y);
}

class Engine{

private:
	Ex1 *ex1;
	Ex2 *ex2;
	Ex3 *ex3;

public:
	void init(){
		Example1 = false;
		Example2 = false;
		Example3 = false;
	}

	void draw(){
		if(!Example1 && !Example2 && !Example3){
			mainmenu();
		}

		if(Example1){
			ex1->draw();
		}
		if(Example2){
			ex2->draw();
		}
		if(Example3){
			ex3->draw();
		}

	}

	void idle(){
		if(Example1){
			ex1->idle();
		}
		if(Example2){
			ex2->idle();
		}
		if(Example3){
			ex3->idle();
		}
	}

	void keyboard(unsigned char key, int x, int y){
		if(Example1 || Example2 || Example3){
			if(key==' '){
				if(!play){
					play = true;
				}
				if(popup_loop){
					bankers = true;
					popup_loop = false;
					for(int i = 0; i<arrows.size(); i++){
						arrows[i].drawable = true;
					}
					for(int i = 0; i<objs.size(); i++){
						objs[i]->selected = false;
					}
					glutPostRedisplay();
				}
				if(popup_final){
					recovering = true;
					bankers = false;
					popup_final = false;
					glutPostRedisplay();
				}
			}

			if(key == 27){
				Example1 = Example3 = Example2 = false;

				glutPostRedisplay();
			}
		}

		if(Example3){
			ex3->keyboard(key, x, y);
		}
	}

	void mouse(int button, int state, int x, int y){
		if(!Example1 && !Example2 && !Example3){
			y = 600-y;
			if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
				if(x>100 && x<250 && y<120 && y>60){
					Example1 = true;
					Object::size = 0;
					Arrow::size = 0;
					ex1 = new Ex1();
					ex1->init();
				}
				if(x>370 && x<520 && y<120 && y>60){
					Example2 = true;
					Object::size = 0;
					Arrow::size = 0;
					ex2 = new Ex2();
					ex2->init();
				}
				if(x>630 && x<780 && y<120 && y>60){
					Example3 = true;
					Object::size = 0;
					Arrow::size = 0;
					ex3 = new Ex3();
					ex3->init();
				}
				glutPostRedisplay();
			}
		}
	}
};

