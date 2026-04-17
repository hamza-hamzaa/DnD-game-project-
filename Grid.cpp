#include "Grid.h"

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

bool Grid::isValidMove(int r, int c) const {
    return isInside(r, c);
}

int Grid::getRows() const {
    return rows;
}

int Grid::getCols() const {
    return cols;
}
