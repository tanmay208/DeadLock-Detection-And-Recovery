struct VectorCouple{
	float ax, ay, bx, by;
};

VectorCouple getArrowHead(float x1, float y1, float x2, float y2, float slope){
	float px1, py1, px2, py2;
	float pax, pay, pbx, pby;
	float xd, yd;

	px1 = x1;
	py1 = y1;
	px2 = x2;
	py2 = y2;

	float d = 43;

	xd = 10 * cos(atan(slope));
	yd = 10 * sin(atan(slope));

	if(xd<0) xd*=-1;
	if(yd<0) yd*=-1;

	float xxd = (7 * cos(atan(-1/slope)));
	float yyd = (7 * sin(atan(-1/slope)));

	if(xxd<0) xxd*=-1;
	if(yyd<0) yyd*=-1;

	if(px2>px1 && py2>py1){
		pax = (px2 - xd) + xxd;
		pay = (py2 - yd) - yyd;
	
		pbx = (px2 - xd) - xxd;
		pby = (py2 - yd) + yyd;

	}

	if(px2>px1 && py2<py1){
		xxd = (7 * cos(atan(1/slope)));
		yyd = (7 * sin(atan(1/slope)));
		pax = (px2 - xd) - xxd;
		pay = (py2 + yd) + yyd;
	
		pbx = (px2 - xd) + xxd;
		pby = (py2 + yd) - yyd;

	} 

	if(px2<px1 && py2>py1){
		xxd = (7 * cos(atan(1/slope)));
		yyd = (7 * sin(atan(1/slope)));
		pax = (px2 + xd) - xxd;
		pay = (py2 - yd) + yyd;
	
		pbx = (px2 + xd) + xxd;
		pby = (py2 - yd) - yyd;

	} 

	if(px2<px1 && py2<py1){
		pax = (px2 + xd) + xxd;
		pay = (py2 + yd) - yyd;
	
		pbx = (px2 + xd) - xxd;
		pby = (py2 + yd) + yyd;
	}

	if(px1==px2){
		if(py2>py1){
			pax = px1 - 7;
			pbx = px2 + 7;
			
			pay = pby = (py2 - 10); 

		} else {
			pax = px1 - 7;
			pbx = px2 + 7;
			
			pay = pby = (py2 + 10); 
		}
	}

	if(py1==py2){
		if(px2>px1){
			pay = py1 - 7;
			pby = py2 + 7;
			
			pax = pbx = (px2 - 10); 

		} else {
			pay = py1 - 7;
			pby = py2 + 7;
			
			pax = pbx = (px2 + 10); 
			
		}
	}

	VectorCouple temp;
	temp.ax = pax;
	temp.ay = pay;
	temp.bx = pbx;
	temp.by = pby;

	return temp;
}

VectorCouple getArrow(float x, float y, float x2, float y2){
	int d = 58;

	float slope = (y2-y)/(x2-x);

	float ax, ay, bx, by;

	if(x2!=x){

		float xd = d * cos(atan(slope));
		float yd = d * sin(atan(slope));

		if(xd < 0) xd*=-1;
		if(yd < 0) yd*=-1;
		
		if((x2>x)&&(y2>y)){
			

			ax = x + xd;
			ay = y + yd;

			bx = x2 - xd;
			by = y2 - yd;
		} else if((x2<x)&&(y2>y)){
			
			ax = x - xd;
			ay = y + yd;

			bx = x2 + xd;
			by = y2 - yd;
		} else if((x2<x)&&(y2<y)){

			ax = x - xd;
			ay = y - yd;

			bx = x2 + xd;
			by = y2 + yd;
		} else if((x2>x)&&(y2<y)){

			ax = x + xd;
			ay = y - yd;

			bx = x2 - xd;
			by = y2 + yd;
		}

		if(y2==y && x2>x){
			ax = x + 43;
			ay = y;

			bx = x2 - 43;
			by = y2;
		} else if(y2==y && x2<x){
			ax = x - 43;
			ay = y;

			bx = x2 + 43;
			by = y2;
		}
	} else {
		if(y2>y){
			ax = x;
			ay = y + 43;

			bx = x2;
			by = y2 - 43;
		} else {
			ax = x;
			ay = y - 43;

			bx = x2;
			by = y2 + 43;

		}
	}

	VectorCouple temp;
	temp.ax = ax;
	temp.ay = ay;
	temp.bx = bx;
	temp.by = by;

	return temp;
}