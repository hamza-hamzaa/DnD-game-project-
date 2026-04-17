#include "GameController.h"
#include <QRandomGenerator>
#include <QKeyEvent>
#include <QTimer>
#include <cstdlib>
#include <utility>

#include "Enemy.h"
#include "mainwindow.h"
//constructor that initalizes all variables to a base value
//level initalized to nullpts as startgame func. is what creates the actual level this is so that the functionalities dont overlap
GameController::GameController(Player* p, MainWindow* owner)
    : QObject(owner), LevelNumber(1), level(nullptr), mainLevel(nullptr), player(p), mw(owner)
{
}
//destructor that deletes alldynamically allocated memory in the class
GameController::~GameController() {
    if (level != mainLevel) {
        delete level;
    }
    delete mainLevel;
}
//function only  used in the start of the game and resets the levelnumber to one and creates the inital level
void GameController::startGame() {
    LevelNumber = 1;
    player->setHealth(player->getMaxHealth());
    loadLevel();
}
//function that creates a level object based on the levelnumber and resets player pos
void GameController::loadLevel() {
    if (level != mainLevel) {
        delete level;
    }
    delete mainLevel;
    mainLevel = new Level(LevelNumber, 8, 8);
    level = mainLevel;
    level->generateLevel();
    player->setPos(0, 0);
    levelStarted = false;
    inTrapChallenge = false;
    trapReturnRow = 0;
    trapReturnCol = 0;
    lastPlayerRow = 0;
    lastPlayerCol = 0;
}
//function first changes player pos through a temp var then checks if this new var is a valid move and if so it sets it as the players final position
//return type is a type Qstring as it should be void however it returns a string so that it can interact with the window and type out messages for the player
QString GameController::movePlayer(int dx, int dy) {
    levelStarted = true;
    const int currentRow = player->getRow();
    const int currentCol = player->getCol();
    int newRow = player->getRow() + dx;
    int newCol = player->getCol() + dy;

    if (level->getGrid().isValidMove(newRow, newCol)
        && !isMoveBlockedByWall(currentRow, currentCol, newRow, newCol)) {
        lastPlayerRow = currentRow;
        lastPlayerCol = currentCol;
        player->setPos(newRow, newCol);
        QString log = handleCellEvent();
        moveEnemy();

        if (player->isAlive() && level->isExitCell(player->getRow(), player->getCol())) {
            if (inTrapChallenge) {
                delete level;
                level = mainLevel;
                inTrapChallenge = false;
                player->setPos(trapReturnRow, trapReturnCol);
                const QString trapMsg = "You cleared the trap chamber and returned safely.";
                return log.isEmpty() ? trapMsg : (log + "\n" + trapMsg);
            }

            return log.isEmpty() ? "You cleared level 1! Head to the end screen."
                                 : log + "\nYou cleared level 1! Head to the end screen.";
        }

        return log;
    }
    return "Blocked";

}

