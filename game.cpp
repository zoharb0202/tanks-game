#include "game.h"
#include "Tank.h"         
#include <conio.h>
#include <Windows.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <filesystem>
#include "player.h"
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "computerPlayer.h"
#include "humanPlayer.h"
#include "steps.h"
#include <string>



using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;


static char board[BOARD_ROWS][BOARD_COLS];
int modeChoice;


void Game::showInstructions()
{
	clrscr();
	cout << "Instructions:\n";
	cout << "This is a 2-player game. Each player controls a tank (or multiple tanks) using separate keys.\n";
	cout << "\nThe objective of the game is to be the last tank standing.\n";
	cout << "\nYou can eliminate the opponent's tank by firing at them directly,\n or by waiting for them to step on a mine - both of which will result in an instant game over.\n";
	cout << "\nFiring at your opponent's cannon will disable said cannon and leave their tank helpless.\n";
	cout << "\nThe only way to achive a win with a missing cannon is to hope the opposing tank will step on a mine,\n so be mindfull not to lose it!\n";
	cout << "\nEach map's border will be a tunnel,\n which can be traversed by both tanks and tank shells to reach the side opposite to the entry point.\n Use them to your adventage!\n";
	cout << "\nTunnels can sometimes be blocked by walls,\n which can only be cleared by firing 2 tank shells at the same wall.\n";
	cout << "\nPress any key to continue....";
	_getch();
	clrscr();
	cout << "Controls:\n";
	cout << "------------------------------------------------------\n";
	cout << " Action                    | Player 1 | Player 2   \n";
	cout << "---------------------------|----------|------------\n";
	cout << " RIGHT track forward       |    E     |     O      \n";
	cout << " RIGHT track backwards     |    D     |     L      \n";
	cout << " LEFT track forward        |    Q     |     U      \n";
	cout << " LEFT track backwards      |    A     |     J      \n";
	cout << " STAY                      |    S     |     K      \n";
	cout << " Shoot                     |    W     |     I (i)  \n";
	cout << " Switch tank               |    Z     |     M      \n";
	cout << "------------------------------------------------------\n";
	cout << "\nPress any key to continue....";
	_getch();
	clrscr();
	cout << "Symbol guide : \n";
	cout << "------------------------------------------------------\n";
	cout << " Game element        | Corresponding symbol  \n";
	cout << "---------------------|-----------------------\n";
	cout << " Tank                |   O    \n";
	cout << " Cannon              | | or / or \\ depending on direction      \n";
	cout << " Mine                |   @    \n";
	cout << " Wall                |   #    \n";
	cout << " Damaged wall        |   %    \n";
	cout << " Tunnel              | | or -    \n";
	cout << " Tank shell          |   *    \n";
	cout << "\nPress any key to continue....";
	_getch();

}

int Game::chooseGameMode() {
	int choice;
	clrscr();
	cout << "(1) Human vs. Human\n";
	cout << "(2) Human vs. Computer\n";
	cout << "(3) Computer vs. Computer\n";
	
	choice = _getch() - '0';
	return choice;

}

