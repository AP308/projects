#include <iostream>
#include <vector>
#include <string>

class Data {

public:

	//7 gear types
	int itemNum = 12;
	std::string armorName[12 + 1] = { " nothing equipped yet ",
		"Basic Helm", "Basic Chestplate", "Ragged Pants", "Old Boots", "Stick", "Rock",
		"Helm of Gorgon", "Chest of Gorgon", "Pants of Gorgon", "Boots of Gorgon", "Sword of Gorgon", "Gorgon's Amulet of Defense"
	};
	int armorStat[12 + 1][5] = {
		{0, 0, 0, 0, 0},

		{1, 0, 0, 5, 0},
		{2, 0, 15, 3, 0},
		{3, 0, 5, 5, 0},
		{4, 0, 0, 3, 2},
		{5, 10, 0, 0, 5},
		{6, 2, 0, 0, 3},

		{1, 0, 100, 32, 0},
		{2, 0, 150, 50, 0},
		{3, 0, 75, 12, 0},
		{4, 0, 25, 1, 3},
		{5, 32, 0, 0, 25},
		{6, 13, 25, 0, 0}
	};

	std::string displayArmorStats(int id) {
		std::string statNames[5] = { "x", "atk", "hp", "def", "crit" };
		std::string final = "[" + armorName[id] + ": ";
		for (int i = 1; i <= 4; i++) {
			if (armorStat[id][i] == 0) {
				continue;
			};
			final += std::to_string(armorStat[id][i]) + " " + statNames[i]; // !! you can't print a 2d array w/o turning it into a string first
			for (int f = i + 1; f <= 4; f++) {
				if (armorStat[id][f] == 0) {
					continue;
				};
				final += ", ";
				break;
			};
		};
		return (final += "]");
	};

};

class Player {

private:
	std::string playerData = "";
	std::string name = "";
	int atk = 0;
	int hp = 0;
	int def = 0;
	int crit = 0;
	int equippedGear[7] = { 0, 0, 0, 0, 0, 0, 0 };
	std::vector<int> inventory = {};
	
	int strToInt(std::string str) {
		int num = 0;
		for (int l = 0; l < str.size(); l++) {
			num += ((int)(str[l]) - 48) * pow(10, (str.size() - l - 1));
		};
		return num;
	};
		


	std::string shorten(std::string text, int length) {
		std::string msg = "";

		if ((text.size()) > length) {
			for (int i = 0; i <= length - 1; i++) {
				msg += text[i];
			};
			return msg;
		};

		msg = text;
		for (int i = 0; i < (length - text.size()); i++) {
			msg += " ";
		};
		return msg;
	};

	void reorganizeInventory() {
		Data data;
		int tempIndex;
		for (int n = 0; n < inventory.size(); n++) {
			for (int i = 0; i < inventory.size()-1; i++) {
				if (data.armorStat[inventory[i]][1] + data.armorStat[inventory[i]][2] + data.armorStat[inventory[i]][3] + data.armorStat[inventory[i]][4] <
					data.armorStat[inventory[i + 1]][1] + data.armorStat[inventory[i + 1]][2] + data.armorStat[inventory[i + 1]][3] + data.armorStat[inventory[i + 1]][4]) {
					//switch indexes
					tempIndex = inventory[i];
					inventory[i] = inventory[i + 1];
					inventory[i + 1] = tempIndex;
				};
			};
		};

	};


public:
	int playerPos = 0;
	//int playerZone = 0;

	Player(std::string d) {
		playerData = d;
		updatePlayerStats();
	};

