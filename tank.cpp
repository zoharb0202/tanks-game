#include "Tank.h"
#include "GameConfig.h"
#include <iostream>
#include <windows.h>
#include "general.h"
#include "game.h"

using std::cout;
using std::vector;

Point Tank::getPosition() const {
    return pos;
}


void Tank::draw(int colorMode) const {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (colorMode) {
        if (playerID == 1)
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        else if (playerID == 2)
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }

    gotoxy(GameConfig::MINX + pos.getX(), GameConfig::MINY + pos.getY());
    cout << 'O';

    if (hasCannon) {
        Point tip = pos.add(getPointFromDir(cannonDir));
        int tipX = (tip.getX() + GameConfig::WIDTH) % GameConfig::WIDTH;
        int tipY = (tip.getY() + GameConfig::HEIGHT) % GameConfig::HEIGHT;
        gotoxy(GameConfig::MINX + tipX, GameConfig::MINY + tipY);
        cout << getCannonChar(cannonDir);
    }

    if (colorMode)
        SetConsoleTextAttribute(hConsole, 7); // reset
}



void Tank::erase(const char board[GameConfig::HEIGHT][GameConfig::WIDTH], int color) const {
    // 1) Erase tank body
    {
        int x = pos.getX();
        int y = pos.getY();
        char bg = board[y][x];

        gotoxy(GameConfig::MINX + x, GameConfig::MINY + y);

        if (color) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


            if (bg == '#') SetConsoleTextAttribute(hConsole, 100); // dark gray wall
            else if (bg == '%') SetConsoleTextAttribute(hConsole, 100); // same as wall
            else if (bg == '@') SetConsoleTextAttribute(hConsole, 41); // red mine
            else if (bg == '-' || bg == '|') SetConsoleTextAttribute(hConsole, 47); // borders
            else SetConsoleTextAttribute(hConsole, 7); // default

            cout << bg;

            SetConsoleTextAttribute(hConsole, 7); // reset to default
        }
        else {
            cout << bg;
        }
    }

    if (hasCannon) {
        Point p = getPointFromDir(cannonDir);
        int bx = (pos.getX() + p.getX() + GameConfig::WIDTH) % GameConfig::WIDTH;
        int by = (pos.getY() + p.getY() + GameConfig::HEIGHT) % GameConfig::HEIGHT;
        char bg = board[by][bx];
        gotoxy(GameConfig::MINX + bx, GameConfig::MINY + by);
        cout << bg;
    }
}

char Tank::getCannonChar(GameConfig::cannonDir dir) const {
    if (dir == GameConfig::cannonDir::NORTH || dir == GameConfig::cannonDir::SOUTH) return '|';
    if (dir == GameConfig::cannonDir::EAST || dir == GameConfig::cannonDir::WEST) return '-';
    if (dir == GameConfig::cannonDir::NORTH_EAST || dir == GameConfig::cannonDir::SOUTH_WEST) return '/';
    if (dir == GameConfig::cannonDir::NORTH_WEST || dir == GameConfig::cannonDir::SOUTH_EAST) return '\\';
    return '?';
}
Point Tank::getPointFromDir(GameConfig::cannonDir dir) const {
    Point p;

    switch (dir)
    {
    case GameConfig::cannonDir::NORTH:
        p.setx(0);
        p.sety(-1);
        break;

    case GameConfig::cannonDir::NORTH_EAST:
        p.setx(1);
        p.sety(-1);
        break;

    case GameConfig::cannonDir::EAST:
        p.setx(1);
        p.sety(0);
        break;
    case GameConfig::cannonDir::SOUTH_EAST:
        p.setx(1);
        p.sety(1);
        break;

    case GameConfig::cannonDir::SOUTH:
        p.setx(0);
        p.sety(1);
        break;

    case GameConfig::cannonDir::SOUTH_WEST:
        p.setx(-1);
        p.sety(1);
        break;

    case GameConfig::cannonDir::WEST:
        p.setx(-1);
        p.sety(0);
        break;

    case GameConfig::cannonDir::NORTH_WEST:
        p.setx(-1);
        p.sety(-1);
        break;
    }

    return p;
}



