#include "GameController.h"
#include <QDateTime>
#include <QRandomGenerator>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

#include "enemy.h"

namespace {

QString normalizeEnemyType(const QString& t)
{
    return t.trimmed();
}

void buildMoveOrderForEnemy(const QString& typeIn, int rowDiff, int colDiff,
                            std::vector<std::pair<int, int>>& outDirs)
{
    const QString type = normalizeEnemyType(typeIn);
    outDirs.clear();

    if (type == QStringLiteral("Orc")) {
        if (colDiff != 0) {
            outDirs.push_back({0, colDiff > 0 ? 1 : -1});
        }
        if (rowDiff != 0) {
            outDirs.push_back({rowDiff > 0 ? 1 : -1, 0});
        }
        outDirs.push_back({0, colDiff > 0 ? -1 : 1});
        outDirs.push_back({rowDiff > 0 ? -1 : 1, 0});
        outDirs.push_back({-1, 0});
        outDirs.push_back({1, 0});
        outDirs.push_back({0, -1});
        outDirs.push_back({0, 1});
    } else if (type == QStringLiteral("Skeleton")) {
        outDirs = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        std::shuffle(outDirs.begin(), outDirs.end(), *QRandomGenerator::global());
    } else {
        int firstDx = 0;
        int firstDy = 0;
        int secondDx = 0;
        int secondDy = 0;
        if (std::abs(rowDiff) >= std::abs(colDiff)) {
            if (rowDiff != 0) {
                firstDx = rowDiff > 0 ? 1 : -1;
            }
            if (colDiff != 0) {
                secondDy = colDiff > 0 ? 1 : -1;
            }
        } else {
            if (colDiff != 0) {
                firstDy = colDiff > 0 ? 1 : -1;
            }
            if (rowDiff != 0) {
                secondDx = rowDiff > 0 ? 1 : -1;
            }
        }
        if (firstDx != 0 || firstDy != 0) {
            outDirs.push_back({firstDx, firstDy});
        }
        if (secondDx != 0 || secondDy != 0) {
            outDirs.push_back({secondDx, secondDy});
        }
        const std::vector<std::pair<int, int>> rest = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (const auto& p : rest) {
            bool dup = false;
            for (const auto& q : outDirs) {
                if (q.first == p.first && q.second == p.second) {
                    dup = true;
                    break;
                }
            }
            if (!dup) {
                outDirs.push_back(p);
            }
        }
    }
}

} // namespace

bool GameController::isMoveBlockedByWall(int fromRow, int fromCol, int toRow, int toCol) const
{
    if (!level) {
        return false;
    }
    return level->getGrid().isWallBetween(fromRow, fromCol, toRow, toCol);
}

bool GameController::isEnemyAtCell(int row, int col, int ignoreIndex) const
{
    if (!level) {
        return false;
    }

    std::vector<Enemy>& enemies = level->getEnemies();
    for (int i = 0; i < static_cast<int>(enemies.size()); i++) {
        if (i == ignoreIndex) {
            continue;
        }

        if (enemies[i].isAlive() && enemies[i].getRow() == row && enemies[i].getCol() == col) {
            return true;
        }
    }

    return false;
}

QString GameController::moveEnemies()
{
    if (!level || !player || !player->isAlive()) {
        return QString();
    }

    Grid& grid = level->getGrid();
    std::vector<Enemy>& enemies = level->getEnemies();
    QString log;
    bool playerReached = false;

    for (int i = 0; i < static_cast<int>(enemies.size()); i++) {
        Enemy& enemy = enemies[i];
        if (!enemy.isAlive()) {
            continue;
        }

        const int enemyRow = enemy.getRow();
        const int enemyCol = enemy.getCol();
        const int rowDiff = player->getRow() - enemyRow;
        const int colDiff = player->getCol() - enemyCol;

        if (rowDiff == 0 && colDiff == 0) {
            playerReached = true;
            break;
        }

        std::vector<std::pair<int, int>> dirs;
        buildMoveOrderForEnemy(enemy.getType(), rowDiff, colDiff, dirs);

        bool moved = false;
        for (const auto& d : dirs) {
            const int nextRow = enemyRow + d.first;
            const int nextCol = enemyCol + d.second;

            if ((nextRow == enemyRow && nextCol == enemyCol)
                || !grid.isInside(nextRow, nextCol)
                || isMoveBlockedByWall(enemyRow, enemyCol, nextRow, nextCol)
                || isEnemyAtCell(nextRow, nextCol, i)) {
                continue;
            }

            grid.getCell(enemyRow, enemyCol).hasEnemy = false;
            enemy.setPos(nextRow, nextCol);
            grid.getCell(nextRow, nextCol).hasEnemy = true;
            moved = true;

            if (nextRow == player->getRow() && nextCol == player->getCol()) {
                playerReached = true;
            }
            break;
        }

        if (playerReached) {
            break;
        }

        if (!moved) {
            continue;
        }
    }

    if (playerReached && player->isAlive()) {
        const QString combatLog = handleCellEvent();
        if (!combatLog.isEmpty()) {
            log = "An enemy reaches you!\n" + combatLog;
        }
    }

    return log;
}

