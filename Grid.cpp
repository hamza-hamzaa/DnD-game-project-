#include "Grid.h"
#include <cstdlib>

Grid::Grid(int r, int c)  : rows(r), cols(c), cells(rows, std::vector<Cell>(cols))
{
}

Cell& Grid::getCell(int r, int c) {
    return cells[r][c];
}

const Cell& Grid::getCell(int r, int c) const {
    return cells[r][c];
}

bool Grid::isInside(int r, int c) const {
    return (r >= 0 && r < rows && c >= 0 && c < cols);
}

bool Grid::isWallBetween(int r1, int c1, int r2, int c2) const {
    if (!isInside(r1, c1) || !isInside(r2, c2)) {
        return true;
    }
    const int dr = r2 - r1;
    const int dc = c2 - c1;
    if (std::abs(dr) + std::abs(dc) != 1) {
        return true;
    }
    if (dr == 0 && dc == 1) {
        return getCell(r1, c1).wallRight;
    }
    if (dr == 0 && dc == -1) {
        return getCell(r1, c1).wallLeft;
    }
    if (dr == 1 && dc == 0) {
        return getCell(r1, c1).wallBottom;
    }
    if (dr == -1 && dc == 0) {
        return getCell(r1, c1).wallTop;
    }
    return true;
}

bool Grid::isValidMove(int r, int c) const {
    return isInside(r, c);
}

int Grid::getRows() const {
    return rows;
}

int Grid::getCols() const {
    return cols;
}
