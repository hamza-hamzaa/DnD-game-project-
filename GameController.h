#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "Level.h"
#include "player.h"
#include <QString>
#include <istream>

class GameController {
private:
    int LevelNumber;
    Level* level;
    Player* player;
    bool levelStarted = false;
    bool pendingEnemyPhase = false;
    qint64 runAnchorEpochMs = 0;
    quint64 victoryElapsedMs = 0;

    bool isEnemyAtCell(int row, int col, int ignoreIndex) const;
    QString moveEnemies();

    bool deserializeAfterHeader(std::istream& in, int levelNum, qint64 runElapsed, quint32 levelSeed,
                                int racialPeriod, int movesRacial, int dwarfTurns, int dwarfBonus,
                                int row, int col, int hp, int atk, int def, bool hasKey);

public:
    bool isMoveBlockedByWall(int fromRow, int fromCol, int toRow, int toCol) const;

    GameController(Player* p);
    ~GameController();

    void startGame();
    void loadLevel();

    QString resolvePlayerTurn(int dx, int dy);
    QString resolveEnemyTurn();
    bool hasPendingEnemyPhase() const { return pendingEnemyPhase; }

    QString handleCellEvent();
    void giveLoot();
    void nextLevel();
    void restartLevel();

    bool checkWin();
    bool checkLose();
    bool isLevelStarted() const;
    Level* getLevel();
    int getLevelNumber();

    qint64 elapsedMs() const;
    quint64 victoryTimeMs() const { return victoryElapsedMs; }

    QString tryRacialAbility();

    bool saveGameToFile(const QString& path) const;
    static bool loadGameFromFile(const QString& path, Player*& outPlayer, GameController*& outGc, QString& errMsg);
};

#endif // GAMECONTROLLER_H
