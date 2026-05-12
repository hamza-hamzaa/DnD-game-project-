#include "Level.h"
#include "Grid.h"

#include <QRandomGenerator>
#include <algorithm>
#include <array>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

void removeWallBetween(Grid& grid, int r, int c, int nr, int nc)
{
    if (nr == r && nc == c + 1) {
        grid.getCell(r, c).wallRight = false;
        grid.getCell(nr, nc).wallLeft = false;
    } else if (nr == r && nc == c - 1) {
        grid.getCell(r, c).wallLeft = false;
        grid.getCell(nr, nc).wallRight = false;
    } else if (nr == r + 1 && nc == c) {
        grid.getCell(r, c).wallBottom = false;
        grid.getCell(nr, nc).wallTop = false;
    } else if (nr == r - 1 && nc == c) {
        grid.getCell(r, c).wallTop = false;
        grid.getCell(nr, nc).wallBottom = false;
    }
}

void dfsCarve(Grid& grid, int rows, int cols, int r, int c, std::vector<std::vector<bool>>& vis,
              QRandomGenerator& rng)
{
    vis[static_cast<size_t>(r)][static_cast<size_t>(c)] = true;

    std::array<std::pair<int, int>, 4> dirs = {{
        {r - 1, c},
        {r + 1, c},
        {r, c - 1},
        {r, c + 1}
    }};

    std::vector<int> order = {0, 1, 2, 3};
    std::shuffle(order.begin(), order.end(), rng);

    for (int k : order) {
        const int nr = dirs[static_cast<size_t>(k)].first;
        const int nc = dirs[static_cast<size_t>(k)].second;
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
            continue;
        }
        if (vis[static_cast<size_t>(nr)][static_cast<size_t>(nc)]) {
            continue;
        }
        removeWallBetween(grid, r, c, nr, nc);
        dfsCarve(grid, rows, cols, nr, nc, vis, rng);
    }
}

void addMazeLoops(Grid& grid, int rows, int cols, QRandomGenerator& rng)
{
    std::vector<std::array<int, 4>> edges;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (c + 1 < cols && grid.getCell(r, c).wallRight) {
                edges.push_back({r, c, r, c + 1});
            }
            if (r + 1 < rows && grid.getCell(r, c).wallBottom) {
                edges.push_back({r, c, r + 1, c});
            }
        }
    }

    std::shuffle(edges.begin(), edges.end(), rng);

    int removeCount = static_cast<int>(edges.size()) * 15 / 100;
    removeCount = std::max(4, removeCount);
    removeCount = std::min(removeCount, static_cast<int>(edges.size()));
    for (int i = 0; i < removeCount && i < static_cast<int>(edges.size()); i++) {
        const auto& e = edges[static_cast<size_t>(i)];
        removeWallBetween(grid, e[0], e[1], e[2], e[3]);
    }
}

} // namespace

