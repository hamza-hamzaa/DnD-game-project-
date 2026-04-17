#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QVector>

#include "Level.h"
#include "player.h"

class MainWindow;

class GameController : public QObject {
    Q_OBJECT
private:
    int LevelNumber;
    Level* level;
    Level* mainLevel;
    Player* player;
    bool levelStarted = false;
    bool inTrapChallenge = false;
    int trapReturnRow = 0;
    int trapReturnCol = 0;
    int lastPlayerRow = 0;
    int lastPlayerCol = 0;
    MainWindow* mw = nullptr;

    bool isMoveBlockedByWall(int fromRow, int fromCol, int toRow, int toCol) const;



public:
    struct WallSegment {
        int rowA;
        int colA;
        int rowB;
        int colB;
    };

    explicit GameController(Player* p, MainWindow* owner = nullptr);
    ~GameController();

    void startGame();
    void loadLevel();
    QString movePlayer(int dx, int dy);
    void moveEnemy();
    QString handleCellEvent();
    void giveLoot();
    void nextLevel();
    void restartLevel();

    bool checkWin();
    bool checkLose();
    bool isLevelStarted() const ;
    Level* getLevel();
    int  getLevelNumber();

    QVector<WallSegment> wallSegmentsForGrid(int rows, int cols) const;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // GAMECONTROLLER_H
