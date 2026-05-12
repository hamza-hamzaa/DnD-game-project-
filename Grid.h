#ifndef GRID_H
#define GRID_H

#include <vector>


using namespace std;
struct Cell{
    bool hasEnemy = false;
    bool hasPotion = false;
    bool hasEquipment = false;
    bool hasVisibleTrap = false;
    bool hasInvisibleTrap = false;
    bool hasKey = false;
    bool visited = false;
    bool wallTop = false;
    bool wallBottom = false;
    bool wallLeft = false;
    bool wallRight = false;

    void clearEvent() {
        hasEnemy = false;
        hasPotion = false;
        hasEquipment = false;
        hasVisibleTrap = false;
        hasInvisibleTrap = false;
        hasKey = false;
    }
};

class Grid {
private:

    int rows;
    int cols;
    vector<vector<Cell>> cells;

public:
    Grid(int r, int c);
    Cell& getCell(int r, int c);
    const Cell& getCell(int r, int c) const;
    bool isValidMove(int r, int c) const;
    bool isInside(int r, int c) const;
    bool isWallBetween(int r1, int c1, int r2, int c2) const;
    int getRows() const;
    int getCols() const;
};
#endif // GRID_H