void Tank::move(int x, int y, char board[GameConfig::HEIGHT][GameConfig::WIDTH]) {
    Point p(x, y);
    int newX = pos.getX() + p.getX();
    int newY = pos.getY() + p.getY();

    // If there's a wall, don't move
    char tile = board[(newY + GameConfig::HEIGHT) % GameConfig::HEIGHT][(newX + GameConfig::WIDTH) % GameConfig::WIDTH];
    if (tile != '#' && tile != '%' && tile != '&') {
        pos.setx((newX + GameConfig::WIDTH) % GameConfig::WIDTH);
        pos.sety((newY + GameConfig::HEIGHT) % GameConfig::HEIGHT);
    }

}

void Tank::setDirection(GameConfig::cannonDir newDir) {
    cannonDir = newDir;
}


void Tank::tick() {
    if (shootCooldown > 0)
        --shootCooldown;
}

void Tank::hitCannon() {
    hasCannon = false;
}

bool Tank::isAt(const Point& p) const {
    return pos.isEqual(p);
}

int Tank::getPlayerID() const {
    return playerID;
}


void Tank::checkKeysP1(Player* p1, char board[GameConfig::HEIGHT][GameConfig::WIDTH], const std::set<char>& keys){
    bool lf = keys.count('Q');
    bool rf = keys.count('E');
    bool lb = keys.count('A');
    bool rb = keys.count('D');
    bool stay = keys.count('S');


    // 1) STOP immediately on stay
    if (stay) {
        isMoving = false;
        velocity = Point(0, 0);
        rotationMode = RotationMode::NONE;
        return;
    }

    // 2) Detect rotation mode changes
    if (rf && !lf && !lb && !rb) {
        rotationMode = RotationMode::ROTATE_CCW;
        isMoving = false;
        velocity = Point(0, 0);
    }
    else if (lf && !rf && !lb && !rb) {
        rotationMode = RotationMode::ROTATE_CW;
        isMoving = false;
        velocity = Point(0, 0);
    }
    else if (rf && lb && !lf && !rb) {
        rotationMode = RotationMode::ROTATE_CCW;
        isMoving = false;
        velocity = Point(0, 0);
    }
    else if (lf && rb && !rf && !lb) {
        rotationMode = RotationMode::ROTATE_CW;
        isMoving = false;
        velocity = Point(0, 0);
    }
    else if (lf && rf) {
        // move forward
        rotationMode = RotationMode::NONE;
        velocity = getPointFromDir(cannonDir);
        isMoving = true;
    }
    else if (lb && rb) {
        // move backward
        rotationMode = RotationMode::NONE;
        Point d = getPointFromDir(cannonDir);
        velocity = Point(-d.getX(), -d.getY());
        isMoving = true;
    }

    // 3) Apply continuous rotation if any
    if (rotationMode == RotationMode::ROTATE_CW) {
        int idx = (static_cast<int>(cannonDir) + 1) % 8;
        GameConfig::cannonDir nextDir = static_cast<GameConfig::cannonDir>(idx);
        if (canRotateTo(nextDir, board)) {
            cannonDir = nextDir;
        }
    }
    else if (rotationMode == RotationMode::ROTATE_CCW) {
        int idx = (static_cast<int>(cannonDir) + 7) % 8;
        GameConfig::cannonDir nextDir = static_cast<GameConfig::cannonDir>(idx);
        if (canRotateTo(nextDir, board)) {
            cannonDir = nextDir;
        }
    }


}

