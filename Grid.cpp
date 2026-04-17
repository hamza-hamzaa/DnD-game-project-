#include "Grid.h"

// Build an empty rows×cols board of cells.
Grid::Grid(int r, int c)  : rows(r), cols(c), cells(rows, std::vector<Cell>(cols))
{
}

// Writable access to one tile (caller must keep row/col in range).
Cell& Grid::getCell(int r, int c) {
    return cells[r][c];
}

const Cell& Grid::getCell(int r, int c) const {
    return cells[r][c];
}

bool Grid::isInside(int r, int c) const {
    return (r >= 0 && r < rows && c >= 0 && c < cols);
}

// Right now "valid move" just means "on the board"; walls are handled in GameController.
bool Grid::isValidMove(int r, int c) const {
    return isInside(r, c);
}

int Grid::getRows() const {
    return rows;
}

int Grid::getCols() const {
    return cols;
}