Level::Level(int LevelNum)
    : LevelNum(LevelNum)
    , Rows(5 + LevelNum)
    , Cols(5 + LevelNum)
    , grid(Rows, Cols)
    , potionNum(0)
    , equipmentNum(0)
{
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

void Level::generateLevel(quint32 forcedSeed) {
    if (forcedSeed != 0) {
        lastSeed = forcedSeed;
    } else {
        lastSeed = QRandomGenerator::global()->generate();
    }
    QRandomGenerator rng(lastSeed);
    generateLevelInternal(rng);
}

void Level::generateLevelInternal(QRandomGenerator& rng) {
    enemies.clear();
    potionNum = 0;
    equipmentNum = 0;

    for (int r = 0; r < Rows; r++) {
        for (int c = 0; c < Cols; c++) {
            Cell& cell = grid.getCell(r, c);
            cell.clearEvent();
            cell.visited = false;
            cell.wallTop = true;
            cell.wallBottom = true;
            cell.wallLeft = true;
            cell.wallRight = true;
        }
    }

    std::vector<std::vector<bool>> vis(static_cast<size_t>(Rows),
                                       std::vector<bool>(static_cast<size_t>(Cols), false));
    dfsCarve(grid, Rows, Cols, 0, 0, vis, rng);
    addMazeLoops(grid, Rows, Cols, rng);

    grid.getCell(0, 0).clearEvent();
    grid.getCell(Rows - 1, Cols - 1).clearEvent();

    std::vector<std::pair<int, int>> cells;
    cells.reserve(static_cast<size_t>(Rows * Cols));
    for (int r = 0; r < Rows; r++) {
        for (int c = 0; c < Cols; c++) {
            if ((r == 0 && c == 0) || (r == Rows - 1 && c == Cols - 1)) {
                continue;
            }
            cells.push_back({r, c});
        }
    }

    std::shuffle(cells.begin(), cells.end(), rng);

    size_t idx = 0;
    if (!cells.empty()) {
        const int kr = cells[idx].first;
        const int kc = cells[idx].second;
        grid.getCell(kr, kc).hasKey = true;
        ++idx;
    }

    const int roomForEnemies = static_cast<int>(cells.size() - idx);
    int maxEnemies = 3 + LevelNum * 2 + (Rows + Cols) / 5 + LevelNum / 2;
    maxEnemies = std::clamp(maxEnemies, 3, std::max(3, roomForEnemies));
    int placedEnemies = 0;
    while (placedEnemies < maxEnemies && idx < cells.size()) {
        const int r = cells[idx].first;
        const int c = cells[idx].second;
        ++idx;

        const QString type = enemyNames[rng.bounded(static_cast<int>(enemyNames.size()))];
        addEnemy(type, r, c, rng);
        grid.getCell(r, c).hasEnemy = true;
        ++placedEnemies;
    }

    const int trapBias = std::min(12, LevelNum * 2);
    for (; idx < cells.size(); ++idx) {
        const int r = cells[idx].first;
        const int c = cells[idx].second;
        Cell& cell = grid.getCell(r, c);

        const int roll = rng.bounded(100);

        if (roll < 9 + trapBias / 2) {
            cell.hasVisibleTrap = true;
        } else if (roll < 18 + trapBias) {
            cell.hasInvisibleTrap = true;
        } else if (roll < 38 && potionNum < 7) {
            cell.hasPotion = true;
            ++potionNum;
        } else if (roll < 54 && equipmentNum < 7) {
            cell.hasEquipment = true;
            ++equipmentNum;
        } else if (roll < 64 + trapBias / 3) {
            cell.hasVisibleTrap = true;
        } else if (roll < 74 + trapBias / 2) {
            cell.hasInvisibleTrap = true;
        }
    }

    if (LevelNum == 5) {
        std::vector<std::pair<int, int>> bossSpots;
        for (int r = 0; r < Rows; r++) {
            for (int c = 0; c < Cols; c++) {
                if ((r == 0 && c == 0) || isExitCell(r, c)) {
                    continue;
                }
                const Cell& z = grid.getCell(r, c);
                if (!z.hasEnemy && !z.hasKey && !z.hasPotion && !z.hasEquipment
                    && !z.hasVisibleTrap && !z.hasInvisibleTrap) {
                    bossSpots.push_back({r, c});
                }
            }
        }
        if (!bossSpots.empty()) {
            const int bi = rng.bounded(static_cast<int>(bossSpots.size()));
            const int br = bossSpots[static_cast<size_t>(bi)].first;
            const int bc = bossSpots[static_cast<size_t>(bi)].second;
            const int bhp = 110 + LevelNum * 14;
            const int batk = 9 + LevelNum * 3;
            const int bdef = 3 + LevelNum;
            addEnemyFixed(QStringLiteral("Lich"), QStringLiteral("Grave Tyrant"), bhp, batk, bdef, br, bc);
            grid.getCell(br, bc).hasEnemy = true;
        }
    }
}

void Level::addEnemy(const QString& type, int r, int c, QRandomGenerator& rng) {
    int hpMin = 26 + (LevelNum - 1) * 12;
    int hpMax = 40 + (LevelNum - 1) * 12;

    int atkMin = 2 + (LevelNum - 1) * 5;
    int atkMax = 7 + (LevelNum - 1) * 6;

    int defMin = LevelNum;
    int defMax = LevelNum + 2;

    int hp = rng.bounded(hpMin, hpMax + 1);
    int atk = rng.bounded(atkMin, atkMax + 1);
    int def = rng.bounded(defMin, defMax + 1);

    enemies.push_back(Enemy(type, type, hp, atk, def, r, c));
}

void Level::addEnemyFixed(const QString& type, const QString& name, int hp, int atk, int def, int r, int c)
{
    enemies.push_back(Enemy(name, type, hp, atk, def, r, c));
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

void Level::writeState(std::ostream& os) const
{
    os << "ROWS " << Rows << "\n";
    os << "COLS " << Cols << "\n";
    os << "LEVEL " << LevelNum << "\n";
    for (int r = 0; r < Rows; r++) {
        for (int c = 0; c < Cols; c++) {
            const Cell& cell = grid.getCell(r, c);
            os << "C " << r << ' ' << c << ' '
               << (cell.wallTop ? 1 : 0) << ' '
               << (cell.wallRight ? 1 : 0) << ' '
               << (cell.wallBottom ? 1 : 0) << ' '
               << (cell.wallLeft ? 1 : 0) << ' '
               << (cell.visited ? 1 : 0) << ' '
               << (cell.hasEnemy ? 1 : 0) << ' '
               << (cell.hasPotion ? 1 : 0) << ' '
               << (cell.hasEquipment ? 1 : 0) << ' '
               << (cell.hasKey ? 1 : 0) << ' '
               << (cell.hasVisibleTrap ? 1 : 0) << ' '
               << (cell.hasInvisibleTrap ? 1 : 0) << '\n';
        }
    }
    os << "ENEMIES " << enemies.size() << '\n';
    for (const Enemy& e : enemies) {
        QString enc = e.getName();
        enc.replace(' ', '_');
        os << "E " << enc.toStdString() << ' ' << e.getType().toStdString() << ' '
           << e.getHealth() << ' ' << e.getAttackPower() << ' ' << e.getDefense() << ' '
           << e.getRow() << ' ' << e.getCol() << '\n';
    }
}

bool Level::readState(std::istream& is)
{
    std::string line;
    int rows = 0;
    int cols = 0;
    int lvl = 0;
    for (int header = 0; header < 3; ++header) {
        if (!std::getline(is, line)) {
            return false;
        }
        std::istringstream ls(line);
        std::string tag;
        ls >> tag;
        if (tag == "ROWS") {
            ls >> rows;
        } else if (tag == "COLS") {
            ls >> cols;
        } else if (tag == "LEVEL") {
            ls >> lvl;
        }
    }
    if (rows != Rows || cols != Cols || lvl != LevelNum) {
        return false;
    }

    enemies.clear();
    const int expectedCells = Rows * Cols;
    for (int i = 0; i < expectedCells; ++i) {
        if (!std::getline(is, line)) {
            return false;
        }
        if (line.empty() || line[0] != 'C') {
            return false;
        }
        std::istringstream ls(line);
        char ch;
        int r, c;
        int wT, wR, wB, wL, vis, he, hp, eq, k, vt, it;
        ls >> ch >> r >> c >> wT >> wR >> wB >> wL >> vis >> he >> hp >> eq >> k >> vt >> it;
        if (r < 0 || r >= Rows || c < 0 || c >= Cols) {
            return false;
        }
        Cell& cell = grid.getCell(r, c);
        cell.wallTop = wT != 0;
        cell.wallRight = wR != 0;
        cell.wallBottom = wB != 0;
        cell.wallLeft = wL != 0;
        cell.visited = vis != 0;
        cell.hasEnemy = he != 0;
        cell.hasPotion = hp != 0;
        cell.hasEquipment = eq != 0;
        cell.hasKey = k != 0;
        cell.hasVisibleTrap = vt != 0;
        cell.hasInvisibleTrap = it != 0;
    }

    if (!std::getline(is, line)) {
        return false;
    }
    int enemyCount = 0;
    {
        std::istringstream ls(line);
        std::string tag;
        ls >> tag >> enemyCount;
        if (tag != "ENEMIES") {
            return false;
        }
    }
    for (int i = 0; i < enemyCount; i++) {
        if (!std::getline(is, line)) {
            return false;
        }
        std::istringstream ls(line);
        char ch;
        std::string nameEnc, typ;
        int hpV, atkV, defV, rr, cc;
        ls >> ch >> nameEnc >> typ >> hpV >> atkV >> defV >> rr >> cc;
        if (ch != 'E') {
            return false;
        }
        QString qname = QString::fromStdString(nameEnc).replace('_', ' ');
        QString qtype = QString::fromStdString(typ);
        enemies.push_back(Enemy(qname, qtype, hpV, atkV, defV, rr, cc));
    }
    return true;
}
