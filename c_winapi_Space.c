#include <stdio.h>
#include <windows.h>
#include <math.h>


static const int frameRate = 64;

static int running = 1;

static int keys[256] = { 0 };

struct {
	int width;
	int height;
	int* pixels;
} frame = { 0 };


//---------------------------------------------

struct {
	double x;
	double y;
	double dir;
	double mass;	//kg (kilograms)
	double thrust;	//N (Newtons)
	double forces[100][2];	//N (Newtons)
	double velocity[2];	//N (Newtons)
	double hitbox[5][2];	//[width, height], [x1, y1], [x2, y2]..
} player = { 0 };

const forceAmountCap = 100;

const int showPhysics = 0;

static double zoomEffect = 1;


double DeltasToDegrees(double deltaX, double deltaY) {

	if (deltaX == 0 && deltaY == 0) {
		return -1;
	};

	if (deltaY == 0 && deltaX > 0) {
		return 0;
	};

	if (deltaY == 0 && deltaX < 0) {
		return 180;
	};

	if (deltaX == 0 && deltaY > 0) {
		return 90;
	};

	if (deltaX == 0 && deltaY < 0) {
		return 270;
	};

	double angle = atan(deltaY / deltaX) * 180 / 3.14;
	if (deltaX < 0 && deltaY < 0) {	//Quadrant III
		return angle + 180;
	};

	if (deltaX < 0 && deltaY > 0) {	//Quadrant I
		return angle + 180;
	};

	return angle;
}

int NormalizeAngle(int angle) {


	while (angle >= 360) {
		angle -= 360;
	};

	while (angle < 0) {
		angle += 360;
	};

	return angle;
};


void DrawLine(int x1, int y1, int x2, int y2, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	double deltaX = x2 - x1;
	double deltaY = y2 - y1;

	if ((x1 == x2) && (y1 == y2)) {	//if given a point
		return;
	};

	double tIncrement = 1 / sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

	//t goes from 0 - 1; 1 is endpoint
	for (double t = 0; t <= 1; t += tIncrement) {

		int x = (int)(x1 + deltaX * t);
		int y = (int)(y1 + deltaY * t);

		if ((x <= 0) || (x >= frame.width)) {
			continue;
		};

		int pixel = y * frame.width + x;

		if ((pixel < 0) || (pixel > frame.width * frame.height)) {
			continue;
		};

		frame.pixels[pixel] = color;

	};

	return;
}


void DrawLineA(double x1, double y1, double angle, double length, int color) {

	if (angle == 90) {
		DrawLine(x1, y1, x1, y1 + length, color);
		return;
	};

	if (angle == 270) {
		DrawLine(x1, y1, x1, y1 - length, color);
		return;
	};

	DrawLine(x1, y1, (int)(x1 + length * cos(angle * 3.14 / 180)), (int)(y1 + length * sin(angle * 3.14 / 180)), color);

	return;
}


void DrawRect(double x1, double y1, double x2, double y2, int color) {

	double width = x2 - x1;
	double height = y2 - y1;

	DrawLine(x1, y1, x2, y1, color);	//bottom
	DrawLine(x2, y1, x2, y2, color);	//right
	DrawLine(x2, y2, x1, y2, color);	//top
	DrawLine(x1, y2, x1, y1, color);	//left

	return;
};


void DrawRectFilled(int x1, int y1, int x2, int y2, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	if (x1 > x2) {
		int temp = x1;
		x1 = x2;
		x2 = temp;
	};

	if (y1 > y2) {
		int temp = y1;
		y1 = y2;
		y2 = temp;
	};

	for (int x = x1; x < x2; x++) {

		if (x < 0 || x >= frame.width) {
			continue;
		};

		for (int y = y1; y < y2; y++) {

			if (y < 0 || y >= frame.height) {
				continue;
			}

			frame.pixels[y * frame.width + x] = color;

		};

	};

	return;
};