int Game::showMenu(bool& exit, string& screenChoice, vector<string>& filenames) {
	system("cls");
	int choice;
	int gameRunning = false;
	int color = false;

	getAllBoardFileNames(filenames);


	while (!gameRunning) {
		clrscr();
		cout << "Menu\n";
		cout << "(1) Start a new game\n";
		cout << "(2) Start a new game in color mode\n";
		cout << "(8) Present instructions and keys\n";
		cout << "(9) EXIT\n";
		choice = _getch() - '0';


		switch (choice) {
		case 1:
			modeChoice = chooseGameMode();
			cout << "Choose a screen" << endl;
			for (const auto& name : filenames) {
				// Extract screen identifier part for cleaner display
				size_t pos1 = name.find("tanks-game_");
				size_t pos2 = name.find(".screen");
				if (pos1 != string::npos && pos2 != string::npos) {
					string screenName = name.substr(pos1 + 11, pos2 - (pos1 + 11));
					cout << " - " << screenName << "\n";
				}
				else {
					cout << " - " << name << "\n"; // fallback
				}
			}
			cin >> screenChoice;
			gameRunning = true;
			clrscr();
			break;
		case 2:
			modeChoice = chooseGameMode();
			cout << "Choose a screen" << endl;
			for (const auto& name : filenames) {
				// Extract screen identifier part for cleaner display
				size_t pos1 = name.find("tanks-game_");
				size_t pos2 = name.find(".screen");
				if (pos1 != string::npos && pos2 != string::npos) {
					string screenName = name.substr(pos1 + 11, pos2 - (pos1 + 11));
					cout << " - " << screenName << "\n";
				}
				else {
					cout << " - " << name << "\n"; // fallback
				}
			}
			cin >> screenChoice;
			gameRunning = true;
			color = true;
			clrscr();
			break;
		case 8:
			showInstructions();
			break;
		case 9:
			cout << "Exiting the game. Goodbye!\n";
			exit = true;
			return color;
		default:
			cout << "Invalid choice. Please try again.\n";
		}

		cout << "\n";
	}
	return color;
}

bool Game::isWall(int x, int y) {
	return board[y][x] == '#';
}

bool Game::isMine(int x, int y) {
	return board[y][x] == '@';
}



bool Game::initBoard(const string& filename, map<int, Tank*>& player1_tanks, map<int, Tank*>& player2_tanks, Point& legendPos, Player& p1, Player& p2)
{
	const int LEGEND_WIDTH = 20;
	const int LEGEND_HEIGHT = 3;

	bool legendBoxDefined = false;
	int startX = 0;
	int startY = 0;
	ifstream screen_file(filename);

	int curr_row = 0;
	int curr_col = 0;
	char c;
	int tanks1_id = 1, tanks2_id = 1;

	while (!screen_file.get(c).eof() && curr_row < GameConfig::HEIGHT) {
		if (c == '\n') {
			if (curr_col < GameConfig::WIDTH) {
				strcpy(board[curr_row] + curr_col, string(GameConfig::WIDTH - curr_col - 1, ' ').c_str());
			}
			++curr_row;
			curr_col = 0;
			continue;
		}
		else if (c == '1') {
			Point currPos(curr_col, curr_row);
			int direction = rand() % 8;
			p1.incrementTanks();
			player1_tanks.insert({ tanks1_id++, new Tank(currPos, 1, static_cast<GameConfig::cannonDir>(direction)) });
		}
		else if (c == '2') {
			Point currPos(curr_col, curr_row);
			int direction = rand() % 8;
			p2.incrementTanks();
			player2_tanks.insert({ tanks2_id++, new Tank(currPos, 2, static_cast<GameConfig::cannonDir>(direction)) });
		}
		else if (c == 'L') {
			legendPos.setx(curr_col);
			legendPos.sety(curr_row);

			startX = curr_col - 1;
			startY = curr_row - 1;
			legendBoxDefined = true;

			// Check bounds, skip screen if out of range
			if (startX < 0 || startY < 0 ||
				startX + LEGEND_WIDTH + 2 >= GameConfig::WIDTH ||
				startY + LEGEND_HEIGHT + 2 >= GameConfig::HEIGHT)
			{
				std::cerr << "Skipping screen: Legend box out of bounds in file " << filename << endl;
				return false;  
			}

			// Draw legend box borders
			for (int y = startY; y <= startY + LEGEND_HEIGHT + 1; ++y) {
				for (int x = startX; x <= startX + LEGEND_WIDTH + 1; ++x) {
					if (y == startY || y == startY + LEGEND_HEIGHT + 1 ||
						x == startX || x == startX + LEGEND_WIDTH + 1) {
						board[y][x] = '&';
					}
				}
			}
			curr_col++; // Advance cursor
		}
		else if (!legendBoxDefined || !(curr_row >= startY && curr_row <= startY + LEGEND_HEIGHT + 1 &&
			curr_col >= startX && curr_col <= startX + LEGEND_WIDTH + 1)) {
			board[curr_row][curr_col++] = c;
		}
		else {
			curr_col++;  // skip legend area chars
		}
	}

	return true; // success
}


