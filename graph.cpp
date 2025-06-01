
#include <iostream>
#include <vector>
#include <string>




double money(double data) {
	double cash = 0;
	int length = 0;
	while ((long long int)data / pow(10, length) > 1) {
		length++;
	};
	cash = (long long int)(data) % (long long int)pow(10, length - 9);
	cash += ((long long int)(data*100) % 100)/100.0;
	if (std::to_string(data)[8] == '0') {
		cash *= -1;
	};
	return cash;
};

std::string dateString(double data) {
	std::string dString = std::to_string(data);
	std::swap(dString[0], dString[1]);
	dString = dString.substr(0, 2) + '/' + dString.substr(2, 2) + '/' + dString.substr(4, 4);
	return dString;
};

double dateValue(double data) {
	//year*100 + month*10 + day
	std::string stringData = std::to_string(data);
	std::swap(stringData[0], stringData[1]);
	double dValue = stoi(stringData.substr(4, 4)) + stoi(stringData.substr(0, 2))*(100.00/12) + stoi(stringData.substr(2, 2))*(10.00/31);
	return dValue;
}



int pointToNumber(int point0, int point1, int width) {

	int number = width * (point1 - 1) + point0;

	return number;
};

int numberToPoint(int number, int width, std::string xOrY) {

	int x = (number % width);
	if (x == 0) {
		x = width;
	};
	int y = (-1 * x / 4) + (number / 4) + 1;

	if (xOrY == "x") {
		return x;
	};
	return y;
}

bool checkCell(int x, int y, std::vector <int> points, int width) {

	for (int cell : points) {
		if (pointToNumber(x, y, width) == cell) {
			return true;
		}
	};
	return false;
};

std::vector <double> combineSameDates(std::vector <double> cashData) {

	for (int i = 1; i <= cashData.size() - 1; i++) {
		for (int n = 1; n <= cashData.size() - 1; n++) {

			if (dateValue(cashData[i]) == dateValue(cashData[n]) && (i != n)) {
				std::string date = std::to_string(cashData[i]).substr(0, 8);
				std::string totalMoney = std::to_string((money(cashData[i]) * 100.0) + (money(cashData[n]) * 100.0));
				double finalData = 0.0;

				if (totalMoney[0] == '-') {
					totalMoney = totalMoney.substr(1, totalMoney.length() - 1);
					finalData = stod(date + "0" + totalMoney) / 100;
				}
				else {
					finalData = stod(date + "1" + totalMoney) / 100;
				};
				std::cout << std::fixed << finalData << '\n';
				cashData[i] = finalData;

				for (int x = n; x <= cashData.size() - 2; x++) {
					cashData[x] = cashData[x + 1];
				};
				cashData.erase(cashData.begin() + cashData.size() - 1);
			};
		};
	};
	return cashData;
}

std::vector <double> organize(std::vector <double> data) {
	for (int i = 1; i <= (data.size() - 1) * (data.size() - 1); i++) {
		int loopNum = ((i - 1) % (data.size() - 2) + 1);
		if (dateValue(data[loopNum]) > dateValue(data[loopNum + 1])) {
			std::swap(data[loopNum], data[loopNum + 1]);
		};
	};
	return data;
};


std::vector <int> border(std::vector <int> points, int width, int height) {
	
	for (int i = 1; i <= width; i++) {
		points.push_back(i);
		points.push_back(width * (height - 1) + i);
	};

	for (int i = 1; i <= height; i++) {
		points.push_back(width * (i - 1) + 1);
		points.push_back(width * i);
	};

	return points;
}


double findHighestCash(std::vector <double> cashData) {
	double cash = cashData[0];
	double highestCash = cash;
	for (int i = 1; i <= cashData.size() - 1; i++) {
		cash += money(cashData[i]);

		if (cash > highestCash) {
			highestCash = cash;
		};
	};
	std::cout << "highest:  " << highestCash << '\n';
	return highestCash;
}


std::vector <int> createLine(std::vector <int> points, int width, int x1, int y1, int x2, int y2) {


	double slope = (y2 - y1) / (x2 - x1);

	std::cout << "slope:  " <<  slope << '\n';

	//int i = 1;

	for (int i = 1; i <= (x2 - x1); i++) {
		points.push_back( pointToNumber(x1 + i, y1 + round (slope * i), width) );//y1 + (slope * i), width));
		//std::cout << "e:  " << x1 + i << "  " << y1 + round(slope * i) << '\n';//y1 + (slope * i) << '\n';
	};


	return points;
};




std::vector <int> scaleAllPoints(std::vector <int> points, std::vector <double> cashData, int width, int height) {

	int startYear = 2023;

	double highestDate = dateValue(cashData[cashData.size() - 1]) - startYear;
	double highestCash = findHighestCash(cashData);

	double scaleX = (double)width / highestDate;
	double scaleY = (double)height / highestCash;

	for (int f = 0; f <= points.size()-1; f++) {

		double xCor = numberToPoint(points[f], width, "x");
		//xCor *= scaleX;
		double yCor = scaleY * numberToPoint(points[f], width, "y");
		//yCor = 1;

		points[f] = pointToNumber(xCor, yCor, width);
		std::cout << "point " << f << " :  " << points[f] << '\n';

	};

	return points;
};


double findMaxCash(std::vector <double> cashData) {

	double cash = cashData[0];
	double highestCash = cash;

	for (int i = 1; i <= cashData.size() - 1; i++) {

		cash += money(cashData[i]);

		if (cash > highestCash) {
			highestCash = cash;
		};

	};

	return highestCash;
};

int main() {

	// SYNTAX: month(flipped) - day - year - 1/0(gain/loss) - amount(double)
	std::vector <double> cashData = {212.50,
								60222024120.00, 60252024010.00, 60262024050.00, 60272024150.00, 60292024120.00,  70072024140.00,
								70062024120.00, 7007202402.61, 70082024120.00, 7011202402.99, 70132024120.00, 70142024040.00,
								70162024030.00, 70182024130.00, 70182024110.00, 70062024040.00, 60152024120.00, 6010202415.00 };

	double cash = cashData[0];

	cashData = combineSameDates(cashData);
	cashData = organize(cashData);
		
	
	
	cash = cashData[0];

	int width = 100;
	int height = 50;

	std::vector <int> points;



	for (int i = 1; i <= cashData.size() - 1; i++) {

		cash += money(cashData[i]);
		
		points.push_back(pointToNumber( dateValue(cashData[i]), 1, width ));


		//points.push_back

	};


	//double maxCash = findMaxCash(cashData);
	//double maxDate = dateValue(cashData[cashData.size() - 1]);

	//std::cout << "max cash/date:  " << maxCash << " / " << maxDate << '\n';

	points = scaleAllPoints(points, cashData, width, height);


	/*double x1;
	double y1;
	double x2;
	double y2;
	int initialSize = points.size() - 2;
	//width += 2;
	for (int i = 1; i <= initialSize; i++) {



		x2 = (points[i + 1] % width);
		if (x2 == 0) {
			x2 = width;
		};
		y2 = (-1 * x2 / 4) + (points[i + 1] / 4) + 1;

		//points = createLine(points, width, x1, y1, x2, y2);
	};
	//width -= 2;*/
	


	//points = border(points, width, height);

	std::cout << "P1" << '\n';
	// +2 for border
	std::cout << width << " " << height << '\n';

	for (int y = height; y <= height; y++) {

		for (int x = 1; x <= width; x++) {

			if (checkCell(x, y, points, width + 2) ) {
				std::cout << "1 ";
			}
			else {
				std::cout << "0 ";
			};

		};

		std::cout << '\n';

	};


	
	return 0;
};