//function checks all possibilities states of the cell such as whether it has an enemy potion or anything and based on these of conditions with execute a certain functionality
QString GameController::handleCellEvent() {
    int row = player->getRow();
    int col = player->getCol();

    Cell& cell = level->getGrid().getCell(row, col);

    if (cell.hasEnemy) {
        vector<Enemy>& enemies = level->getEnemies();
        //function first confirms that there is an enemy in this cell then through a full loop it confirms which enemy it is
        for (size_t i = 0; i < enemies.size(); i++) {
            if (enemies[i].getRow() == row && enemies[i].getCol() == col) {
                QString log;
                Enemy& enemy = enemies[i];
                //through a wide loop which checks if both player and the enemy or alive loops over the fight sequence until one person dies
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
    else if (cell.hasTrap) {
        cell.hasTrap = false;
        if (!inTrapChallenge) {
            inTrapChallenge = true;
            trapReturnRow = lastPlayerRow;
            trapReturnCol = lastPlayerCol;
            level = new Level(LevelNumber, 3, 3);
            level->generateLevel();
            player->setPos(0, 0);
            return "A trap pulled you into a 3x3 trap chamber. Reach its exit to return.";
        }
        player->takeDamage(8);
        return "A trap hurt you for 8 HP.";
    }

    return "";
}
//through a random number generator function either he his player for 20 or 10 HP
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
//checks game is won by checking the level number the health and the cell position
bool GameController::checkWin() {
    return level
           && !inTrapChallenge
           && LevelNumber == 1
           && player->getHealth() > 0
           && level->isExitCell(player->getRow(), player->getCol());
}
//checks if game is lost by checking player health
bool GameController::checkLose() {
    return player->getHealth() <= 0;
}
//getter for level variable
Level* GameController::getLevel(){return level;}
//getter for levelNumber variable
int GameController::getLevelNumber(){return LevelNumber;}
//getter for levelstarted variable
bool GameController::isLevelStarted() const { return levelStarted; }
//function similar to move player however you're here it rates over the entire enemies array and checks if the move is valid and search, he has enemy boolen to a correct value
void GameController::moveEnemy(){
    vector<Enemy>& enemies = level->getEnemies();
    const int playerRow = player->getRow();
    const int playerCol = player->getCol();

    for (size_t i = 0; i < enemies.size(); i++) {
        const int enemyRow = enemies[i].getRow();
        const int enemyCol = enemies[i].getCol();
        const int rowDelta = playerRow - enemyRow;
        const int colDelta = playerCol - enemyCol;

        // Try the axis with bigger distance first, then the other axis.
        std::vector<std::pair<int, int>> candidates;
        if (std::abs(rowDelta) >= std::abs(colDelta)) {
            if (rowDelta != 0) candidates.push_back({(rowDelta > 0) ? 1 : -1, 0});
            if (colDelta != 0) candidates.push_back({0, (colDelta > 0) ? 1 : -1});
        } else {
            if (colDelta != 0) candidates.push_back({0, (colDelta > 0) ? 1 : -1});
            if (rowDelta != 0) candidates.push_back({(rowDelta > 0) ? 1 : -1, 0});
        }

        // If direct chase is blocked, try side steps so enemies do not freeze.
        candidates.push_back({1, 0});
        candidates.push_back({-1, 0});
        candidates.push_back({0, 1});
        candidates.push_back({0, -1});

        for (size_t j = 0; j < candidates.size(); j++) {
            const int dx = candidates[j].first;
            const int dy = candidates[j].second;
            const int newRow = enemyRow + dx;
            const int newCol = enemyCol + dy;

            if (newRow == playerRow && newCol == playerCol) {
                continue;
            }

            if (!level->getGrid().isValidMove(newRow, newCol)) {
                continue;
            }

            if (isMoveBlockedByWall(enemyRow, enemyCol, newRow, newCol)) {
                continue;
            }

            Cell& targetCell = level->getGrid().getCell(newRow, newCol);
            if (targetCell.hasEnemy || targetCell.hasPotion || targetCell.hasEquipment) {
                continue;
            }

            level->getGrid().getCell(enemyRow, enemyCol).hasEnemy = false;
            enemies[i].setPos(newRow, newCol);
            level->getGrid().getCell(newRow, newCol).hasEnemy = true;
            break;
        }
    }
}

QVector<GameController::WallSegment> GameController::wallSegmentsForGrid(int rows, int cols) const
{
    QVector<WallSegment> validSegments;

    if (!level || rows != 8 || cols != 8 || inTrapChallenge) {
        return validSegments;
    }

    const QVector<WallSegment> layout = {
        {0, 1, 0, 2}, {0, 4, 0, 5}, {1, 0, 1, 1}, {1, 3, 1, 4}, {1, 5, 1, 6},
        {2, 2, 2, 3}, {2, 4, 2, 5}, {3, 1, 3, 2}, {3, 6, 3, 7}, {4, 0, 4, 1},
        {4, 3, 4, 4}, {4, 5, 4, 6}, {5, 2, 5, 3}, {5, 4, 5, 5}, {6, 1, 6, 2},
        {6, 6, 6, 7}, {7, 3, 7, 4}, {7, 5, 7, 6},
        {0, 2, 1, 2}, {1, 4, 2, 4}, {2, 1, 3, 1}, {2, 5, 3, 5}, {3, 3, 4, 3},
        {4, 2, 5, 2}, {4, 6, 5, 6}, {5, 4, 6, 4}, {6, 0, 7, 0}, {6, 3, 7, 3}
    };

    for (int i = 0; i < layout.size(); i++) {
        validSegments.push_back(layout[i]);
    }

    return validSegments;
}

bool GameController::isMoveBlockedByWall(int fromRow, int fromCol, int toRow, int toCol) const
{
    if (!level) {
        return false;
    }

    Grid& grid = level->getGrid();
    const QVector<WallSegment> segments = wallSegmentsForGrid(grid.getRows(), grid.getCols());
    for (int i = 0; i < segments.size(); i++) {
        const WallSegment& segment = segments[i];
        const bool sameDirection = segment.rowA == fromRow && segment.colA == fromCol
                                   && segment.rowB == toRow && segment.colB == toCol;
        const bool oppositeDirection = segment.rowA == toRow && segment.colA == toCol
                                       && segment.rowB == fromRow && segment.colB == fromCol;
        if (sameDirection || oppositeDirection) {
            return true;
        }
    }

    return false;
}

bool GameController::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(watched, event);
    }

    if (!mw || !player || !level) {
        return QObject::eventFilter(watched, event);
    }

    auto* keyEvent = static_cast<QKeyEvent*>(event);
    int dx = 0, dy = 0;
    QString dir;

    switch (keyEvent->key()) {
    case Qt::Key_Up:    case Qt::Key_W:  dx = -1; dir = "north"; break;
    case Qt::Key_Down:  case Qt::Key_S:  dx =  1; dir = "south"; break;
    case Qt::Key_Left:  case Qt::Key_A:  dy = -1; dir = "west";  break;
    case Qt::Key_Right: case Qt::Key_D:  dy =  1; dir = "east";  break;
    default:
        return QObject::eventFilter(watched, event);
    }

    const int currentRow = player->getRow();
    const int currentCol = player->getCol();
    const int newRow = currentRow + dx;
    const int newCol = currentCol + dy;

    if (isMoveBlockedByWall(currentRow, currentCol, newRow, newCol)) {
        mw->showLog("A wall blocks your path.");
        return true;
    }

    QString msg = movePlayer(dx, dy);
    mw->drawGrid();
    mw->redrawEntities();
    mw->updateHUD();

    if (!msg.isEmpty() && msg != "Blocked") {
        mw->showLog(msg);
    } else if (msg == "Blocked") {
        mw->showLog("Blocked.");
    } else {
        mw->showLog("You move " + dir + ".");
    }

    QTimer::singleShot(800, mw, &MainWindow::checkEndConditions);
    return true;
}