void Game::drawBoard() {
	for (int y = 0; y < BOARD_ROWS; ++y) {
		for (int x = 0; x < BOARD_COLS; ++x) {
			char c = board[y][x];
			if (c == '#' || c== '%' || c=='&') {
				gotoxy(GameConfig::MINX + x, GameConfig::MINY + y);
				cout << c;
			}
			if (c == '@') {
				gotoxy(GameConfig::MINX + x, GameConfig::MINY + y);
				cout << c;
			}
		}
	}
}

void Game::drawBoardColor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int y = 0; y < BOARD_ROWS; ++y) {
		for (int x = 0; x < BOARD_COLS; ++x) {
			char c = board[y][x];
			gotoxy(GameConfig::MINX + x, GameConfig::MINY + y);

			switch (c) {
			case '@':  // Mine
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
				break;
			case '#':  // Wall
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			case '%':  // Weakened wall
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
				break;
			case '|': case '-':  // Borders
				SetConsoleTextAttribute(hConsole, BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
				break;
			default:
				SetConsoleTextAttribute(hConsole, 7);  // Default
				break;
			}

			cout << c;
		}
	}
	SetConsoleTextAttribute(hConsole, 7); // Reset after draw
}

void Game::printScore(Point legPos) {
	gotoxy(legPos.getX(), legPos.getY());
	cout << "Player 1 Score: " << scoreP1;
	gotoxy(legPos.getX(), legPos.getY()+1);
	cout << "Player 2 Score: " << scoreP2;

}

void Game::pauseGame(map <int, Tank*> player1_tanks, map <int, Tank*> player2_tanks, bool& running, int color) {
	gotoxy(3, 4);
	cout << "Game paused, press ESC again to continue or X to go back to the main menu";
	char keyPressed = 0;

	int shellColorStep = 0;
	for (auto& [tankID, tankPtr] : player1_tanks) {
		if (tankPtr != nullptr) {
			tankPtr->drawShells(board, color, shellColorStep);
		}
	}

	for (auto& [tankID, tankPtr] : player2_tanks) {
		if (tankPtr != nullptr) {
			tankPtr->drawShells(board, color, shellColorStep);
		}
	}



	while (keyPressed != (char)GameConfig::eKeys::ESC && keyPressed != (char)GameConfig::eKeys::RETURN1 && keyPressed != (char)GameConfig::eKeys::RETURN2)
	{
		if (_kbhit())
			keyPressed = _getch();
	}
	if (keyPressed == (char)GameConfig::eKeys::RETURN1 || keyPressed == (char)GameConfig::eKeys::RETURN2) {
		running = false;
		resetScore();
		returnToMenu();
	}

	

	for (auto& [tankID, tankPtr] : player1_tanks) {
		if (tankPtr != nullptr) {
			gotoxy(tankPtr->getPosition().getX(), tankPtr->getPosition().getY());
			cout << " ";
			tankPtr->erase(board, color);
		}
	}
	for (auto& [tankID, tankPtr] : player2_tanks) {
		if (tankPtr != nullptr) {
			gotoxy(tankPtr->getPosition().getX(), tankPtr->getPosition().getY());
			cout << " ";
			tankPtr->erase(board, color);
		}
	}
	

	gotoxy(0, 4);
	for (int x = 0; x < GameConfig::WIDTH; ++x) {
		cout << board[4][x];
	}

}

void Game::printResult(Player& p1, Player& p2) {
	if (p1.getNumOfTanks()>0 && p2.getNumOfTanks()==0) {
		gotoxy(3, 4);
		cout << "Player 2 died, Player 1 wins!\n     Press any key to continue....";
		_getch();
	}

	else if (p2.getNumOfTanks() > 0 && p1.getNumOfTanks() == 0) {
		gotoxy(3, 4);
		cout << "Player 1 died, Player 2 wins!\n     Press any key to continue....";
		_getch();
	}
	else if (p1.getNumOfTanks() == 0 && p2.getNumOfTanks() == 0) {
		gotoxy(3, 4);
		cout << "Both players died at the same time, its a tie!\n     Press any key to continue....";
		_getch();
	}
}

