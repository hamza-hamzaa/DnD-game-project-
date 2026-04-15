#ifndef LEVEL_H
#define LEVEL_H
#include <QString>
#include <vector>
#include"Grid.h"
#include"Enemy.h"

class Level {
private:
    int LevelNum;
    int Rows;
    int Cols;
    Grid grid;
    std::vector<Enemy> enemies;
    std::vector<QString> enemyNames = {"Goblin", "Orc", "Skeleton"};


public:
    Level(int levelNum);

    void generateLevel();
    void placeRandomEvents(int r, int c);
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