GameController::GameController(Player* p)
    : LevelNumber(1)
    , level(nullptr)
    , player(p)
{
}

GameController::~GameController() {
    delete level;
}

void GameController::startGame() {
    LevelNumber = 1;
    player->setHealth(player->getMaxHealth());
    runAnchorEpochMs = QDateTime::currentMSecsSinceEpoch();
    victoryElapsedMs = 0;
    pendingEnemyPhase = false;
    const int period = QRandomGenerator::global()->bounded(4, 13);
    player->setRacialMovePeriod(period);
    loadLevel();
}

void GameController::loadLevel() {
    delete level;
    level = new Level(LevelNumber);
    level->generateLevel();
    player->setPos(0, 0);
    player->setHasLevelKey(false);
    levelStarted = false;
    pendingEnemyPhase = false;
}

QString GameController::resolvePlayerTurn(int dx, int dy)
{
    levelStarted = true;
    pendingEnemyPhase = false;

    int newRow = player->getRow() + dx;
    int newCol = player->getCol() + dy;

    if (!level->getGrid().isInside(newRow, newCol)) {
        return QStringLiteral("Blocked!");
    }
    if (isMoveBlockedByWall(player->getRow(), player->getCol(), newRow, newCol)) {
        return QStringLiteral("Blocked!");
    }

    if (!level->getGrid().isValidMove(newRow, newCol)) {
        return QStringLiteral("Blocked!");
    }

    player->setPos(newRow, newCol);
    player->onSuccessfulMove();
    QString log = handleCellEvent();

    if (!player->isAlive()) {
        return log;
    }

    if (level->isExitCell(player->getRow(), player->getCol())) {
        const bool canDescend = !level->hasEnemies() && player->getHasLevelKey();
        if (canDescend) {
            if (LevelNumber == 5) {
                victoryElapsedMs = static_cast<quint64>(elapsedMs());
                pendingEnemyPhase = false;
                return log.isEmpty() ? QStringLiteral("You conquered the dungeon!")
                                     : log + QStringLiteral("\nYou conquered the dungeon!");
            }

            nextLevel();
            pendingEnemyPhase = false;
            QString tail = log.isEmpty()
                ? QStringLiteral("You reached the exit. Descending to the next level.")
                : log + QStringLiteral("\nYou reached the exit. Descending to the next level.");
            if (LevelNumber == 5) {
                tail += QStringLiteral("\nA grave chill spreads—the Grave Tyrant awaits!");
            }
            return tail;
        }

        QString seal = QStringLiteral("The exit is sealed. ");
        if (level->hasEnemies()) {
            seal += QStringLiteral("Defeat all enemies. ");
        }
        if (!player->getHasLevelKey()) {
            seal += QStringLiteral("Find the dungeon key. ");
        }
        if (!log.isEmpty()) {
            return log + QStringLiteral("\n") + seal;
        }
        return seal;
    }

    pendingEnemyPhase = true;
    return log;
}

QString GameController::resolveEnemyTurn()
{
    pendingEnemyPhase = false;
    if (!level || !player || !player->isAlive()) {
        return QString();
    }
    return moveEnemies();
}

qint64 GameController::elapsedMs() const
{
    if (runAnchorEpochMs == 0) {
        return 0;
    }
    return QDateTime::currentMSecsSinceEpoch() - runAnchorEpochMs;
}

