#pragma once
#include "player.h"
#include "Tank.h"
#include <vector>
#include <map>
#include "shell.h"

class ComputerPlayer : public Player {
private:
    Player* enemy = nullptr;
public:
    ComputerPlayer(int id) : Player(id) {}
    void setEnemy(Player* p) { enemy = p; }
    Player* getEnemy() const { return enemy; }
    bool isEnemyInSightAndShoot(Tank* tank, Player* enemy, std::vector<Shell>& shells, char board[GameConfig::HEIGHT][GameConfig::WIDTH]);

    bool isShellChasing(char board[GameConfig::HEIGHT][GameConfig::WIDTH], const Tank* tank, const std::vector<Shell>& shells);
    void handleTurn(char board[GameConfig::HEIGHT][GameConfig::WIDTH], std::vector<Shell>& shells) override;


    bool isComputer() const override { return true; }

    


};

