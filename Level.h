#ifndef LEVEL_H
#define LEVEL_H
#include <QRandomGenerator>
#include <QString>
#include <iosfwd>
#include <vector>
#include "Grid.h"
#include "enemy.h"

class Level {
private:
    int LevelNum;
    int Rows;
    int Cols;
    Grid grid;
    std::vector<Enemy> enemies;
    std::vector<QString> enemyNames = {"Goblin", "Orc", "Skeleton"};
    int potionNum = 0;
    int equipmentNum = 0;
    quint32 lastSeed = 0;

    void generateLevelInternal(QRandomGenerator& rng);

public:
    Level(int levelNum);

    void generateLevel(quint32 forcedSeed = 0);
    void resetLevel();

    bool hasEnemies() const;
    bool isCompleted() const;
    bool isExitCell(int row, int col) const;
    void addEnemy(const QString& type, int r, int c, QRandomGenerator& rng);
    void addEnemyFixed(const QString& type, const QString& name, int hp, int atk, int def, int r, int c);

    Grid& getGrid();
    std::vector<Enemy>& getEnemies();
    const std::vector<Enemy>& getEnemies() const { return enemies; }

    int getRows() const;
    int getCols() const;
    int getDungeonLevel() const { return LevelNum; }
    quint32 getLastSeed() const { return lastSeed; }

    void writeState(std::ostream& os) const;
    bool readState(std::istream& is);
};

#endif // LEVEL_H