void Game::handleShells(vector<Shell>& shells, Player& p1, Player& p2, bool& running, int color, Result& result, size_t iteration, bool silentandload){ //written using chatgpt
	
		for (auto& s : shells) s.move();

		size_t n = shells.size();
		vector<bool> collided(n, false);

		// Handle shell vs shell collisions
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = i + 1; j < n; ++j) {
				if (shells[i].position == shells[j].position) {
					collided[i] = collided[j] = true;
				}
			}
		}

		// Check collision of shells with tanks of both players
		for (size_t i = 0; i < n; ++i) {
			if (collided[i]) continue;

			Shell& s = shells[i];
			Point sp = s.position;

			for (Player* player : { &p1, &p2 }) {
				auto& tanksMap = player->getTanksMap();

				for (auto& [id, tank] : tanksMap) {
					if (!tank || !tank->isTankAlive()) continue;

					Point body = tank->getPosition();
					Point cannon = body.add(tank->getPointFromDir(tank->getCannonDir()));

					// Check hit to tank body
					if (sp == body) {
						result.pushResult(std::to_string(iteration) + " TANK HIT" + " X: " + std::to_string(body.getX()) + ", Y: " + std::to_string(body.getY()));

						tank->setTankStatus();
						tank->erase(board, color);
						
						
						player->decrementTanks();
						if (id == player->getControlledTank())
							player->changeControlledTank();

						tanksMap.erase(id);

						if (s.player != player->getPlayerID())
							(s.player == 1 ? updateScoreP1(5) : updateScoreP2(5));
						if (player->getNumOfTanks() == 0)
							running = false;
						collided[i] = true;
						break;
					}
					// Check hit to tank cannon
					else if (sp == cannon && tank->isCannonActive()) {
						result.pushResult(std::to_string(iteration) + " CANNON HIT" + " X: " + std::to_string(body.getX()) + ", Y: " + std::to_string(body.getY()));
						tank->hitCannon();
						if (s.player != player->getPlayerID())
							(s.player == 1 ? updateScoreP1(2) : updateScoreP2(2));
						collided[i] = true;
						break;
					}
				}
				if (collided[i]) break;
			}

			if (collided[i]) continue;

			// Wall and weakened wall hit detection
			int bx = sp.getX();
			int by = sp.getY();
			if (bx >= 0 && bx < BOARD_COLS && by >= 0 && by < BOARD_ROWS) {
				char& tile = board[by][bx];
				if (tile == '#') {
					
						result.pushResult(std::to_string(iteration) + " WALL HIT" + " X: " + std::to_string(bx) + ", Y: " + std::to_string(by));
						tile = '%';
					
					(s.player == 1 ? updateScoreP1(1) : updateScoreP2(1));
					gotoxy(GameConfig::MINX + bx, GameConfig::MINY + by);
					if(!silentandload)
					{
						cout << '%';
					}
					collided[i] = true;
				}
				else if (tile == '%') {
					
						result.pushResult(std::to_string(iteration) + " WEAKEND WALL HIT" + " X: " + std::to_string(bx) + ", Y: " + std::to_string(by));
						tile = ' ';
					
					(s.player == 1 ? updateScoreP1(2) : updateScoreP2(2));
					gotoxy(GameConfig::MINX + bx, GameConfig::MINY + by);
					cout << ' ';
					collided[i] = true;
				}
				else if (tile == '&') {
					collided[i] = true;
				}
			}
		}

		// Keep only surviving shells
		vector<Shell> survivors;
		survivors.reserve(n);
		for (size_t i = 0; i < n; ++i) {
			if (!collided[i]) {
				survivors.push_back(shells[i]);
			}
		}
		shells.swap(survivors);

		// Redraw shells
		int shellColorStep = 0;
		if(!silentandload)
		{
			for (auto& s : shells)
				s.draw(color, shellColorStep++);

		}

}