bool Tank::canRotateTo(GameConfig::cannonDir newDir, char board[GameConfig::HEIGHT][GameConfig::WIDTH]) const {
    Point offset = getPointFromDir(newDir);
    Point cannonTip = pos.add(offset);

    int x = cannonTip.getX();
    int y = cannonTip.getY();


    if (x < 0 || x >= GameConfig::WIDTH || y < 0 || y >= GameConfig::HEIGHT)
        return false;

    char tile = board[y][x];
    return (tile != '#' && tile != '%'&& tile != '&');
}



void Tank::checkKeysP2(Player* p2, char board[GameConfig::HEIGHT][GameConfig::WIDTH], const std::set<char>& keys){
bool lf = keys.count('U');
bool rf = keys.count('O');
bool lb = keys.count('J');
bool rb = keys.count('L');
bool stay = keys.count('K');




    // 1) STOP immediately on stay
    if (stay) {
        isMoving = false;
        velocity = Point(0, 0);
        rotationMode = RotationMode::NONE;
        return;
    }

    // 2) Detect rotation mode changes
    if (rf && !lf && !lb && !rb) {
        rotationMode = RotationMode::ROTATE_CCW;
        isMoving = false;
        velocity = Point(0, 0);
    }
    else if (lf && !rf && !lb && !rb) {
        rotationMode = RotationMode::ROTATE_CW;
        isMoving = false;
        velocity = Point(0, 0);
    }
    else if (rf && lb && !lf && !rb) {
        rotationMode = RotationMode::ROTATE_CCW;
        isMoving = false;
        velocity = Point(0, 0);
    }
    else if (lf && rb && !rf && !lb) {
        rotationMode = RotationMode::ROTATE_CW;
        isMoving = false;
        velocity = Point(0, 0);
    }
    else if (lf && rf) {
        // move forward
        rotationMode = RotationMode::NONE;
        velocity = getPointFromDir(cannonDir);
        isMoving = true;
    }
    else if (lb && rb) {
        // move backward
        rotationMode = RotationMode::NONE;
        Point d = getPointFromDir(cannonDir);
        velocity = Point(-d.getX(), -d.getY());
        isMoving = true;
    }

    // 3) Apply continuous rotation if any
    if (rotationMode == RotationMode::ROTATE_CW) {
        int idx = (static_cast<int>(cannonDir) + 1) % 8;
        GameConfig::cannonDir nextDir = static_cast<GameConfig::cannonDir>(idx);
        if (canRotateTo(nextDir, board)) {
            cannonDir = nextDir;
        }
    }
    else if (rotationMode == RotationMode::ROTATE_CCW) {
        int idx = (static_cast<int>(cannonDir) + 7) % 8;
        GameConfig::cannonDir nextDir = static_cast<GameConfig::cannonDir>(idx);
        if (canRotateTo(nextDir, board)) {
            cannonDir = nextDir;
        }
    }

    // 4) Move if flagged
    if (isMoving) {
        move(velocity.getX(), velocity.getY(), board);
    }
}



void Tank::drawShells(char board[GameConfig::HEIGHT][GameConfig::WIDTH], bool colorMode, int& colorStep) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    static vector<WORD> shellColors = {
        FOREGROUND_RED | FOREGROUND_INTENSITY,
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    };

    for (int i = 0; i < MAX_SHELLS; ++i) {
        if (shellActive[i]) {
            const Point& pos = shellPositions[i];

            int x = pos.getX() - GameConfig::MINX;
            int y = pos.getY() - GameConfig::MINY;

            if (x >= 0 && x < BOARD_COLS && y >= 0 && y < BOARD_ROWS) {
                if (colorMode) {
                    SetConsoleTextAttribute(hConsole, shellColors[colorStep % shellColors.size()]);
                }

                gotoxy(GameConfig::MINX + x, GameConfig::MINY + y);
                cout << '*';

                if (colorMode) {
                    SetConsoleTextAttribute(hConsole, 7); // reset
                }

                colorStep++; // next shell gets next color
            }
        }
    }
}