void DrawRectA(int xAnchor, int yAnchor, int horizontalShift, int verticalShift, double angle, int width, int height, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};


	int x1 = xAnchor + horizontalShift * cos((angle - 90) * 3.14 / 180) + verticalShift * cos(angle * 3.14 / 180);
	int y1 = yAnchor + horizontalShift * sin((angle - 90) * 3.14 / 180) + verticalShift * sin(angle * 3.14 / 180);

	int x2 = x1 + height * cos(angle * 3.14 / 180);
	int y2 = y1 + height * sin(angle * 3.14 / 180);

	int x4 = x1 + width * cos((angle - 90) * 3.14 / 180);
	int y4 = y1 + width * sin((angle - 90) * 3.14 / 180);

	int x3 = x1 + height * cos(angle * 3.14 / 180) + width * cos((angle - 90) * 3.14 / 180);
	int y3 = y1 + height * sin(angle * 3.14 / 180) + width * sin((angle - 90) * 3.14 / 180);

	DrawLine(x1, y1, x2, y2, color);
	DrawLine(x2, y2, x3, y3, color);
	DrawLine(x3, y3, x4, y4, color);
	DrawLine(x4, y4, x1, y1, color);

	return;
};


void DrawRectAFilled(int xAnchor, int yAnchor, int horizontalShift, int verticalShift, int angle, int width, int height, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	if (angle == 90) {
		DrawRectFilled(xAnchor + horizontalShift, yAnchor + verticalShift, xAnchor + width + horizontalShift, yAnchor + height + verticalShift, color);
		return;
	};


	if (angle == 270) {
		DrawRectFilled(xAnchor - horizontalShift, yAnchor - verticalShift, xAnchor - width - horizontalShift, yAnchor - height - verticalShift, color);
		return;
	};

	int x1 = xAnchor + horizontalShift * cos((angle - 90) * 3.14 / 180) + verticalShift * cos(angle * 3.14 / 180);
	int y1 = yAnchor + horizontalShift * sin((angle - 90) * 3.14 / 180) + verticalShift * sin(angle * 3.14 / 180);

	int x3 = x1 + height * cos(angle * 3.14 / 180) + width * cos((angle - 90) * 3.14 / 180);
	int y3 = y1 + height * sin(angle * 3.14 / 180) + width * sin((angle - 90) * 3.14 / 180);

	int angle1 = NormalizeAngle(angle);
	int angle2 = NormalizeAngle(angle - 90);

	int angle3 = NormalizeAngle(angle + 180);
	int angle4 = NormalizeAngle(angle + 90);
	        
	int longestSide = height;
	if (width > height) {
		longestSide = width;
	};

	for (int x = x1 - longestSide * 1.5; x < x1 + longestSide * 1.5; x++) {

		if (x < 0 || x >= frame.width) {
			continue;
		};

		for (int y = y1 - longestSide * 1.5; y < y1 + longestSide * 1.5; y++) {

			if (y < 0 || y >= frame.height) {
				continue;
			};

			int fill = 1;
			int pointAngle = NormalizeAngle(DeltasToDegrees(x - x1, y - y1));

			if (pointAngle < angle1 != pointAngle < angle2) {
				fill = -1;
			};

			if (NormalizeAngle(angle) >= 90) {
				fill *= -1;
			};

			if (fill == -1) {
				continue;
			};

			fill = 1;
			pointAngle = NormalizeAngle(DeltasToDegrees(x - x3, y - y3));

			if (pointAngle <= angle3 == pointAngle <= angle4) {
				fill = -1;
			};

			if (NormalizeAngle(angle3) < 90) {
				fill *= -1;
			};

			if (fill == -1) {
				continue;
			};

			frame.pixels[y * frame.width + x] = color;

		};

	};

	return;
};


