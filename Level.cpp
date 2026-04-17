#include "Level.h"
#include"Grid.h"

#include <QRandomGenerator>
#include <algorithm>

Level::Level(int LevelNum, int rows, int cols)
    : LevelNum(LevelNum), Rows(rows), Cols(cols), grid(Rows, Cols), potionNum(0), equipmentNum(0){


}

bool Level::hasEnemies() const {
    return !enemies.empty();
}

bool Level::isCompleted() const {
    return enemies.empty();
}

bool Level::isExitCell(int row, int col) const {
    return row == Rows - 1 && col == Cols - 1;
}

void Level::resetLevel() {
    generateLevel();
}

void Level::generateLevel() {
    enemies.clear();
    potionNum = 0;
    equipmentNum = 0;
    const int cellCount = Rows * Cols;
    const int maxPotions = std::max(2, cellCount / 8);
    const int maxEquipment = std::max(1, cellCount / 10);
    const int maxEnemies = std::max(2, LevelNum + cellCount / 6);

    for (int r = 0; r < Rows; r++) {
        for (int c = 0; c < Cols; c++) {
            Cell& cell = grid.getCell(r, c);
            cell.visited = false;
            cell.hasEnemy = false;
            cell.hasPotion = false;
            cell.hasEquipment = false;
            cell.hasTrap = false;
            cell.wallTop = false;
            cell.wallBottom = false;
            cell.wallLeft = false;
            cell.wallRight = false;
        }
    }

    grid.getCell(0, 0).clearEvent();
    grid.getCell(Rows - 1, Cols - 1).clearEvent();

    for (int r = 0; r < Rows; r++) {
        for (int c = 0; c < Cols; c++) {
            if ((r == 0 && c == 0) || (r == Rows - 1 && c == Cols - 1)) {
                continue;
            }

            Cell& cell = grid.getCell(r, c);
            int randValue = QRandomGenerator::global()->bounded(100);

            if (randValue >= 80 && potionNum < maxPotions) {
                cell.hasPotion = true;
                potionNum++;
            }
            else if (randValue >= 60 && randValue < 80 && equipmentNum < maxEquipment) {
                cell.hasEquipment= true;
                equipmentNum++;
            }
            else if (randValue >= 40 && randValue < 60 && enemies.size() < static_cast<size_t>(maxEnemies)) {
                cell.hasEnemy = true;
            }

            if (cell.hasEnemy) {
                addEnemy(enemyNames[QRandomGenerator::global()->bounded(static_cast<int>(enemyNames.size()))], r, c);
            }
        }
    }

    // Guarantee at least one trap in regular levels.
    if (Rows >= 4 && Cols >= 4) {
        bool placedTrap = false;
        for (int attempt = 0; attempt < 100 && !placedTrap; attempt++) {
            const int r = QRandomGenerator::global()->bounded(Rows);
            const int c = QRandomGenerator::global()->bounded(Cols);
            if ((r == 0 && c == 0) || (r == Rows - 1 && c == Cols - 1)) {
                continue;
            }
            Cell& trapCell = grid.getCell(r, c);
            if (!trapCell.hasEnemy && !trapCell.hasPotion && !trapCell.hasEquipment) {
                trapCell.hasTrap = true;
                placedTrap = true;
            }
        }
    }
}

void Level::addEnemy(const QString& type, int r, int c) {
    int hpMin = 20 + (LevelNum - 1) * 10;
    int hpMax = 30 + (LevelNum - 1) * 10;

    int atkMin = 1 + (LevelNum - 1) * 4;
    int atkMax = 4 + (LevelNum - 1) * 5;

    int defMin = LevelNum - 1;
    int defMax = LevelNum + 1;

    int hp = QRandomGenerator::global()->bounded(hpMin, hpMax + 1);
    int atk = QRandomGenerator::global()->bounded(atkMin, atkMax + 1);
    int def = QRandomGenerator::global()->bounded(defMin, defMax + 1);

    enemies.push_back( Enemy(type, type, hp, atk, def, r, c) );
}

//Enemy(const QString& name, const QString& type, int health, int attackPower, int defense = 0)
Grid& Level::getGrid() {
    return grid;
}



vector<Enemy>& Level::getEnemies() {
    return enemies;
}
int Level::getRows() const {
    return Rows;
}

int Level::getCols() const {
    return Cols;
}