bool Game::compareResultsSilent(const std::string& baseName) {
	const std::string fileName = baseName + ".result";

	// 1) Read expected from disk
	std::ifstream expifs(fileName);
	if (!expifs.is_open()) {
		std::cerr << "Could not open expected result: " << fileName << "\n";
		return false;
	}
	std::vector<std::string> expected;
	std::string line;
	while (std::getline(expifs, line)) expected.push_back(line);
	expifs.close();

	// 2) Read actual (just written) from disk
	std::ifstream actifs(fileName);
	std::vector<std::string> actual;
	while (std::getline(actifs, line)) actual.push_back(line);
	actifs.close();

	// 3) Compare
	bool pass = (expected.size() == actual.size());
	size_t maxLines = max(expected.size(), actual.size());
	struct Diff { size_t line; std::string exp, act; };
	std::vector<Diff> diffs;
	for (size_t i = 0; i < maxLines; ++i) {
		const std::string& e = (i < expected.size() ? expected[i] : "<no line>");
		const std::string& a = (i < actual.size() ? actual[i] : "<no line>");
		if (e != a) {
			pass = false;
			if (diffs.size() < 5)  // cap number of reported diffs
				diffs.push_back({ i + 1, e, a });
		}
	}

	// 4) Print summary
	std::cout << (pass ? "Test PASSED for " : "Test FAILED for ")
		<< baseName << "\n";
	if (!pass) {
		for (auto& d : diffs) {
			std::cout << " Line " << d.line
				<< ": expected [`" << d.exp
				<< "`] but got [`" << d.act
				<< "`]\n";
		}
	}
	return pass;
}

void Game::getAllBoardFileNames(vector<string>& vec_to_fill) {
	namespace fs = std::filesystem;
	vec_to_fill.clear();  
	for (const auto& entry : fs::directory_iterator(fs::current_path())) {
		auto filename = entry.path().filename();
		auto filenameStr = filename.string();
		if (filenameStr.substr(0, 11) == "tanks-game_" && filename.extension() == ".screen") {
			vec_to_fill.push_back(filenameStr);
		}
	}
}

void Game::initPlayersByMode(int modeChoice, std::unique_ptr<Player>& p1, std::unique_ptr<Player>& p2) {
	p1.reset();
	p2.reset();
	switch (modeChoice) {
	case 1: // Human vs. Human
		p1 = std::make_unique<HumanPlayer>(1);
		p2 = std::make_unique<HumanPlayer>(2);
		break;

	case 2: // Human vs. Computer
		p1 = std::make_unique<HumanPlayer>(1);
		p2 = std::make_unique<ComputerPlayer>(2);
		dynamic_cast<ComputerPlayer*>(p2.get())->setEnemy(p1.get());
		break;

	case 3: // Computer vs. Computer
		p1 = std::make_unique<ComputerPlayer>(1);
		p2 = std::make_unique<ComputerPlayer>(2);
		dynamic_cast<ComputerPlayer*>(p1.get())->setEnemy(p2.get());
		dynamic_cast<ComputerPlayer*>(p2.get())->setEnemy(p1.get());
		break;

	default:
		std::cerr << "Invalid mode. Defaulting to Human vs Human.\n";
		p1 = std::make_unique<HumanPlayer>(1);
		p2 = std::make_unique<HumanPlayer>(2);
		break;
	}
}

