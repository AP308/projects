
#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <thread>
#include <chrono>


int pos_to_id(int x, int y) {
	int id = 50 * (y - 1) + x;
	return id;
};

/*int id_to_pos(int id) {


	return
};*/

std::vector <int> line(int posX, int posY, int length, std::string direction, std::vector <int> arr) {



	arr.push_back(pos_to_id(posX, posY));

	if (direction == "right") {
		for (int f = 1; f <= length; f++) {
			arr.push_back(pos_to_id(posX + f, posY));
		};
	};

	if (direction == "left") {
		for (int f = 1; f <= length; f++) {
			arr.push_back(pos_to_id(posX - f, posY));
		};
	};

	if (direction == "up") {
		for (int f = 1; f <= length; f++) {
			arr.push_back(pos_to_id(posX, posY - f));
		};
	};

	if (direction == "down") {
		for (int f = 1; f <= length; f++) {
			arr.push_back(pos_to_id(posX, posY + f));
		};
	};

	return arr;
};


int map(int id) {

	int width = 50;
	int height = 25;


	std::vector <int> mapData = { pos_to_id(38, 5) };

	mapData = line(32, 18, 5, "up", mapData);
	mapData = line(32, 18, 12, "left", mapData);



	mapData = line(4, 4, 6, "down", mapData);
	mapData = line(4, 11, 9, "right", mapData);
	mapData = line(14, 11, 3, "up", mapData);

	mapData = line(14, 8, 8, "right", mapData);
	mapData = line(22, 8, 12, "down", mapData);

	mapData = line(44, 23, 6, "left", mapData);
	mapData = line(44, 22, 8, "up", mapData);

	mapData = line(44, 14, 7, "left", mapData);


	mapData = line(44, 4, 4, "down", mapData);
	mapData = line(44, 8, 12, "left", mapData);
	mapData = line(32, 8, 5, "up", mapData);
	mapData = line(43, 4, 2, "left", mapData);


	if (id == mapData[0]) {
		return 2;
	};


	for (int wall : mapData) {
		if (id == wall) {
			return 1;
		};
	};


	for (int n = 1; n <= width; n++) {
		if ((id == n) || (id == width * (height - 1) + n)) {
			return 1;
		};
	};

	for (int n = 1; n <= height; n++) {
		if ((id == width * (n - 1) + 1) || (id == (width * n))) {
			return 1;
		};
	};


	return 0;
};


int update(static int pos[]) {
	int mapWidth = 50;
	int mapHeight = 25;

	for (int i = 1; i <= mapWidth * mapHeight; i++) {

		if (ceil(double(i - 1) / mapWidth) == floor(double(i - 1) / mapWidth)) {
			std::cout << '\n';
		};

		if (pos_to_id(pos[0], pos[1]) == i) {
			std::cout << "O";
			continue;
		};

		if (map(i) == 2) {
			std::cout << "!";
			continue;
		};

		if (map(i) == 1) {
			std::cout << "X";
			continue;
		};

		std::cout << " ";
		continue;
	};
	std::cout << '\n';

	return 0;
};


int move(int pos[], char input, int arrayPos) {

	//static int arr[2] = {2, 2};

	if ((input == 'w' || input == 's') && arrayPos == 0) {
		return pos[0];
	};
	if ((input == 'a' || input == 'd') && arrayPos == 1) {
		return pos[1];
	};

	//std::string translate[8] = { "w", "s", "a", "d", }

	if (input == 'w') {
		return (pos[1] - 1);
	};
	if (input == 's') {
		return (pos[1] + 1);
	};
	if (input == 'a') {
		return (pos[0] - 1);
	};
	if (input == 'd') {
		return (pos[0] + 1);
	};


	return 0;
};
//42 21 (goal)



std::vector <char> modify(static std::vector <char> arr) {
	//srand(seed*rand());
	char keys[4] = { 'w', 'a', 's', 'd' };
	int size = arr.size();//sizeof(arr)/sizeof(arr[0]);
	//srand(time(0));

	int choice = (rand() % 4) + 1;
	//choice = 1;


	if (choice == 2 && size > 3) {
		arr[(rand() % (size - 2))] = keys[rand() % 4];
		return (arr);
	};

	if (choice == 3 && size > 3) {


		int randElement = (rand() % (size - 1)) + 1;
		arr.erase(arr.begin() + (randElement - 1));

		for (int i = randElement; i <= size - 3; i++) {
			arr[i] = arr[i + 1];
		};
		arr.pop_back();

		return (arr);
	};

	if (choice == 4) {
		arr.clear();
		for (int i = 0; i <= (rand() % 30); i++) {
			arr.push_back(keys[rand() % 4]);
		};
		return (arr);
	};


	arr.push_back(keys[rand() % 4]);
	return (arr);



	return (arr);
};




