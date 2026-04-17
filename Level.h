#ifndef LEVEL_H
#define LEVEL_H
#include <QString>
#include <vector>
#include "Grid.h"
#include "enemy.h"

// One dungeon floor: a grid plus a list of enemies that match "hasEnemy" cells.
// Also tracks how many loot tiles we spawned so we do not overfill the map.
class Level {
private:
    int LevelNum;
    int Rows;
    int Cols;
    Grid grid;
    std::vector<Enemy> enemies;
    std::vector<QString> enemyNames = {"Goblin", "Orc", "Skeleton"};
    int potionNum;
    int equipmentNum;

public:
    Level(int levelNum);

    void generateLevel();
    void resetLevel();

    bool hasEnemies() const;
    bool isCompleted() const;
    bool isExitCell(int row, int col) const;
    void addEnemy(const QString& type, int r, int c);

    Grid& getGrid();
    std::vector<Enemy>& getEnemies();

    int getRows() const;
    int getCols() const;
};

#endif // LEVEL_H
