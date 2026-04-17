#ifndef GRID_H
#define GRID_H

#include <vector>

// One map tile: what is sitting on it (enemy, potion, gear, trap) and unused wall flags for future use.
// Kept as a struct because it is mostly plain data with one tiny reset helper.
struct Cell{
    bool hasEnemy=false;
    bool hasPotion=false;
    bool hasEquipment=false;
    bool hasTrap=false;
    bool visited=false;
    bool wallTop= false;
    bool wallBottom=false;
    bool wallLeft=false;
    bool wallRight=false;

    void clearEvent() {
        hasEnemy = false;
        hasPotion = false;
        hasEquipment = false;
        hasTrap = false;
    }
};

// Fixed-size 2D board of Cells; knows width/height and whether a coordinate is in bounds.
class Grid {
private:

    int rows;
    int cols;
    std::vector<std::vector<Cell>> cells;

public:
    Grid(int r, int c);
    Cell& getCell(int r, int c);
    const Cell& getCell(int r, int c) const;
    bool isValidMove(int r, int c) const;
    bool isInside(int r, int c) const;
    int getRows() const;
    int getCols() const;
};
#endif // GRID_H