void GetHitboxPosition(double hitbox[][2], double x, double y, double angle) {

	double width = hitbox[0][0];
	double height = hitbox[0][1];

	double xDeltaX = width / 2 * cos((angle + 90) * 3.14 / 180);
	double xDeltaY = height / 2 * cos(angle * 3.14 / 180);

	double yDeltaX = width / 2 * sin((angle + 90) * 3.14 / 180);
	double yDeltaY = height / 2 * sin(angle * 3.14 / 180);

	hitbox[1][0] = x - xDeltaY + xDeltaX;
	hitbox[1][1] = y - yDeltaY + yDeltaX;

	hitbox[2][0] = x + xDeltaY + xDeltaX;
	hitbox[2][1] = y + yDeltaY + yDeltaX;

	hitbox[3][0] = x + xDeltaY - xDeltaX;
	hitbox[3][1] = y + yDeltaY - yDeltaX;

	hitbox[4][0] = x - xDeltaY - xDeltaX;
	hitbox[4][1] = y - yDeltaY - yDeltaX;

	return;
};


void DrawArrow(int x1, int y1, double arrow_angle, int arrow_length, int arrow_width, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	DrawRectAFilled(x1, y1, -arrow_width / 2, 0, arrow_angle, arrow_width, arrow_length, color, arrow_length);
	DrawLineA(x1 + arrow_length * cos(arrow_angle * 3.14 / 180) + arrow_width * cos((arrow_angle + 90) * 3.14 / 180), y1 + arrow_length * sin(arrow_angle * 3.14 / 180) + arrow_width * sin((arrow_angle + 90) * 3.14 / 180), arrow_angle - 30, 2 * arrow_width, color);
	DrawLineA(x1 + arrow_length * cos(arrow_angle * 3.14 / 180) - arrow_width * cos((arrow_angle + 90) * 3.14 / 180), y1 + arrow_length * sin(arrow_angle * 3.14 / 180) - arrow_width * sin((arrow_angle + 90) * 3.14 / 180), arrow_angle + 30, 2 * arrow_width, color);
	return;
};


void DrawCircle(int x1, int y1, int radius, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	for (double x = x1 - radius; x < x1 + radius; x += 0.1) {

		/// (y-h)^2 + (x-h)^2 = r^2
		// y = sqrt( r^2 - (x-h)^2 ) + h

		if (x < 0 || x > frame.width) {
			continue;
		};

		int y = sqrt(radius * radius - (x - x1) * (x - x1)) + y1;	// top half

		if ((y > 0) && (y < frame.height)) {
			frame.pixels[y * frame.width + (int)(x)] = color;
		};

		y = y1 - sqrt(radius * radius - (x - x1) * (x - x1));	// flipped half

		if ((y <= 0) || (y >= frame.height)) {
			continue;
		};

		frame.pixels[(int)(y)*frame.width + (int)(x)] = color;

	};

	return;
};


void DrawCircleFilled(int x1, int y1, int radius, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	for (int x = x1 - radius; x < x1 + radius; x++) {

		if (x < 0 || x >= frame.width) {
			continue;
		};

		for (int y = y1 - radius; y < y1 + radius; y++) {

			if ((y < 0) || (y >= frame.height)) {
				continue;
			};

			// (y-h)^2 + (x-h)^2 = r^2
			if (sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1)) > radius) {
				continue;
			};

			frame.pixels[y * frame.width + x] = color;

		};

	};

	return;
}


static double stars[100];
static double starOffset = 10000;	// to mess them up; at the start the stars are too organized

void SetStars() {

	for (int i = 0; i < sizeof(stars) / sizeof(stars[0]); i++) {

		double rnd = rand();

		while (rnd > .7) {	// lower threshold -> less whizzing by stars, but more clustered together
			rnd /= 10;
		};

		stars[i] = rnd;
	};

	return;
};