void Game::run(bool isLoad, bool isSave, bool isSilent) {
	bool exit = false;
	
	std::unique_ptr<Player> p1, p2;
	if (isSave) {
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(L"tanks-game_*.steps", &findFileData);

		while (hFind != INVALID_HANDLE_VALUE) {
			DeleteFile(findFileData.cFileName);
			if (!FindNextFile(hFind, &findFileData)) {
				FindClose(hFind);
				break;
			}
		}
	}
	while (!exit) {

	vector<string> filenames;
	getAllBoardFileNames(filenames);
	sort(filenames.begin(), filenames.end()); // ensure lexicographic order

	vector<string> screensToPlay;
	int color = false;
	if (isLoad) {
		for (int i = 1; i <= 10; ++i) {
			std::ostringstream screenName;
			screenName << "tanks-game_0" << i;
			std::string screenFile = screenName.str() + ".screen";
			std::string stepsFile = screenName.str() + ".steps";

			if (std::filesystem::exists(screenFile) && std::filesystem::exists(stepsFile)) {
				screensToPlay.push_back(screenFile);
			}
		}

		if (screensToPlay.empty()) {
			std::cerr << "No valid screen+steps file pairs found for load mode.\n";
			return;
		}
	}
	else {

		string screenChoice;
		resetMenu();
		color = showMenu(exit, screenChoice, filenames);
		if (exit) return;

		string selectedFilename = "tanks-game_" + screenChoice + ".screen";
		auto it = std::find(filenames.begin(), filenames.end(), selectedFilename);
		if (it != filenames.end()) {
			screensToPlay.assign(it, filenames.end());
		}
	}

			for (size_t i = 0; i < screensToPlay.size(); ++i) {
				resetMenu();
				std::string screenFile = screensToPlay[i];
				std::string stepsFile = screenFile.substr(0, screenFile.find_last_of('.')) + ".steps";

				string baseName = screenFile.substr(0, screenFile.find_last_of('.'));
				Result result(baseName);
				Steps steps;
				if (isLoad) {
					steps = Steps::loadSteps(baseName + ".steps");
					
				
				


					modeChoice = steps.getModeChoice();
					color = steps.getColorMode();
				}
				else {
					// Generate and record a single timestamp seed
					long seed = static_cast<long>(time(nullptr));
					steps.setRandomSeed(seed);
					srand(seed);

					steps.setModeChoice(modeChoice);
					steps.setColorMode(color);
				
				}
				/*else {
					steps.setRandomSeed(rand());
					srand(steps.getRandomSeed());
					steps.setModeChoice(modeChoice);
					steps.setColorMode(color);
				}*/

				if (p1) {
					for (auto& [id, t] : p1->getTanksMap())
						if (t) delete t;
					p1->getTanksMap().clear();
				}

				if (p2) {
					for (auto& [id, t] : p2->getTanksMap())
						if (t) delete t;
					p2->getTanksMap().clear();
				}

				memset(board, ' ', sizeof(board));
				initPlayersByMode(modeChoice, p1, p2);
				Point legPos;
				vector<Shell> shells;
				srand(steps.getRandomSeed());

				bool loaded = initBoard(screenFile, p1->getTanksMap(), p2->getTanksMap(), legPos, *p1, *p2);

				if (!loaded) {
					std::cerr << "Screen " << screenFile << " skipped due to legend position.\n";
					continue;
				}

				clrscr();
				if (!(isSilent&&isLoad))
				{
					if (!color)
						drawBoard();
					else
						drawBoardColor();
				}

				bool running = true;



				size_t iteration = 0;
				while (running) {
					if (!(isLoad && isSilent))
					{
						for (auto& [tankID, tankPtr] : p1->getTanksMap()) {
							if (tankPtr != nullptr) {
								tankPtr->erase(board, color);
							}
						}
						for (auto& [tankID, tankPtr] : p2->getTanksMap()) {
							if (tankPtr != nullptr) {
								tankPtr->erase(board, color);
							}
						}
						for (auto& s : shells) s.erase(board);
					}

					

					if (_kbhit()) {
						char ch = _getch();
						if (ch == (char)GameConfig::eKeys::ESC) {
							pauseGame(p1->getTanksMap(), p2->getTanksMap(), running, color);
						}
					}

					if (p1->isComputer())
						p1->handleTurn(board, shells);

					if (p2->isComputer())
						p2->handleTurn(board, shells);

					bool currMove;
					Point currVelocity;
					if (!(p1->isComputer())) {
						if (GetAsyncKeyState('Z') & 0x8000) {
							if (isSave) {
								std::set<char> keysThisTick;
								keysThisTick.insert('Z');
								if (!keysThisTick.empty())
									steps.addStep(iteration, keysThisTick);
							}
							if (p1->getNumOfTanks() != 1) {
								if (p1->getControlledTank() != p1->getNumOfTanks()) {
									p1->setControlledTank(p1->getControlledTank() + 1);
								}
								else
									p1->setControlledTank(1);
							}
						}
					}

					if (!(p2->isComputer())) {
						if (GetAsyncKeyState('M') & 0x8000) {
							if (isSave) {
								std::set<char> keysThisTick;
								keysThisTick.insert('M');
								if (!keysThisTick.empty())
									steps.addStep(iteration, keysThisTick);
							}
							if (p2->getNumOfTanks() != 1) {
								if (p2->getControlledTank() != p2->getNumOfTanks()) {
									p2->setControlledTank(p2->getControlledTank() + 1);
								}
								else
									p2->setControlledTank(1);
							}
						}
					}

					if (!(p1->isComputer())) {
						if (p1->getControlledTank() != -1) {
							auto it1 = p1->getTanksMap().find(p1->getControlledTank());
							if (it1 != p1->getTanksMap().end() && it1->second != nullptr) {
								std::set<char> keysThisTick;
								if (isLoad) {
									if (steps.isNextStepOnIteration(iteration)) {
									
										keysThisTick = steps.popStep();
									}
								}
								else {
									if (GetAsyncKeyState('Q') & 0x8000) keysThisTick.insert('Q'); // LEFT track forward
									if (GetAsyncKeyState('A') & 0x8000) keysThisTick.insert('A'); // LEFT track backward
									if (GetAsyncKeyState('E') & 0x8000) keysThisTick.insert('E'); // RIGHT track forward
									if (GetAsyncKeyState('D') & 0x8000) keysThisTick.insert('D'); // RIGHT track backward
									if (GetAsyncKeyState('S') & 0x8000) keysThisTick.insert('S'); // Stay
									if (GetAsyncKeyState('W') & 0x8000) keysThisTick.insert('W'); // Shoot
									if (!keysThisTick.empty())
										steps.addStep(iteration, keysThisTick);
								}

								it1->second->checkKeysP1(p1.get(), board, keysThisTick);
								it1->second->tick(); // cooldown timers, etc.

								// New: Handle shooting if 'W' pressed this tick
								if (keysThisTick.count('W') &&
									it1->second->isCannonActive() &&
									it1->second->getShootCooldown() == 0) {

									shells.emplace_back(
										it1->second->getPosition().add(it1->second->getPointFromDir(it1->second->getCannonDir())),
										it1->second->getPointFromDir(it1->second->getCannonDir()));
									shells.back().player = 1;
									it1->second->setShootCooldown(HUMAN_COOLDOWN);
								}
							}
						}
					}


					if (!(p2->isComputer())) {
						if (p2->getControlledTank() != -1) {
							auto it2 = p2->getTanksMap().find(p2->getControlledTank());
							if (it2 != p2->getTanksMap().end() && it2->second != nullptr) {
								std::set<char> keysThisTick;
								if (isLoad) {
									if (steps.isNextStepOnIteration(iteration)) {
										
										keysThisTick = steps.popStep();
									}
								}
								else
								{
									if (GetAsyncKeyState('U') & 0x8000) keysThisTick.insert('U'); // LEFT track forward
									if (GetAsyncKeyState('J') & 0x8000) keysThisTick.insert('J'); // LEFT track backward
									if (GetAsyncKeyState('O') & 0x8000) keysThisTick.insert('O'); // RIGHT track forward
									if (GetAsyncKeyState('L') & 0x8000) keysThisTick.insert('L'); // RIGHT track backward
									if (GetAsyncKeyState('K') & 0x8000) keysThisTick.insert('K'); // Stay
									if (GetAsyncKeyState('I') & 0x8000) keysThisTick.insert('I'); // Shoot
									if (!keysThisTick.empty()) {
										steps.addStep(iteration, keysThisTick);
									}
								}
								it2->second->checkKeysP2(p2.get(), board, keysThisTick);
								it2->second->tick();

							}
						}
					}

					auto& m1 = p1->getTanksMap();
					for (auto it = m1.begin(); it != m1.end(); /*no increment here*/) {
						Tank* t = it->second;
						if (t) {
							// 1) move if needed
							if (t->isMovingStat()) {
								t->move(t->getVelX(), t->getVelY(), board);
							}
							// 2) immediate mine check
							int x = t->getPosition().getX();
							int y = t->getPosition().getY();
							if (board[y][x] == '@') {
								result.pushResult(std::to_string(iteration) + " MINE P1" + " X: " + std::to_string(x) + ", Y: " + std::to_string(y));
								t->setTankStatus();
								t->erase(board, color);
								if (it->first == p1->getControlledTank())
									p1->changeControlledTank();
								delete t;
								it = m1.erase(it);
								p1->decrementTanks();
								if (p1->getNumOfTanks() == 0) running = false;
								continue;   // skip ++it, since we erased
							}
						}
						++it;
					}
					auto& m2 = p2->getTanksMap();
					for (auto it = m2.begin(); it != m2.end(); ) {
						Tank* t = it->second;
						if (t) {
							if (t->isMovingStat()) {
								t->move(t->getVelX(), t->getVelY(), board);
							}
							int x = t->getPosition().getX();
							int y = t->getPosition().getY();
							if (board[y][x] == '@') {
								result.pushResult(std::to_string(iteration) + " MINE P2" + " X: " + std::to_string(x) + ", Y: " + std::to_string(y));
								t->setTankStatus();
								t->erase(board, color);
								if (it->first == p2->getControlledTank())
									p2->changeControlledTank();
								delete t;
								it = m2.erase(it);
								p2->decrementTanks();
								if (p2->getNumOfTanks() == 0) running = false;
								continue;
							}
						}
						++it;
					}




					if (!(p2->isComputer())) {
						if (p2->getControlledTank() != -1) {
							auto it2 = p2->getTanksMap().find(p2->getControlledTank());
							if (it2 != p2->getTanksMap().end() && it2->second != nullptr) {
								if ((GetAsyncKeyState('I') & 0x8000) && it2->second->isCannonActive() && it2->second->getShootCooldown() == 0) {
									shells.emplace_back(
										it2->second->getPosition().add(it2->second->getPointFromDir(it2->second->getCannonDir())),
										it2->second->getPointFromDir(it2->second->getCannonDir()));
									shells.back().player = 2;
									it2->second->setShootCooldown(HUMAN_COOLDOWN);
								}
							}
						}
					}
					if(!(isSilent&&isLoad))
					{
						for (auto& [tankID, tankPtr] : p1->getTanksMap()) {
							if (tankPtr != nullptr) {
								tankPtr->draw(color);
							}
						}
						for (auto& [tankID, tankPtr] : p2->getTanksMap()) {
							if (tankPtr != nullptr) {
								tankPtr->draw(color);
							}
						}
					}




					handleShells(shells, *p1, *p2, running, color, result, iteration, (isSilent&&isLoad));


					if (!(isLoad && isSilent))
					{
						printScore(legPos);
					}

					++iteration;
					if ((isLoad && isSilent))
						Sleep(0);
					else if (isLoad&&!isSilent)
						Sleep(100);
					else
						Sleep(100);

				}
				if (checkReturn())
					break;
				if (!(isLoad && isSilent))
				{
					printResult(*p1, *p2); // Show result before next screen
				}
				clrscr();
				/*std::string baseName = filenames[i].substr(0, filenames[i].find_last_of('.')); */
				if (isSave)
				{
					steps.saveSteps(baseName + ".steps");
					result.pushResult("SCORE P1=" + std::to_string(Game::scoreP1) + " P2=" + std::to_string(Game::scoreP2));
					result.writeToFile(steps.getRandomSeed());
				}
				if (isLoad && isSilent) {
					// compare and exit
					compareResultsSilent(baseName);
					return;
				}
			}


			// Clean up remaining tanks
			for (auto& [id, t] : p1->getTanksMap()) {
				if (t) delete t;
			}
			p1->getTanksMap().clear();

			for (auto& [id, t] : p2->getTanksMap()) {
				if (t) delete t;
			}
			p2->getTanksMap().clear();

			resetScore();
			if (!checkReturn()) {
				cout << "All screens completed.\nPress any key to return to menu...\n";
				if (isLoad)
					break;
				_getch();
			}
	}
	
}