	void updatePlayerStats() {
		Data data;
		std::string dataChunk = "";
		int dataNum = 0;
		inventory.clear();
		for (int i = 0; i <= playerData.size() - 1; i++) {
			if (playerData[i] == ' ') {
				continue;
			};
			if (playerData[i] == '/') {
				// set vars to corresponding dataChunk
				if (dataNum == 18) {
					playerPos = strToInt(dataChunk);
				};
				if ((dataNum >= 8) && (dataNum <= 17)) {
					inventory.push_back(strToInt(dataChunk));
				};
				if ((dataNum >= 1) && (dataNum <= 7)) {
					equippedGear[dataNum - 1] = strToInt(dataChunk);
				};
				if (dataNum == 0) {
					name += dataChunk;
				};
			};
			if (playerData[i] == '/') {
				dataChunk = "";
				dataNum++;
				continue;
			};
			dataChunk += playerData[i];
		};
		atk = hp = def = crit = 0;
		for (int i = 0; i <= 6; i++) {
			atk += data.armorStat[equippedGear[i]][1];
			hp += data.armorStat[equippedGear[i]][2];
			def += data.armorStat[equippedGear[i]][3];
			crit += data.armorStat[equippedGear[i]][4];
		};
	};

	void editPlayerData(int pos, int val) {
		int dataNum = 0;
		std::string newData = "";
		bool found = false;
		for (int i = 0; i <= playerData.size(); i++) {
			if (pos == 0) {
				break;
			};
			if (dataNum == pos) {
				newData = playerData.substr(0, i-1) + std::to_string(val); // beginning of string + val itself
				found = true;
			};
			if ((found == true) && (playerData[i] == '/')) {
				
				newData +=  playerData.substr(i, playerData.size()-1); // add the rest of the string
				playerData = newData;
				break;
			};
			if (playerData[i] == '/') {
				dataNum++;
			};
		};
	};

	void displayPlayerData() {
		std::cout << "----------------------------------\n";
		std::cout << "PlayerData: " << playerData << '\n';
	};


	void displayGear() {
		std::cout << "----------------------------------\n" << name << '\n';
		Data data;
		std::cout << "  [] \t" << data.displayArmorStats(equippedGear[0]);
		std::cout << "\n |[]|\t" << data.displayArmorStats(equippedGear[1]);
		std::cout << "\n  || \t" << data.displayArmorStats(equippedGear[2]);
		std::cout << "\n  ** \t" << data.displayArmorStats(equippedGear[3]);
		std::cout << "\n\n <==[=   " << data.displayArmorStats(equippedGear[4]);
		std::cout << "\n\n  o{     " << data.displayArmorStats(equippedGear[5]);
		std::cout << "\n  o{     " << data.displayArmorStats(equippedGear[6]);
		std::cout << "\n\n " << atk << " atk, " << hp << " hp,  " << def << " def, " << crit << " crit";
		std::cout << "\n ----------------------------------\n";
		//delete data; how to delete instance of an object?? ptrs dont work
	};

	void displayInventory() {
		reorganizeInventory();
		Data data;
		std::cout << "Inventory:" << "\n";
		int size = inventory.size();
		for (int i = 0; i < size; i++) {
			std::cout << " (" + std::to_string(i + 1) + ")" + data.displayArmorStats(inventory[i]) << '\n';
		};
		//std::cout << "-Top 10 are kept, others are sold automatically-\n";
		std::cout << "----------------------------------\n";
	};

	void equip(int item) {
		Data data;
		int tempItem;
		item--;
		int type = data.armorStat[inventory[item]][0];
		type--;

		tempItem = equippedGear[type];
		//equippedGear[type] = inventory[item];
		editPlayerData(type + 1, inventory[item]);
		//inventory[item] = tempItem;
		editPlayerData(8 + item, tempItem);

		displayPlayerData();

		updatePlayerStats();
	};

	//void sell

};


class Map {

	private:
		int sizeX = 20;
		int sizeY = 20;
		int map[20][20];
		int timesEdited = 0;


		int tileAdjacent(int x, int y, char direction) {
			// north, east, south, west


			if ((direction == 'a') && (x == 0)) { // check if valid
				return 0;
			};
			if ((direction == 'd') && (x == sizeX - 1)) {
				return 0;
			};
			if ((direction == 'w') && (y == 0)) {
				return 0;
			};
			if ((direction == 's') && (x == sizeY - 1)) {
				return 0;
			};

			switch (direction) { // return adjacent
			case 'a':
				return map[x - 1][y];
				break;
			case 'd':
				return map[x + 1][y];
				break;
			case 'w':
				return map[x][y - 1];
				break;
			case 's':
				return map[x][y + 1];
				break;
			default:
				return 0;
			};


		}

