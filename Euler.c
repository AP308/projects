
#include <stdio.h>

double formula(double x, double y) {

	return x + (2 * y) - 1;

};

int main() {

	double deltaX = 1;

	double xDesired = 1;

	double xStart = -1;
	double y = -12;

	double dydx = formula(xStart, y);


	printf("x  \t  y\t    dydx\n");
	printf("%.3f     %.3f     %.3f\n", xStart, y, dydx);

	for (double x = xStart + deltaX; x <= xDesired; x += deltaX) {


	
		y += deltaX * dydx;

		dydx = formula(x, y);


		printf("%.3f     %.3f     %.3f\n", x, y, dydx);

	};

	//printf("%.3f     %.3f     %.3f\n", xDesired, y, dydx);

	
	printf("Answer: %f\n", y);



	return 0;
};