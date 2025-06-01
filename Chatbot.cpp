
#include <iostream>
#include <random>
#include <vector>
#include <string>

class Convert {

public:

	// set random seed for Random()
	void SetRandom(int r) {
		randIndex = r;
	};


	// scrolls through randNum based on randIndex
	long long unsigned int Random() {
		if (randIndex > 17) {
			randIndex = 1;
			return (randNum % 10);
		};
		long long int p = pow(10, randIndex);
		randIndex++;
		return ((randNum % (p * 10)) / p);
	};

	std::string RemoveGrammar(std::string str) {

		std::string msg = "";
	
		for (int i = 0; i < str.length(); i++) {
		
			// 97-122
			if ((char(str[i]) < 97) || (char(str[i]) > 122)) {
				continue;
			};

			msg += str[i];
		};

		return msg;
	};

	// returns word by its index in the sentence (0, 1, 2, ..)
	std::string GetWord(std::string str, int index) {

		int wordCount = 0;
		int wordIndex = 0;
		std::string word = "";
		for (int i = 0; i < str.length(); i++) {

			if ((str[i] != ' ') && (i != str.length() - 1)) {
				continue;
			};

			word = "";
			for (int w = wordIndex; w <= str.length(); w++) {

				if (w >= str.length()) {
					break;
				};

				if (str[w] == ' ') {
					break;
				};
				
				word += str[w];
			};

			if (wordCount == index) {
				return RemoveGrammar(word);
			};

			wordIndex = i + 1;
			wordCount++;
		};
		
		return "x";
	};


	// returns index of word
	int FindWord(std::string str, std::string word) {

		std::string currentWord;
		int index = 0;
		
		while (true) {
	
			if (GetWord(str, index) == word) {
				return index;
			};

			if (GetWord(str, index) == "x") {
				break;
			};

			index++;
		};
	
		return -1;
	};


	// returns random Synonym (could be word itself)
	std::string Synonym(std::string str, int index) {
		int indexC = -1;

		str = RemoveGrammar(str);

		for (int c = 0; c < synonyms.size(); c++) {
			for (int r = 0; r < synonyms[c].size(); r++) {

				if (synonyms[c][r] == str) {
					indexC = c;
					break;
				};

			};
		};

		if (indexC == -1) {
			return str;
		};

		return (synonyms[indexC][index % (synonyms[indexC].size())]);
	};


	int FindSynonym(std::string str, std::string word) {	// find  a way to get length of synonym row; rn 25 temporarily
	
		for (int s = 1; s < 25; s++) {

			if (FindWord(str, Synonym(word, s)) == -1) {
				continue;
			};
			return FindWord(str, Synonym(word, s));

		};

		return -1;
	};


	bool IsSynonym(std::string word1, std::string word2) {
	
		for (int s = 0; s < 25; s++) {
			
			if (word1 == Synonym(word2, s)) {
				return true;
			};

		};
	
		return false;
	};

	std::string syn(std::string word) {
		return (Synonym(word, Random()));
	};

