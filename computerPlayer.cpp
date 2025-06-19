#include "computerPlayer.h"

bool ComputerPlayer::isEnemyInSightAndShoot(Tank* tank, Player* enemy, std::vector<Shell>& shells, char board[GameConfig::HEIGHT][GameConfig::WIDTH]) {
    if (!tank || !tank->isTankAlive() || !tank->isCannonActive() || tank->getShootCooldown() > 0)
        return false;

    // Check all 8 directions (0 to 7)
    for (int dir = 0; dir < 8; ++dir) {
        Point direction = tank->getPointFromDir(static_cast<GameConfig::cannonDir>(dir));
        Point current = tank->getPosition().add(direction);

        // Prevent infinite loop with a max number of steps (diagonals included)
        int maxSteps = max(GameConfig::WIDTH, GameConfig::HEIGHT);

        for (int step = 0; step < maxSteps; ++step) {
            int x = (current.getX() + GameConfig::WIDTH) % GameConfig::WIDTH;
            int y = (current.getY() + GameConfig::HEIGHT) % GameConfig::HEIGHT;

            char tile = board[y][x];

            if (tile == '#' || tile == '%' || tile == '&')
                break;

            for (const auto& [id, enemyTank] : enemy->getTanksMap()) {
                if (!enemyTank || !enemyTank->isTankAlive()) continue;

                Point enemyPos = enemyTank->getPosition();
                Point enemyCannon = enemyPos.add(enemyTank->getPointFromDir(enemyTank->getCannonDir()));

                if (current == enemyPos || current == enemyCannon) {
                    // Aim and shoot
                    tank->setCannonDir(static_cast<GameConfig::cannonDir>(dir));
                    Point cannonTip = tank->getPosition().add(tank->getPointFromDir(tank->getCannonDir()));
                    shells.emplace_back(cannonTip, direction);
                    shells.back().player = tank->getPlayerID();
                    tank->setShootCooldown(CP_COOLDOWN);
                    return true;
                }
            }

            current = current.add(direction); // go further in the same direction
        }
    }

    return false; // No target found in any direction
}



void ComputerPlayer::handleTurn(char board[GameConfig::HEIGHT][GameConfig::WIDTH], std::vector<Shell>& shells) {

    if (getNumOfTanks() == 0 || !enemy || enemy->getNumOfTanks() == 0)
        return;



    for (auto& [id, tank] : getTanksMap()) {

        if (!tank || !tank->isTankAlive()) continue;
        if (getControlledTank() != -1 && getTanksMap().count(getControlledTank()) == 0)
            setControlledTank(-1); // reset invalid ID
        tank->tick();

        if (isEnemyInSightAndShoot(tank, enemy, shells, board))
            continue;

        if (isShellChasing(board,tank,shells)) {
            tank->setMoving(true);
            tank->setVelocity(tank->getPointFromDir(tank->getCannonDir()));
        }
        else {
            tank->setMoving(false);
        }
    }
}


bool ComputerPlayer::isShellChasing(char board[GameConfig::HEIGHT][GameConfig::WIDTH],const Tank* tank, const std::vector<Shell>& shells) {
    Point tankPos = tank->getPosition();

    for (const Shell& shell : shells) {
        Point shellPos = shell.position;
        Point dir = shell.direction;

        // Simulate shell movement in its direction
        Point current = shellPos;

        for (int i = 0; i < 10; ++i) { // limit how far we scan
            current = current.add(dir);

            if (current == tankPos) {
                return true;
            }

            // Stop if shell would hit wall
            int x = (current.getX() + GameConfig::WIDTH) % GameConfig::WIDTH;
            int y = (current.getY() + GameConfig::HEIGHT) % GameConfig::HEIGHT;
            char tile = board[y][x];
            if (tile == '#' || tile == '%' || tile == '&')
                break;
        }
    }
    return false;
}
