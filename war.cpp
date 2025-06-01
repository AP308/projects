
#include <iostream>
#include <string>
#include <vector>


int cardValue(std::string card) {

	//card = card[0];
	
	//std::cout << isdigit(card[0]);
	int cardValue = (card[0]);

	switch (int(card[0])) {
		case 65:
			cardValue = 14;
			break;
		case 75:
			cardValue = 13;
			break;
		case 81:
			cardValue = 12;
			break;
		case 74:
			cardValue = 11;
			break;
		default:
			cardValue = int(card[0]) - 48;
	};

	if ((card[0] == '1') && (card[1] == '0')) {
		cardValue = 10;
	};


	return cardValue;
};


std::vector <std::string> addOrRemove(static std::vector<std::string> deck, std::string effect, std::string card1, std::string card2) {


	if (effect == "add") {
		for (int f = 0; f <= (deck.size() - 2); f++) {
			deck[f] = deck[f + 1];
		};
		deck[deck.size()-1] = card1;
		deck.push_back(card2);
	};

	if (effect == "remove") {
		for (int f = 0; f <= (deck.size() - 2); f++) {
			deck[f] = deck[f + 1];
		};
		//deck.push_back("e");
		deck.erase(deck.begin() + deck.size() - 1);
	};

	return deck;
};


int main() {
	//hearts, spades, clubs, diamonds, % ^ * +
	std::string deckSymbols[] = { "x", "^", "%", "*", "+", "J", "Q", "K", "A"};
	std::string deckData[57] = {};
	for (int n = 1; n <= 4; n++) {
		for (int i = 1; i <= 14; i++) {
			if (i <= 10) {
				deckData[14 * (n - 1) + i] = std::to_string(i) + deckSymbols[n];
			};
			if (i >= 11) {
				deckData[14 * (n - 1) + i] = deckSymbols[i - 6] + deckSymbols[n];
			};
		};
	};

	srand(time(0));
	int random;
	int random2;
	std::string card;
	for (int i = 0; i <= 52; i++) {
		random = rand() % 52;
		random2 = rand() % 52;
		card = deckData[random];
		deckData[random] = deckData[random2];
		deckData[random2] = card;
	};


	int amount = 0;
	for (std::string card : deckData) {
		amount++;
		std::cout << card << ' ';
	};
	std::cout << '\n';
	std::cout << amount << '\n';
	std::cout << '\n';



	static std::vector <std::string> deck1 = {};
	static std::vector <std::string> deck2 = {};

	for (int i = 1; i <= 56; i++) {
		if (i < 29) {
			deck1.push_back(deckData[i]);
		};
		if (i > 28) {
			deck2.push_back(deckData[i]);
		};
	};

	for (std::string card : deck1) {
		std::cout << card << ' ';
	};
	std::cout << '\n';
	for (std::string card : deck2) {
		std::cout << card << ' ';
	};
	std::cout << '\n';
	std::cout << '\n';




	bool equal;
	std::vector <std::string> newDeck;
	std::string winner;
	int stack;
	int rounds;
	std::string choice;

	winner = "?";
	rounds = 0;
	while (winner == "?") {
		rounds++;


		equal = false;
		if (cardValue(deck1[0]) == cardValue(deck2[0])) {
			equal = true;
		};


		if (deck1.size() < 4 && equal) {
			winner = "P2";
			break;
		};
		if (deck2.size() < 4 && equal) {
			winner = "P1";
			break;
		};
		if (deck1.size() == 1) {
			winner = "P2";
			break;
		};
		if (deck2.size() == 1) {
			winner = "P1";
			break;
		};



		std::cout << "P1   ";
		for (std::string card : deck1) {
			std::cout << card << ' ';
		};
		std::cout << '\n';
		std::cout << "P2   ";
		for (std::string card : deck2) {
			std::cout << card << ' ';
		};
		std::cout << '\n';
		std::cout << '\n';


		if (cardValue(deck1[0]) > cardValue(deck2[0])) {
			newDeck = addOrRemove(deck1, "add", deck1[0], deck2[0]);
			deck1 = newDeck;

			newDeck = addOrRemove(deck2, "remove", deck1[0], deck2[0]);
			deck2 = newDeck;
			continue;
		};

		if (cardValue(deck2[0]) > cardValue(deck1[0])) {
			newDeck = addOrRemove(deck2, "add", deck2[0], deck1[0]);
			deck2 = newDeck;

			newDeck = addOrRemove(deck1, "remove", deck2[0], deck1[0]);
			deck1 = newDeck;
			continue;
		};


		stack = 1;
		while (cardValue(deck1[3*stack]) == cardValue(deck2[3*stack])) {
			stack++;
		};


		if (equal && cardValue(deck1[3*stack]) > cardValue(deck2[3*stack])) {
			for (int n = 0; n <= 3; n++) {
				newDeck = addOrRemove(deck1, "add", deck1[0], deck2[0]);
				deck1 = newDeck;

				newDeck = addOrRemove(deck2, "remove", deck1[0], deck2[0]);
				deck2 = newDeck;
			};
			continue;
		};

		if (equal && cardValue(deck2[3*stack]) > cardValue(deck1[3*stack])) {
			for (int n = 0; n <= 3; n++) {
				newDeck = addOrRemove(deck2, "add", deck2[0], deck1[0]);
				deck2 = newDeck;

				newDeck = addOrRemove(deck1, "remove", deck2[0], deck1[0]);
				deck1 = newDeck;
			};
			continue;
		};

	};


	std::cout << '\n';
	std::cout << '\n';
	std::cout << "After  " << rounds << "  rounds,  " << winner << "  is the winner!" << '\n';
	std::cout << '\n';


	return 0;
};