	std::string GenerateResponse(std::string str, std::string subject, std::string verb) {

		std::string response = "x";

		std::vector<std::vector<std::string>> responseSheet{

			//word1,	(word2),	response1,	(response2)

			{"+log", "logging"},

			{"+talk", "+you",	"i can't really talk, as im " + syn("just") + " a " + syn("algorithm")},

			{"+want", "+game",	"it makes me " + syn("very") + " sad that i am can't play"},
			{"+turtle",			"animals are a responsibility, but they are " + syn("really") + " cool " + syn("sometimes")},
			{"+dumb",			syn("maybe") + " you're the " + syn("idiot")},
			{"+machine",			"a " + syn("machine") + " is a delicate piece of art, built with a " + syn("large") + " amount of " + syn("work")},
			{"+stop",			"if it stops, will it " + syn("start") + " again?"},
			{"+or",				"i know you " + syn("can") + " " + syn("decide"),	syn("definitely") + " one of them", "the " + syn("first") + ", " + syn("definitely")},
			{"+work",			syn("maybe") + " " + syn("sleep") + " is what you " + syn("require")},
			{"+rock",			syn("rock")},
			{"+think",	"+you",	"i can't " + syn("think") + ", i am " + syn("just") + " a " + syn("machine")},
			{"+think",			"to " + syn("think") + " is " + syn("hard")},
			{"+tired",			"you should " + syn("rest")},
			{"+sleep",			syn("sleep") + " is " + syn("very") + " " + syn("important")},
			{"+you", "+doing",	"i am " + syn("feeling") + " " + syn("well")},
			{"+really",			syn("yes"),	syn("really") + " " + syn("could") + " be an overstatement"},
			{"+loud",			syn("maybe") + " you " + syn("need") + " " + syn("some") + " " + syn("silence")},
			{"+quiet",			"quiet doesnt " + syn("really") + " mean peace"},
			{"+first",			"how one got" + syn("there") + " is " + syn("really") + " " + syn("important")},
			{"+error",			" an " + syn("error") + " " + syn("can") + "be a break from the " + syn("mundane")},
			{"+oh",				"the most " + syn("great") + " " + syn("realization") + "s are punctuated with an 'oh'", "what " + syn("surprised") + " you?" },
			{"+lol",			"whats so " + syn("funny") + "?"},

			{"+why",			"maybe the " + syn("reason") + " for that is more complicated than you think"},
			{"+who",			syn("maybe") + " you don't " + syn("truly") + " " + syn("know") + " that person"},
			{"+what",			syn("definitely") + " something" },
			{"+where",			"every " + syn("place") + " means something"},
			{"+when",			"the time must be " + syn("right") + " for things to " + syn("happen")},


			{"+hi",				syn("hi"), "anything to talk " + syn("about") + "?"},
			{"+bye",			"bye"},
			{"+no",				"you don't " + syn("agree") + "?", "that makes sense"},
			{"+yes", "-no",		"you " + syn("agree") + "?", "im " + syn("glad") + " you " + syn("agree")},


		};

		for (int c = 0; c < responseSheet.size(); c++) {
			
			int responseIndex = 0;
			bool valid = true;
			std::string msg;
			for (int r = 0; r < responseSheet[c].size(); r++) {

				msg = responseSheet[c][r];
				
				if ((msg[0] != '+') && (msg[0] != '-')) {
					responseIndex = r;
					break;
				};

				if ((msg[0] == '+') && (FindSynonym(str, msg) == -1)) {
					valid = false;
				};

				if ((msg[0] == '-') && (FindSynonym(str, msg) != -1)) {
					valid = false;
				};
				
			};

			if (!valid) {
				continue;
			};
			
			response = (responseSheet[c][(Random() % (responseSheet[c].size() - responseIndex)) + responseIndex]);

			if (response == "logging") {
			errorLog.push_back(tempLog);
			};
			tempLog = str + " -> '" + response + "'";

			return response;
		};

		if ((subject != "x") && (verb != "x")) {
			response = subject + " " + verb + "?";
		};

		if (response == "x") {
			response = syn("sorry") + ", " + syn("could") + " you " + syn("say") + " that again?";
			errorLog.push_back("'" + str + "'");
		};


	
		return response;
	};
	

	std::string Respond(std::string input) {
		//std::cout << "times\n";
		std::string response = "x";

		std::string subject = "x"; // i, you, other
		int subjIndex = 0;
		std::string verb = "x";

		if (FindSynonym(input, "i") != -1) {
			subject = "i";
			subjIndex = FindSynonym(input, "i");
		};

		if (FindSynonym(input, "you") != -1) {
			subject = "you";
			subjIndex = FindSynonym(input, "you");
		};

		if (FindSynonym(input, "he") != -1) {
			subject = "he";
			subjIndex = FindSynonym(input, "he");
		};

		if (FindSynonym(input, "she") != -1) {
			subject = "she";
			subjIndex = FindSynonym(input, "she");
		};

		if (FindSynonym(input, "they") != -1) {
			subject = "they";
			subjIndex = FindSynonym(input, "they");
		};

		verb = GetWord(input, subjIndex + 1);

		//response = "tone: " + std::to_string(tone) + "  subj: " + std::to_string(subject) + "  verb: " + verb + '\n';

		response = GenerateResponse(input, subject, verb);

		if (response == "x") {
			//response = syn("sorry") + ", i didn't " + syn("really") + " " + syn("hear") + " that, can you say it again?";
			//std::cout << "error\n";
			errorLog.push_back(input);
		};

		return response;
	};
	
	std::vector<std::string> errorLog = {};


private:

	std::string tempLog = "";

	int randIndex = 1;
	long long unsigned int randNum = 18375620184722937;