void DrawStars(int x1, int y1) {

	int starCount = 0;

	for (int x = 0; x < frame.width; x += frame.width / 10) {

		for (int y = 0; y < frame.height; y += frame.height / 10) {

			if ((x + y) % 10 == 0) {
				continue;
			};

			double parallax = stars[starCount % 100];

			if (parallax > 1 || parallax <= 0) {
				parallax = 1;
			};

			int yNegative = 0;
			int xNegative = 0;

			if (y1 + y + starOffset < 0) {	// if negative axes, stop axis flipping (width*height - whateverX or Y)
				yNegative = 1;
			};

			if (x1 + x + starOffset < 0) {
				xNegative = 1;
			};

			int compensate = 0;

			if (yNegative && !xNegative) {
				compensate = 0;
			};

			if (xNegative && !yNegative) {
				compensate = 1;
			};

			if (xNegative && yNegative) {	//if it ain't broke, dont fix it.
				frame.pixels[((abs)(((int)((y1 + y + starOffset) * parallax)) * frame.width + ((int)((x1 + x + starOffset) * parallax)))) % (frame.width * frame.height)] = RGB(250, 170, 200);
				starCount++;
				continue;
			};

			if (!xNegative && !yNegative) {
				compensate = 1;
			};


			if ((y1 + y + starOffset < 0) && (x1 + x + starOffset < 0)) {
				compensate = 1;
			};


			frame.pixels[(abs)((frame.width * frame.height * compensate) - (abs)(((int)((y1 + y + starOffset) * parallax)) * frame.width + ((int)((x1 + x + starOffset) * parallax))) % (frame.width * frame.height))] = RGB(250, 170, 200);

			starCount++;
		};

	};

	return;
}


void AddForce(double forces[][2], double force, double direction) {

	int iEnd = 1;
	for (int i = 0; i < forceAmountCap; i++) {

		if (!forces[i][0]) {
			iEnd = i;
			break;
		};

	};



	forces[iEnd][0] = force;
	forces[iEnd][1] = direction;

}




void ClearForces(double forces[][2]) {

	for (int i = 0; i < forceAmountCap; i++) {

		forces[i][0] = 0;
		forces[i][1] = 0;

	};

	return;
}


void CalculateVelocity(double forces[][2], double velocity[2]) {

	for (int i = 0; i < 1; i++) {

		if (forces[i][0] == 0) {
			continue;
		};

		double force = forces[i][0];
		double forceDirection = forces[i][1];

		if (velocity[0] == 0) {
			velocity[0] = force;
			velocity[1] = forceDirection;
			continue;
		};

		velocity[0] += 1.0 * force * sin((velocity[1] - forceDirection) * 3.14 / 180 + 3.14 / 2);

		velocity[1] -= (100 * force / velocity[0]) * sin((velocity[1] - forceDirection) * 3.14 / 180);

	};

	if (velocity[0] < 0) {
		velocity[0] *= -1;
		velocity[1] += 180;
	};

	return;
}

