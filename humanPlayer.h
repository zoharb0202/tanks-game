#pragma once
#include "player.h"
class HumanPlayer : public Player {
public:
    HumanPlayer(int id) : Player(id) {} 


   void handleTurn(char board[GameConfig::HEIGHT][GameConfig::WIDTH], std::vector<Shell>& shells) override;

    bool isComputer() const override { return false; }
};