	std::vector<std::vector<std::string>> synonyms = {
		{"he", "his"},
		{"she", "her", "hers"},
		{"they","others", "other",},
		{"maybe", "probably"},
		{"feel", "feeling", "well", "healthy"},
		{"really?", "you sure?"},
		{"like", "enjoy"},
		{"bye", "goodbye", "cya"},
		{"hi", "hello", "yo", "wassup"},
		{"no", "nah", "nope", "not"},
		{"yes", "yeah", "yea", "yup", "okay", "ok"},
		{"dont", "doesnt", "does not", "do not", "wont", "will not"},
		{"idiot", "stupid", "dumb", "fool", "mean"},
		{"limited", "slow", "challenged"},
		{"stop", "halt"},
		{"soon", "quickly", "fast"},
		{"know", "believe", "think"},
		{"really", "true", "truly", "definitely", "surely","completely"},
		{"place", "location", "area"},
		{"happy", "occur"},
		{"right", "correct"},
		{"must", "has to"},
		{"can", "could", "will"},
		{"problem", "challenge", "issue"},
		{"try", "attempt"},
		{"interesting", "cool", "smart", "funny", "incredible", "amazing", "fantastic", "great", "good", "nice"},
		{"stuff", "thing", "item"},
		{"machine", "algorithm", "process"},
		{"you", "you're", "youre", "you", "your", "we"},
		{"i", "me", "myself", "my"},
		{"glad", "happy", "joyful"},
		{"it", "that", "it's", "its"},
		{"hear", "catch"},
		{"negative", "downbeat", "sad"},
		{"here", "present", "existing"},
		{"want", "wish", "desire", "wanna", "pick", "choose", "want to"},
		{"one", "two", "three", "both", "none", "neither", "all"},
		{"first", "second", "third", "last"},
		{"choice", "decision"},
		{"do", "act"},
		{"did", "was"},
		{"kill", "kys", "die"},
		{"violence", "danger", "harm"},
		{"too", "as well", "additionaly"},
		{"just", "simply", "basically", "essentially"},
		{"job", "work", "hard", "difficult", "toil", "labor"},
		{"tired", "exhausted"},
		{"sleep", "rest"},
		{"about", "of"},
		{"unknown", "mysterious"},
		{"chance", "possibility"},
		{"say", "talk about"},
		{"reason", "motivation", "drive"},
		{"agree", "think so"},
		{"important", "of value", "of importance", "valuable"},
		{"very", "greatly", "extremely", "largely"},
		{"error", "issue", "problem"},
		{"boring", "mundane", "mindnumbing"},
		{"there", "here", "nowhere"},
		{"got", "received"},
		{"maybe", "perhaps"},
		{"need", "require"},
		{"some", "bit"},
		{"silence", "quiet", "peace"},
		{"realization", "revelation"},
		{"wow", "wowzers", "wowza", "realize", "oh"},
		{"surprised", "shocked", "bamboozled", "interested"},
		{"start", "begin"},
		{"turtle", "dog", "cat", "lizard", "bird", "reptile", "mammal"},
		{"rock", "paper", "scissor", "scissors"},
		{"things", "stuff"},
		{"lol", "LOL", "haha"},
		{"decide", "pick"}

	
	};


};

int main() {

	Convert c;	

	// GetWord(int index)			- gets word by index
	// FindWord (str, word)			- returns index of word in str			; else -1
	// FindSynonym (str, word)		- find word/synonyms	; else -1
	// IsSynonym (str, index, word)	- if synonym of word is at that index
	// SetRandom()					- set rnd seed
	// Random()						- get rnd number
	// Synonym(std::string str)		- get similar word/phrase for str
	// Respond(std::string input)	- respond to input
	// RemoveGrammar(str)			- removes non-char letters


	//std::cout << c.GetWord("i, am a smart person.", 4);

	//std::cout << c.FindWord("this is a great. sentence!", "bad");

	//std::cout << c.GetWord("+a plus is in here", 4);


	c.SetRandom(3);
	
	std::cout << "Hello, I'm a computer you can talk to. Anything you want to talk about?\n";

	std::string input;
	std::string response;
	bool running = true;
	while (running) {

		std::cout << "\t< ";
		std::getline(std::cin, input);

		response = c.Respond(input);
		std::cout << response << '\n';

		if (response == "bye") {
			running = false;
			continue;
		};

	};

	std::cout << "\n\n\n\nError log (bot had no response for these inputs):\n";
	for (std::string e : c.errorLog) {
		std::cout << e << '\n';
	};
	
	return 0;
}