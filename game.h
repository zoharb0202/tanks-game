#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <conio.h>
#include <Windows.h>
#include "general.h"
#include "gameConfig.h"
#include "tank.h"
#include <vector>
#include <string>
#include <map>
#include "result.h"

using std::string;
using std::map;
using std::vector;




class Game
{
	int scoreP1 = 0, scoreP2 = 0;
	bool ReturnToMenu = false;

public:
	Game() {};
	void run(bool isLoad, bool isSave, bool isSilent);
	void drawBoard();
	void drawBoardColor();
	void showInstructions();
	bool initBoard(const string& filename, map <int , Tank*>& player1_tanks, map <int, Tank*>& player2_tanks, Point& legendPos, Player& p1, Player& p2);
	int showMenu(bool& exit, string& screenChoice, vector<string>& filenames);
	bool isWall(int x, int y);
	bool isMine(int x, int y);
	void pauseGame(map <int, Tank*> player1_tanks, map <int, Tank*> player2_tanks, bool& running, int color);
	void printResult(Player& p1, Player& p2);
	void handleShells(vector<Shell>& shells, Player& p1, Player& p2, bool& running, int color, Result& result, size_t iteration, bool silentandload);
	void updateScoreP1(int score) { this->scoreP1 += score; }
	void updateScoreP2(int score) { this->scoreP2 += score; }
	void printScore(Point legPos);
	void getAllBoardFileNames(vector<string>& vec_to_fill);
	void resetScore() { scoreP1 = 0, scoreP2 = 0; }
	void resetMenu() { ReturnToMenu = false; }
	void returnToMenu() { ReturnToMenu = true; }
	bool checkReturn() { return ReturnToMenu; }
	int chooseGameMode();
	void initPlayersByMode(int modeChoice, std::unique_ptr<Player>& p1, std::unique_ptr<Player>& p2);
	bool compareResultsSilent(const std::string& baseName);
};