// letters 32-126
int character_design[95][25] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//space
	{0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },	//!
	{0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//"
	{0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0 },	//#
	{0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0 },	//$
	{0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, },	//%
	{0, 0, 1, 0, 0,	0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, },	//&
	{0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//'
	{0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, },	//(
	{0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, },	//)
	{0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, },	//*
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, },	//+
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },	//,
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },	//-
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },	//.
	{0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, },	//slash (/)

	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 },	//0
	{0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 },	//1
	{0, 1, 1, 0, 0,	0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, },	//2
	{0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0 },	//3
	{0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, },	//4
	{0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, },	//5
	{ 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },	//6
	{0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, },	//7
	{0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },	//8
	{0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0 },	//9

	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },	//:
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },	//;
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },	//<
	{0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0 },	//=
	{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },	//>
	{0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },	//?
	{0, 1, 1, 1, 0,	1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0 },	//@

	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, },	//A
	{1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, },	//B
	{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, },	//C
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, },	//D
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, },	//E
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, },	//F
	{0, 1, 1, 1, 1,	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//G	
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, },	//H
	{1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, },	//I
	{1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//J
	{1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, },	//K
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, },	//L
	{1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, },	//M
	{1, 0, 0, 0, 1,	1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, },	//N
	{0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//O
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, },	//P
	{0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, },	//Q
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, },	//R
	{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, },	//S
	{1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, },	//T
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//U
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, },	//V
	{1, 0, 0, 0, 1,	1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, },	//W
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, },	//X
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, },	//Y
	{1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, },	//Z

	{0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0 },	//[
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, },	//backslash (\)
	{0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0 },	//]
	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//^
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 },	//_
	{0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	//`

	{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, },	//a
	{1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, },	//b
	{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, },	//c
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, },	//d
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, },	//e
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, },	//f
	{0, 1, 1, 1, 1,	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//g	
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, },	//h
	{1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, },	//i
	{1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//j
	{1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, },	//k
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, },	//l
	{1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, },	//m
	{1, 0, 0, 0, 1,	1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, },	//n
	{0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//o
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, },	//p
	{0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, },	//q
	{1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, },	//r
	{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, },	//s
	{1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, },	//t
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, },	//u
	{1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 },	//v
	{1, 0, 0, 0, 1,	1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0 },	//w
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1 },	//x
	{1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },	//y
	{1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1 },	//z

	{0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0 },	//{
	{0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },	//|
	{0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0 },	//}
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }	//~
};


void DrawString(int x1, int y1, char string[], int string_length, int color) {

	if (color < 0) {
		color = 0x00FFFFFF;
	};

	int font_size = 3;
	int font_spacing = 4;
	int character_id = 0;



	for (int letter = 0; letter < string_length; letter++) {

		character_id = (int)(string[letter]) - 32;

		int pixel = -1;
		for (int y = font_size * 5; y > 0; y -= font_size) {

			for (int x = 0; x < font_size * 5; x += font_size) {

				pixel++;

				if (!character_design[character_id][pixel]) {
					continue;
				};

				for (int innerX = 0; innerX < font_size; innerX++) {

					if ((x1 + letter * (5 * font_size + font_spacing) + x + innerX < 0) || (x1 + letter * (5 * font_size + font_spacing) + x + innerX > frame.width)) {
						continue;
					};

					for (int innerY = 0; innerY < font_size; innerY++) {


						if ((y1 + y * innerY < 0) || (y1 + y * innerY > frame.height)) {
							continue;
						};

						frame.pixels[(y1 + y + innerY) * frame.width + (x1 + letter * (5 * font_size + font_spacing) + x + innerX)] = color;

					};

				};

			};

		};



	};



	return;
};

void AppendString(char string[], int string_length, char string2[], int string2_length) {

	int iEnd = 0;
	for (int i = 0; i < string_length; i++) {

		if (!string[i]) {
			iEnd = i;
			break;
		};

	};

	for (int i = 0; i < string2_length; i++) {

		string[iEnd + i] = string2[i];

	};

	return;
}

void AppendInt(char string[], int string_length, int num) {

	int iEnd = 0;
	for (int i = 0; i < string_length; i++) {

		if (!string[i]) {
			iEnd = i;
			break;
		};

	};

	if (num == 0) {
		string[iEnd] = '0';
		return;
	};

	int num_length = 0;
	int tempNum = abs(num);

	while (tempNum > 0) {

		num_length++;
		tempNum /= 10;

	};

	if (num < 0) {
		string[iEnd] = '-';
		iEnd++;
	};

	num = abs(num);

	for (int i = num_length - 1; i >= 0; i--) {

		string[iEnd + i] = (char)(num % 10 + 48);

		num /= 10;
	};


	return;
}