int main() {
	srand(time(0));
	std::string input;
	int startingPos[2] = {13, 20};
	int pos[2];
	pos[0] = startingPos[0];
	pos[1] = startingPos[1];
	update(pos);
	double score = 0;
	double bestScore = 0;
	int moves = 0;
	double distance = sqrt(pow((38 - pos[0]), 2) + pow((5 - pos[1]), 2));
	std::vector <char> bestMoves = { };
	for (int i = 0; i <= 5; i++) {
		bestMoves = modify(bestMoves);
	};
	std::vector <char> newMoves;
	std::string roundInput;
	int trainingLength = 0;
	int timesModded;
	double previousDistance = 12345.12345;
	int bestIteration;
	std::vector <int> usedPositions;


	while (true) {


		std::cout << "Play best (p) or train (t#### (4 digits) )" << '\n';
		std::cout << "  ?:  ";
		std::cin >> roundInput;

		if (roundInput == "p") {
			pos[0] = startingPos[0];
			pos[1] = startingPos[1];
			for (char input : bestMoves) {
				system("cls");
				if (map(pos_to_id(move(pos, input, 0), move(pos, input, 1))) == 1) {
					//score = 0;
					continue;
				};
				pos[0] = move(pos, input, 0);
				pos[1] = move(pos, input, 1);
				update(pos);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			};
			continue;
		};
		if (roundInput[0] == 't') {//&& roundInput[1] != 'b') {

			trainingLength = 1000 * ( ((int(roundInput[1]) - 48) * 1000) + ((int(roundInput[2]) - 48) * 100) + ((int(roundInput[3]) - 48) * 10) + (int(roundInput[4]) - 48) );
		};
		/*if (roundInput[0] == 't' && roundInput[1] == 'b') {

		};*/

		//length = 0;

		auto timeA = std::chrono::high_resolution_clock::now();

		bestIteration = 0;
		for (int x = 0; x <= trainingLength; x++) {



			pos[0] = startingPos[0];
			pos[1] = startingPos[1];
			score = 0;
			distance = 0;
			newMoves = bestMoves;
			moves = 0;
			//newMoves = modify(newMoves);
			for (int l = 0; l <= ( (x - bestIteration)/1000 ) + 3 ; l++) {
				newMoves = modify(newMoves);
				timesModded = l;
			};


			//newMoves = modify(bestMoves);
			for (char input : newMoves) {
				moves++;

				if (move(pos, input, 0) == 0) {
					std::cout << "badInp";
					continue;
				};

				if (map(pos_to_id(move(pos, input, 0), move(pos, input, 1))) == 1) {
					//score = 0;
					continue;
				};

				pos[0] = move(pos, input, 0);
				pos[1] = move(pos, input, 1);

				if ((map(pos_to_id(pos[0], pos[1])) == 2)) {
					score += 1;
					break;
				};
			};

			distance = sqrt(pow((38 - pos[0]), 2) + pow((5 - pos[1]), 2));
			if (distance == previousDistance) {
				score -= 0.25;
			};
			if (distance != 0 && distance != previousDistance) {
				score += (1 / distance);
			};
			//previousDistance = distance;

			if ( (x - bestIteration) > 1000 )  {
				bestScore = 0;
				//usedPositions.push_back
			};


		
			for (int position : usedPositions) {
				if ( (position == pos_to_id(pos[0], pos[1])) && (map(pos_to_id(pos[0], pos[1])) != 2) ) {
					score -= 0.5;
				};
			};
			usedPositions.push_back( pos_to_id( pos[0], pos[1]) );

			score += (1 / (double(moves)*1000));

			if (score > bestScore) {
				//system("cls");
				//update(pos);

				bestMoves = newMoves;
				bestScore = score;

				auto timeB = std::chrono::high_resolution_clock::now();
				auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(timeB - timeA);

				double timeRemainingMin = round(((double(timeTaken.count()) / x) * trainingLength / 60000) * 100.00) / 100.00;

				double timeTakenMin = round((double(timeTaken.count()) / 60000) * 100.00) / 100.00;



				std::cout << "[" << x  << " / " << double(trainingLength) / 1000000 << " mil] (" << (double(x) / trainingLength) * 100 << "%) (" << timeTakenMin << " min / " << timeRemainingMin << " min left)"
					<< "] AI completed w/ score of  " << score << " / " << bestScore
					<< "  and was at a distance of " << distance <<  " (" << pos[0] << " , " << pos[1] << ").";
				std::cout << "It did this with  " << moves << "  moves. (Modified  " << (x - bestIteration)/1000 + 3  << "  ) times." << '\n';

				std::cout << "{";
				for (char currentMove : newMoves) {
					std::cout << "'" << currentMove << "', ";
				};
				std::cout << "}";
				std::cout << '\n';
				std::cout << '\n';

				//std::this_thread::sleep_for(std::chrono::milliseconds(200));
				previousDistance = distance;
				bestIteration = x;
			};


			/*std::cout << "[" << x << "]  -  " << distance << "{";
			for (char currentMove : newMoves) {
				std::cout << "'" << currentMove << "', ";
			};
			std::cout << "}";
			std::cout << '\n';*/

			//system("cls");
			//update(pos);


		};
		auto timeB = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(timeB - timeA);

		std::cout << "Time taken:  " << (double(timeTaken.count())/1000) << "  seconds" << '\n';

	};
	return 0;
};