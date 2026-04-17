#include "Level.h"
#include "Grid.h"

#include <QRandomGenerator>

// Default map is 6×6; difficulty numbers scale a bit with LevelNum for later floors.
Level::Level(int LevelNum): LevelNum(LevelNum),Rows(6),Cols(6),grid(Rows, Cols),potionNum(0),equipmentNum(0){


}

bool Level::hasEnemies() const {
    return !enemies.empty();
}

// "Completed" here means all foes are dead (the win screen also checks you reached the exit).
bool Level::isCompleted() const {
    return enemies.empty();
}

// Bottom-right corner is the level exit in this project.
bool Level::isExitCell(int row, int col) const {
    return row == Rows - 1 && col == Cols - 1;
}

void Level::resetLevel() {
    generateLevel();
}

// Wipe the grid, then roll random loot and enemies on each tile (except start and exit).
void Level::generateLevel() {
    enemies.clear();
    potionNum = 0;
    equipmentNum = 0;

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



    for (int r = 0; r < Rows; r++) {
        for (int c = 0; c < Cols; c++) {
            if ((r == 0 && c == 0) || (r == Rows - 1 && c == Cols - 1)) {
                continue;
            }

            Cell& cell = grid.getCell(r, c);
            int randValue = QRandomGenerator::global()->bounded(100);

            if (randValue >= 80&&potionNum<6) {
                cell.hasPotion = true;
                potionNum++;
            }
            else if (randValue >= 60&&randValue <80&& equipmentNum<6) {
                cell.hasEquipment= true;
                equipmentNum++;
            }
            else if (randValue >= 40&&randValue<60&&enemies.size()<=30) {
                cell.hasEnemy = true;
            }

            if (cell.hasEnemy) {
                addEnemy(enemyNames[QRandomGenerator::global()->bounded(static_cast<int>(enemyNames.size()))], r, c);
            }
        }
    }
}

// Roll stats from level-based ranges and store one Enemy at (r,c); name and type both use the type string for now.
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

Grid& Level::getGrid() {
    return grid;
}



std::vector<Enemy>& Level::getEnemies() {
    return enemies;
}
int Level::getRows() const {
    return Rows;
}

int Level::getCols() const {
    return Cols;
}
