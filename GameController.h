#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QVector>

#include "Level.h"
#include "player.h"

class MainWindow;

// Glue between the UI and the rules: loads levels, moves the hero, runs enemy AI, combat triggers, and keyboard input.
class GameController : public QObject {
    Q_OBJECT
private:
    int LevelNumber;
    Level* level;
    Player* player;
    bool levelStarted = false;
    MainWindow* mw = nullptr;

    bool isMoveBlockedByWall(int fromRow, int fromCol, int toRow, int toCol) const;



public:
    // One wall is an edge between two neighboring cells (used to block movement).
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
    QString moveEnemy();
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