		void editTileAdjacent(int x, int y, char direction, int tile) {

			if (tileAdjacent(x, y, direction) == 0) {
				return;
			};


			switch (direction) {
			case 'a':
				map[x - 1][y] = tile;
				break;
			case 'd':
				map[x + 1][y] = tile;
				break;
			case 'w':
				map[x][y - 1] = tile;
				break;
			case 's':
				map[x][y + 1] = tile;
				break;
			};

			timesEdited++;
		};

		int numOfAdjacentTiles(int x, int y) {
			int num = 0;
			if (tileAdjacent(x, y, 'w') == 2) {
				num++;
			};
			if (tileAdjacent(x, y, 's') == 2) {
				num++;
			};
			if (tileAdjacent(x, y, 'a') == 2) {
				num++;
			};
			if (tileAdjacent(x, y, 'd') == 2) {
				num++;
			};
			return num;
		};

		bool chance(int chance) {

			if (rand() % chance == 0)


			return false;
		};

		// 0 - invalid // 1 - grass // 2 - water

	public:

		void generateMap() {

			int unit = 0;

			for (int y = 0; y < sizeY; y++) {
				for (int x = 0; x < sizeX; x++) {

					map[x][y] = rand() % 2 + 1;
					if (rand() % 2 == 0) {
						map[x][y] = 1;
						continue;
					};

				};
			};


			for (int n = 0; n < 2; n++) {
				for (int y = 0; y < sizeY; y++) {
					for (int x = 0; x < sizeX; x++) {

						unit = rand() % 2 + 1;

						editTileAdjacent(x, y, 'a', unit);
						editTileAdjacent(x, y, 'd', unit);
						editTileAdjacent(x, y, 'w', unit);
						editTileAdjacent(x, y, 's', unit);


						//unit = rand

						/*
						if (tileAdjacent(x, y, 'a') != 0) {
							editTileAdjacent(x, y, 'd', 2);
						};
						if (tileAdjacent(x, y, 'd') == 2) {
							editTileAdjacent(x, y, 'a', 2);
						};
						if (tileAdjacent(x, y, 'w') == 2) {
							editTileAdjacent(x, y, 's', 2);
						};
						if (tileAdjacent(x, y, 's') == 2) {
							editTileAdjacent(x, y, 'w', 2);
						};*/

					};
				};
			};

			for (int y = 0; y < sizeY; y++) {
				for (int x = 0; x < sizeX; x++) {

					if ( (numOfAdjacentTiles(x, y) == 1) && (chance(2)) ) { // if only 1 neighbor, 1/2 chance to delete
						map[x][y] = 1;
					};


					if (numOfAdjacentTiles(x, y) == 0) { // if no neighbors, delete
						map[x][y] = 1;
					};

				};
			};

			std::cout << "timesEdited: " << timesEdited << '\n';
		
		};


		void displayMap() {

			for (int y = 0; y < sizeY; y++) {
				for (int x = 0; x < sizeX; x++) {

					//if (x >= )

					switch (map[x][y]) {
						case 1:
							std::cout << '.';
							break;
						case 2:
							std::cout << 'w';
							break;
						default:
							std::cout << '?';
							break;
					};
					std::cout << ' ';


				};
				std::cout << "\n";
			};

		};
	


};




int main() {



	//          name / 1-7 gear / 8-? inventory
	// *after every data segment put a '/'
	
	Player plr("Player/ 1/2/3/4/5/6/6/ 3/4/5/6/8/3/1/9/0/0/ 32/");


	plr.displayPlayerData();

	/*
	plr.displayPlayerData();


	plr.displayGear();
	plr.displayInventory();

	plr.equip(1);

	plr.displayGear();
	plr.displayInventory();
	*/

	Map map;

	std::cout << "player pos: " << plr.playerPos << "\n\n";

	map.generateMap();

	map.displayMap();




	return 0;
}