QString GameController::tryRacialAbility()
{
    if (!player || !level) {
        return QString();
    }
    if (!player->isRacialAbilityReady()) {
        return QStringLiteral("Racial ability not ready yet. Keep exploring.");
    }

    const QString race = player->getRace().trimmed();
    QString msg;

    if (race == QStringLiteral("Human")) {
        const int heal = player->getMaxHealth() * 35 / 100;
        player->heal(heal);
        msg = QStringLiteral("Human — Second Wind! You recover ") + QString::number(heal) + QStringLiteral(" HP.");
    } else if (race == QStringLiteral("Elf")) {
        int revealed = 0;
        for (int r = 0; r < level->getRows(); r++) {
            for (int c = 0; c < level->getCols(); c++) {
                Cell& cell = level->getGrid().getCell(r, c);
                if (cell.hasInvisibleTrap) {
                    cell.hasInvisibleTrap = false;
                    cell.hasVisibleTrap = true;
                    ++revealed;
                }
            }
        }
        msg = QStringLiteral("Elf — Keen Senses! You revealed ") + QString::number(revealed)
            + QStringLiteral(" hidden trap(s).");
    } else if (race == QStringLiteral("Dwarf")) {
        player->applyDwarfStoneblood(10, 8);
        msg = QStringLiteral("Dwarf — Stoneblood! +10 defense for 8 moves.");
    } else {
        msg = QStringLiteral("No racial ability for this race.");
        return msg;
    }

    player->resetRacialProgressAfterUse();
    return msg;
}

QString GameController::handleCellEvent() {
    int row = player->getRow();
    int col = player->getCol();

    Cell& cell = level->getGrid().getCell(row, col);

    const int trapDmg = 8 + level->getDungeonLevel() * 2;

    if (cell.hasEnemy) {
        std::vector<Enemy>& enemies = level->getEnemies();

        for (size_t i = 0; i < enemies.size(); i++) {
            if (enemies[i].getRow() == row && enemies[i].getCol() == col) {
                QString log;
                Enemy& enemy = enemies[i];

                while (player->isAlive() && enemy.isAlive()) {
                    int playerDmg = player->basicAttackDamage();
                    enemy.takeDamage(playerDmg);

                    log += "You hit " + enemy.getName() + " for "
                           + QString::number(playerDmg) + " damage. ";

                    if (!enemy.isAlive()) {
                        log += enemy.getName() + " was defeated!";
                        break;
                    }

                    int enemyDmg = enemy.basicAttackDamage();
                    player->takeDamage(enemyDmg);

                    log += enemy.getName() + " hits back for "
                           + QString::number(enemyDmg) + " damage. ";

                    if (!player->isAlive()) {
                        log += "You were defeated!";
                        break;
                    }

                    log += "\n";
                }

                if (!enemy.isAlive()) {
                    QString ename = enemy.getName();
                    enemies.erase(enemies.begin() + static_cast<long>(i));
                    cell.hasEnemy = false;
                    return log + "\nYou defeated " + ename + "!";
                }

                return log;
            }
        }
    }
    else if (cell.hasPotion) {
        if (player->getHealth() >= player->getMaxHealth()) {
            return "Your HP is already full. Potion left for later.";
        }
        int before = player->getHealth();
        giveLoot();
        int healed = player->getHealth() - before;
        cell.hasPotion = false;
        return "You drank a potion and restored " + QString::number(healed) + " HP!";
    }
    else if (cell.hasEquipment) {
        player->addAttack(3);
        player->addDefense(1);
        cell.hasEquipment = false;
        return "You found equipment! +3 attack, +1 defense.";
    }
    else if (cell.hasKey && !player->getHasLevelKey()) {
        player->setHasLevelKey(true);
        cell.hasKey = false;
        return "You picked up the dungeon key!";
    }
    else if (cell.hasVisibleTrap) {
        player->takeDamage(trapDmg);
        cell.hasVisibleTrap = false;
        return QStringLiteral("You stepped on a spiked trap! Lost ") + QString::number(trapDmg) + QStringLiteral(" HP.");
    }
    else if (cell.hasInvisibleTrap) {
        player->takeDamage(trapDmg);
        cell.hasInvisibleTrap = false;
        return QStringLiteral("A hidden pressure plate snaps! Lost ") + QString::number(trapDmg) + QStringLiteral(" HP.");
    }

    return "";
}

