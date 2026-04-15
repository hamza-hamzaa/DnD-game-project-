#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "Level.h"
#include "player.h"

class GameController {
private:
    int LevelNumber;
    Level* level;
    Player* player;
    bool levelStarted = false;



public:
    GameController(Player* p);
    ~GameController();

    void startGame();
    void loadLevel();
    QString movePlayer(int dx, int dy);
    QString handleCellEvent();
    void giveLoot();
    void nextLevel();
    void restartLevel();

    bool checkWin();
    bool checkLose();
    bool isLevelStarted() const ;
    Level* getLevel();
    int  getLevelNumber();
};

#endif // GAMECONTROLLER_H
