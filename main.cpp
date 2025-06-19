#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include "general.h"
#include "gameConfig.h"
#include "game.h"



int main(int argc, char** argv) {
	bool isLoad=false;
	bool isSave=false;
	bool isSilent = false;
	if (argc > 1 ) {
		if (std::string(argv[1]) == "-load")
		{
			isLoad = true;
			if (argc > 2 && std::string(argv[2]) == "-silent")
				isSilent = true;
		}
		else if (std::string(argv[1]) == "-save")
			isSave = true;
	}
	Game().run(isLoad, isSave, isSilent);

}



