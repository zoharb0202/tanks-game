#pragma once
#include "point.h"
#include <map>
#include "Tank.h"


using std::map;


class Player {
private:
    int playerID;
    int numOfTanks = 0;
    int controlledTank = 1; // -1 means no tank is selected
    map<int, Tank*> tanksMap;

public:
    Player(int id) : playerID(id) {}

    int getPlayerID() const {
        return playerID;
    }

    int getNumOfTanks() const {
        return numOfTanks;
    }

    map<int, Tank*>& getTanksMap() {
        return tanksMap;
    }


    void incrementTanks() {
        ++numOfTanks;
    }

    void decrementTanks() {
        if (numOfTanks > 0)
            --numOfTanks;
    }

    void setControlledTank(int tankIndex) {
        controlledTank = tankIndex;
    }

    int getControlledTank() const {
        return controlledTank;
    }

    void changeControlledTank() {
        if (tanksMap.empty()) {
            controlledTank = -1;
            return;
        }

        auto it = tanksMap.find(controlledTank);
        bool foundNext = false;

        // Start from next tank
        if (it != tanksMap.end())
            ++it;
        else
            it = tanksMap.begin();

        for (; it != tanksMap.end(); ++it) {
            if (it->second && it->second->isTankAlive()) {
                controlledTank = it->first;
                foundNext = true;
                break;
            }
        }

        // Fallback: try looping from beginning
        if (!foundNext) {
            for (auto& [id, tank] : tanksMap) {
                if (tank && tank->isTankAlive()) {
                    controlledTank = id;
                    return;
                }
            }
            controlledTank = -1; // No valid tanks
        }
    }

    virtual void handleTurn(char board[GameConfig::HEIGHT][GameConfig::WIDTH], std::vector<Shell>& shells) = 0;

    virtual bool isComputer() const = 0;
};