void AppendDouble(char string[], int string_length, double num) {

	AppendInt(string, string_length, (int)(num));

	int iEnd = 0;
	for (int i = 0; i < string_length; i++) {

		if (!string[i]) {
			iEnd = i;
			break;
		};

	};

	string[iEnd] = '.';

	AppendInt(string, string_length, (int)(num * 1000) % 1000);

	return;
}


//double planets[100][4];
// [planet] [x, y, mass, radius] 

//---------------------------------------------


static BITMAPINFO frame_bitmapInfo;
static HBITMAP frame_hBitmap = 0;
static HDC frame_dc = 0;

LRESULT CALLBACK WindowProc(HWND hWNd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	const wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = { };
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;

	RegisterClass(&wc);

	frame_bitmapInfo.bmiHeader.biSize = sizeof(frame_bitmapInfo.bmiHeader);	//set bitmap information
	frame_bitmapInfo.bmiHeader.biPlanes = 1;
	frame_bitmapInfo.bmiHeader.biBitCount = 32;	// filler, r, g, b
	frame_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	frame_dc = CreateCompatibleDC(frame_dc);

	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX;

	RECT rect;
	rect.left = 500;
	rect.top = 250;
	rect.right = rect.left + 700;
	rect.bottom = rect.top + 500;

	HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"SpaceGame", style, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);


	player.x = 0;
	player.y = 0;
	player.dir = 90;
	player.mass = 20000.0;		//kg (20, 000kg)
	player.thrust = 1000.0;	//N	(15,000N)
	player.hitbox[0][0] = 40;	//hitbox width
	player.hitbox[0][1] = 70;	//hitbox height

	SetStars();

	int planets[100][5];
	//[planet] [xPosition, yPosition, mass, radius, color]

	

	for (int i = 0; i < sizeof(planets) / sizeof(planets[0]); i++) {


		planets[i][0] = rand() % 10000;

		if (rand() % 2) {
			planets[i][0] *= -1;
		};

		planets[i][1] = rand() % 10000;
		
		if (rand() % 2) {
			planets[i][1] *= -1;
		};

		planets[i][2] = rand() % 1000000000;

		planets[i][3] = planets[i][2] / 1000000;
		
		planets[i][4] = RGB(80, 80, rand() % 155 + 100);

	};


	MSG msg = { };

	while (running == 1) {

		PeekMessage(&msg, hWnd, 0u, 0u, PM_REMOVE);

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		for (int k = 0; k < 256; k++) {	// Get Keyboard Input
			keys[k] = GetKeyState(k) / 10;
		};

		if (keys[27]) {	//esc
			running = 0;
		};


		ClearForces(&player.forces);

		// Process Input

		if (keys[73]) {	// I
			zoomEffect += 0.01;
		};

		if (keys[79]) {	// O
			zoomEffect -= 0.01;
		};

		if (zoomEffect < 0.01) {
			zoomEffect = 0.01;
		};

		if (keys[81]) {	//Q
			player.dir += 2;
		};

		if (keys[69]) {	//E
			player.dir -= 2;
		};

		if (keys[87] && !keys[84]) {	//W
			AddForce(&player.forces, 1.0 * player.thrust / player.mass, player.dir);
		};

		if (keys[87] && keys[84]) {	//W && T
			AddForce(&player.forces, 3.0 * player.thrust / player.mass, player.dir);
		};

		if (keys[83]) {	//S
			AddForce(&player.forces, 0.5 * player.thrust / player.mass, player.dir + 180);
		};
		
		if (keys[65]) {	//A
			AddForce(&player.forces, 0.5 * player.thrust / player.mass, player.dir + 90);
		};

		if (keys[68]) {	//D
			AddForce(&player.forces, 0.5 * player.thrust / player.mass, player.dir - 90);
		};




		GetHitboxPosition(&player.hitbox, player.x, player.y, player.dir);

		// Planet gravities

		for (int p = 0; p < 100; p++) {


			if (!planets[p][2]) {	// if no mass
				continue;
			};

			int hitbox_corner_closest = 0;

			double deltaX = planets[p][0] - player.hitbox[0][0];
			double deltaY = planets[p][1] - player.hitbox[0][1];

			int force_distance = sqrt(deltaX * deltaX + deltaY * deltaY);

			for (int i = 1; i < 5; i++) {

				double deltaX = planets[p][0] - player.hitbox[i][0];
				double deltaY = planets[p][1] - player.hitbox[i][1];

				if (sqrt(deltaX * deltaX + deltaY * deltaY) < force_distance) {

					hitbox_corner_closest = i;
					force_distance = sqrt(deltaX * deltaX + deltaY * deltaY);

				};

			};

			deltaX = planets[p][0] - player.hitbox[hitbox_corner_closest][0];
			deltaY = planets[p][1] - player.hitbox[hitbox_corner_closest][1];

			double hitbox_corner_distance = sqrt(deltaX * deltaX + deltaY * deltaY);
			double gravity_force = (.0000000000667) * (player.mass) * (planets[p][2]) / (hitbox_corner_distance * hitbox_corner_distance);
			double gravity_direction = DeltasToDegrees(deltaX, deltaY);

			if (gravity_force < 0) {
				gravity_force *= -1;
				gravity_direction += 180;
			};

			if (hitbox_corner_distance < planets[p][3]) {

				player.velocity[0] = 0;

			};

			if (gravity_force < 1) {
				continue;
			};

			AddForce(&player.forces, gravity_force, gravity_direction);

		};



		// Calculate forces

		CalculateVelocity(&player.forces, &player.velocity);

		player.x += player.velocity[0] * cos(player.velocity[1] * 3.14 / 180);
		player.y += player.velocity[0] * sin(player.velocity[1] * 3.14 / 180);


		// Drawing

		for (int i = 0; i < frame.width * frame.height; i++) {	//bg color
			frame.pixels[i] = 0;
		};

		DrawStars(player.x, player.y);

		//	draw player
		
		if (keys[81]) {	//Q
			DrawRectAFilled(frame.width / 2, frame.height / 2, 15, 20, player.dir, 5, 5, 0x00DDDDDD);
			DrawRectAFilled(frame.width / 2, frame.height / 2, -20, -25, player.dir, 5, 5, 0x00DDDDDD);
		};

		if (keys[69]) {	//E
			DrawRectAFilled(frame.width / 2, frame.height / 2, -20, 20, player.dir, 5, 5, 0x00DDDDDD);
			DrawRectAFilled(frame.width / 2, frame.height / 2, 15, -25, player.dir, 5, 5, 0x00DDDDDD);
		};

		if (keys[65]) {	//A
			DrawRectAFilled(frame.width / 2, frame.height / 2, 15, 20, player.dir, 5, 5, 0x00DDDDDD);
			DrawRectAFilled(frame.width / 2, frame.height / 2, 15, -25, player.dir, 5, 5, 0x00DDDDDD);
		};

		if (keys[68]) {	//D
			DrawRectAFilled(frame.width / 2, frame.height / 2, -20, 20, player.dir, 5, 5, 0x00DDDDDD);
			DrawRectAFilled(frame.width / 2, frame.height / 2, -20, -25, player.dir, 5, 5, 0x00DDDDDD);
		};
		

		if (keys[87] && !keys[84]) {	//W && !T
			DrawRectAFilled(frame.width / 2, frame.height / 2, -10, -45, player.dir, 20, 15, 0x00FF0000);
			DrawRectAFilled(frame.width / 2, frame.height / 2, -5, -50, player.dir, 10, 20, 0x00FF2200);
		};

		if (keys[87] && keys[84]) {	//W && T
			DrawRectAFilled(frame.width / 2, frame.height / 2, -11, -46, player.dir, 22, 17, 0x004422FF);
			DrawRectAFilled(frame.width / 2, frame.height / 2, -5, -51, player.dir, 10, 22, 0x004444FF);
		};

		if (keys[83]) {	//S
			DrawRectAFilled(frame.width / 2, frame.height / 2, -12, 30, player.dir, 5, 5, 0x00DDDDDD);
			DrawRectAFilled(frame.width / 2, frame.height / 2, 7, 30, player.dir, 5, 5, 0x00DDDDDD);
		};

		DrawRectAFilled(frame.width/2, frame.height/2, -15, -30, player.dir, 30, 60, 0x00999999);


		
		for (int p = 0; p < sizeof(planets) / sizeof(planets[0]); p++) {	// draw Planets
			DrawCircleFilled((planets[p][0] - player.x) + frame.width / 2, (planets[p][1] - player.y) + frame.height / 2, planets[p][3], planets[p][4]);
		};


		if (1) {	// draw Forces
			for (int i = 0; i < forceAmountCap; i++) {

				if (player.forces[i][0] == 0) {
					continue;
				};

				DrawArrow(frame.width / 2, frame.height / 2, player.forces[i][1], player.forces[i][0] * 100, 8, 0x00FF0000);
			};
			
			DrawArrow(frame.width / 2, frame.height / 2, player.velocity[1], player.velocity[0], 10, 0x00FFFF00);
		};


		char velocityStr[100] = { 0 };	// display velocity
		AppendString(&velocityStr, sizeof(velocityStr) / sizeof(velocityStr[0]), "V: ", 3);
		AppendDouble(&velocityStr, sizeof(velocityStr) / sizeof(velocityStr[0]), player.velocity[0]);
		AppendString(&velocityStr, sizeof(velocityStr) / sizeof(velocityStr[0]), "m/s", 3);
		DrawString(25, 25, velocityStr, 100, -1);

		char positionStr[100] = { 0 };	// display position
		AppendString(&positionStr, sizeof(positionStr) / sizeof(positionStr[0]), "Pos: (", 6);
		AppendInt(&positionStr, sizeof(positionStr) / sizeof(positionStr[0]), (int)(player.x));
		AppendString(&positionStr, sizeof(positionStr) / sizeof(positionStr[0]), ", ", 2);
		AppendInt(&positionStr, sizeof(positionStr) / sizeof(positionStr[0]), (int)(player.y));
		AppendString(&positionStr, sizeof(positionStr) / sizeof(positionStr[0]), ")", 1);
		DrawString(25, 50, positionStr, 100, -1);


		
		InvalidateRect(hWnd, NULL, 0);
		UpdateWindow(hWnd);

		Sleep(10);
	};

	return 0;
};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {

	case (WM_CLOSE): {
		DestroyWindow(hWnd);
		break;
	}

	case (WM_DESTROY): {
		PostQuitMessage(0);
		running = 0;
		break;
	}

	case (WM_SIZE): {

		frame_bitmapInfo.bmiHeader.biWidth = LOWORD(lParam);	//finish setting up bitmap info (size); in WM_SIZE bc handles resizing as well
		frame_bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

		if (frame_hBitmap) {
			DeleteObject(frame_hBitmap);
		};
		// set frame.pixels to an array
		frame_hBitmap = CreateDIBSection(NULL, &frame_bitmapInfo, DIB_RGB_COLORS, &frame.pixels, 0, 0);

		SelectObject(frame_dc, frame_hBitmap);

		frame.width = LOWORD(lParam);
		frame.height = HIWORD(lParam);

		break;
	}

	case (WM_PAINT): {

		static PAINTSTRUCT ps;
		static HDC hdc;
		hdc = BeginPaint(hWnd, &ps);

		BitBlt(hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			(ps.rcPaint.right - ps.rcPaint.left),
			(ps.rcPaint.bottom - ps.rcPaint.top),
			frame_dc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			SRCCOPY);

		EndPaint(hWnd, &ps);

		break;
	};


	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	};

	return 0;
};