void GameController::giveLoot() {
    int chance = QRandomGenerator::global()->bounded(20);

    if (chance == 19) {
        player->heal(20);
    }
    else {
        player->heal(10);
    }
}

void GameController::nextLevel() {
    LevelNumber++;
    loadLevel();
    player->heal(player->getMaxHealth() / 3);
}

void GameController::restartLevel() {
    player->setHealth(player->getMaxHealth());
    loadLevel();
}

bool GameController::checkWin() {
    return level
           && LevelNumber == 5
           && player->getHealth() > 0
           && level->isExitCell(player->getRow(), player->getCol())
           && !level->hasEnemies()
           && player->getHasLevelKey();
}

bool GameController::checkLose() {
    return player->getHealth() <= 0;
}

Level* GameController::getLevel(){return level;}
int GameController::getLevelNumber(){return LevelNumber;}
bool GameController::isLevelStarted() const { return levelStarted; }

bool GameController::saveGameToFile(const QString& path) const
{
    if (!level || !player) {
        return false;
    }
    std::ofstream out(path.toStdString());
    if (!out) {
        return false;
    }
    out << "VERSION 2\n";
    out << "LEVELNUM " << LevelNumber << "\n";
    out << "RUNELAPSED " << elapsedMs() << "\n";
    out << "LEVELSEED " << level->getLastSeed() << "\n";
    out << "RACIAL_PERIOD " << player->getRacialMovePeriod() << "\n";
    out << "MOVES_RACIAL " << player->getMovesTowardRacial() << "\n";
    out << "DWARF_TURNS " << player->getDwarfArmorTurnsRemaining() << "\n";
    out << "DWARF_BONUS " << player->getDwarfArmorDefenseBonus() << "\n";
    out << "NAME " << player->getName().toStdString() << "\n";
    out << "RACE " << player->getRace().toStdString() << "\n";
    out << "STYLE " << player->getStyle().toStdString() << "\n";
    out << "PLAYER " << player->getRow() << ' ' << player->getCol() << ' '
        << player->getHealth() << ' ' << player->getAttackPower() << ' ' << player->getDefense() << ' '
        << (player->getHasLevelKey() ? 1 : 0) << "\n";
    level->writeState(out);
    return true;
}

bool GameController::deserializeAfterHeader(std::istream& in, int levelNum, qint64 runElapsed, quint32 levelSeed,
                                            int racialPeriod, int movesRacial, int dwarfTurns, int dwarfBonus,
                                            int row, int col, int hp, int atk, int def, bool hasKey)
{
    (void)levelSeed;
    LevelNumber = levelNum;
    delete level;
    level = new Level(LevelNumber);
    if (!level->readState(in)) {
        return false;
    }
    runAnchorEpochMs = QDateTime::currentMSecsSinceEpoch() - runElapsed;
    player->setRacialMovePeriod(racialPeriod);
    player->setMovesTowardRacialForLoad(movesRacial);
    player->restoreDwarfBuffForLoad(dwarfBonus, dwarfTurns);
    player->setPos(row, col);
    player->setHealth(hp);
    player->setAttackPower(atk);
    player->setDefense(def);
    player->setHasLevelKey(hasKey);
    levelStarted = true;
    pendingEnemyPhase = false;
    return true;
}

bool GameController::loadGameFromFile(const QString& path, Player*& outPlayer, GameController*& outGc, QString& errMsg)
{
    std::ifstream in(path.toStdString());
    if (!in) {
        errMsg = QStringLiteral("Could not open save file.");
        return false;
    }
    std::string line;
    int levelNum = 1;
    qint64 runElapsed = 0;
    quint32 levelSeed = 0;
    int racialPeriod = 6;
    int movesRacial = 0;
    int dwarfTurns = 0;
    int dwarfBonus = 0;
    std::string name, race, style;
    int pr = 0, pc = 0, php = 0, patk = 0, pdef = 0, pkey = 0;

    if (!std::getline(in, line) || line != "VERSION 2") {
        errMsg = QStringLiteral("Unsupported save version.");
        return false;
    }
    auto readPair = [&](const std::string& prefix, int& out) -> bool {
        if (!std::getline(in, line)) {
            return false;
        }
        if (line.rfind(prefix, 0) != 0) {
            return false;
        }
        std::istringstream ls(line);
        std::string tag;
        ls >> tag >> out;
        return true;
    };

    if (!readPair("LEVELNUM", levelNum)) {
        errMsg = QStringLiteral("Bad save (LEVELNUM).");
        return false;
    }
    long long re = 0;
    if (!std::getline(in, line)) {
        errMsg = QStringLiteral("Bad save.");
        return false;
    }
    {
        std::istringstream ls(line);
        std::string tag;
        ls >> tag >> re;
        if (tag != "RUNELAPSED") {
            errMsg = QStringLiteral("Bad save (RUNELAPSED).");
            return false;
        }
    }
    runElapsed = re;
    if (!std::getline(in, line)) {
        errMsg = QStringLiteral("Bad save.");
        return false;
    }
    {
        std::istringstream ls(line);
        std::string tag;
        quint32 seedVal = 0;
        ls >> tag >> seedVal;
        if (tag != "LEVELSEED") {
            errMsg = QStringLiteral("Bad save (LEVELSEED).");
            return false;
        }
        levelSeed = seedVal;
    }
    if (!readPair("RACIAL_PERIOD", racialPeriod)) {
        errMsg = QStringLiteral("Bad save.");
        return false;
    }
    if (!readPair("MOVES_RACIAL", movesRacial)) {
        errMsg = QStringLiteral("Bad save.");
        return false;
    }
    if (!readPair("DWARF_TURNS", dwarfTurns)) {
        errMsg = QStringLiteral("Bad save.");
        return false;
    }
    if (!readPair("DWARF_BONUS", dwarfBonus)) {
        errMsg = QStringLiteral("Bad save.");
        return false;
    }
    if (!std::getline(in, line) || line.rfind("NAME ", 0) != 0) {
        errMsg = QStringLiteral("Bad save (NAME).");
        return false;
    }
    name = line.substr(5);
    if (!std::getline(in, line) || line.rfind("RACE ", 0) != 0) {
        errMsg = QStringLiteral("Bad save (RACE).");
        return false;
    }
    race = line.substr(5);
    if (!std::getline(in, line) || line.rfind("STYLE ", 0) != 0) {
        errMsg = QStringLiteral("Bad save (STYLE).");
        return false;
    }
    style = line.substr(6);
    if (!std::getline(in, line) || line.rfind("PLAYER ", 0) != 0) {
        errMsg = QStringLiteral("Bad save (PLAYER).");
        return false;
    }
    {
        std::istringstream ls(line);
        std::string tag;
        ls >> tag >> pr >> pc >> php >> patk >> pdef >> pkey;
    }

    std::ostringstream levelBody;
    levelBody << in.rdbuf();
    std::istringstream levelIn(levelBody.str());

    const QString qname = QString::fromStdString(name);
    const QString qrace = QString::fromStdString(race);
    const QString qstyle = QString::fromStdString(style);

    Player* loadedPlayer = nullptr;
    if (qstyle == QStringLiteral("Warrior")) {
        loadedPlayer = new Warrior(qname, qrace);
    } else if (qstyle == QStringLiteral("Fire Mage")) {
        loadedPlayer = new FireMage(qname, qrace);
    } else if (qstyle == QStringLiteral("Ice Mage")) {
        loadedPlayer = new IceMage(qname, qrace);
    } else {
        errMsg = QStringLiteral("Unknown class in save.");
        return false;
    }

    GameController* loadedGc = new GameController(loadedPlayer);
    if (!loadedGc->deserializeAfterHeader(levelIn, levelNum, runElapsed, levelSeed, racialPeriod, movesRacial,
                                           dwarfTurns, dwarfBonus, pr, pc, php, patk, pdef, pkey != 0)) {
        errMsg = QStringLiteral("Failed to restore dungeon state.");
        delete loadedGc;
        delete loadedPlayer;
        return false;
    }

    delete outGc;
    outGc = nullptr;
    delete outPlayer;
    outPlayer = nullptr;
    outPlayer = loadedPlayer;
    outGc = loadedGc;
    return true;
}
