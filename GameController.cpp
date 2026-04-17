// Ties together level data, player, and UI: movement, cell events, enemy turns, walls, and keyboard control.

#include "GameController.h"
#include <QRandomGenerator>
#include <QKeyEvent>
#include <QTimer>
#include <cstdlib>
#include <limits>
#include <vector>

#include "enemy.h"
#include "combat.h"
#include "mainwindow.h"
//constructor that initalizes all variables to a base value
//level initalized to nullpts as startgame func. is what creates the actual level this is so that the functionalities dont overlap
GameController::GameController(Player* p, MainWindow* owner)
    : QObject(owner), LevelNumber(1), level(nullptr), player(p), mw(owner)
{
}
//destructor that deletes alldynamically allocated memory in the class
GameController::~GameController() {
    delete level;
}
//function only  used in the start of the game and resets the levelnumber to one and creates the inital level
void GameController::startGame() {
    LevelNumber = 1;
    player->setHealth(player->getMaxHealth());
    loadLevel();
}
//function that creates a level object based on the levelnumber and resets player pos
void GameController::loadLevel() {
    delete level;
    level = new Level(LevelNumber);
    level->generateLevel();
    player->setPos(0, 0);
    levelStarted = false;

}
//function first changes player pos to a temp var then checks if this new var is a valid move and if so it sets it as the players final position
//return  type isnt void since  it returns a string so that it can interact with the window and type out messages for the player
QString GameController::movePlayer(int dx, int dy) {
    if (!level || !player) {
        return "Blocked";
    }
    levelStarted = true;
    int newRow = player->getRow() + dx;
    int newCol = player->getCol() + dy;

    if (level->getGrid().isValidMove(newRow, newCol)) {
        player->setPos(newRow, newCol);
        QString log = handleCellEvent();
        if (player->isAlive()) {
            const QString enemyLog = moveEnemy();
            if (!enemyLog.isEmpty()) {
                if (!log.isEmpty()) {
                    log += "\n";
                }
                log += enemyLog;
            }
        }

        if (player->isAlive() && level->isExitCell(player->getRow(), player->getCol()) && level->getEnemies().empty()) {
            if (LevelNumber == 1) {
                if(log.isEmpty()){
                    return "You reached the final exit!" ;
                }
                else{
                    return log + "\nYou reached the final exit!";
                }
            }

        }

        return log;
    }
    return "Blocked";

}

//function checks all possibilities states of the cell such as whether it has an enemy potion or anything and based on these of conditions with execute a certain functionality
QString GameController::handleCellEvent() {
    if (!level || !player) {
        return {};
    }
    int row = player->getRow();
    int col = player->getCol();

    Cell& cell = level->getGrid().getCell(row, col);

    if (cell.hasEnemy) {
        std::vector<Enemy>& enemies = level->getEnemies();
        //function first confirms that there is an enemy in this cell then through a full loop it confirms which enemy it is
        for (size_t i = 0; i < enemies.size(); i++) {
            if (enemies[i].getRow() == row && enemies[i].getCol() == col) {
                Enemy& enemy = enemies[i];
                QString log = Combat::runMeleeEncounter(*player, enemy);

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
        const int trapRaw = 10;
        const int dmg = player->reducedDamageFrom(trapRaw);
        player->takeDamage(trapRaw);
        cell.hasTrap = false;
        return "You triggered a trap! Lost " + QString::number(dmg) + " HP.";
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
           && LevelNumber == 1
           && level->getEnemies().empty()
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
// Move enemies toward the player and attack when they catch up.
QString GameController::moveEnemy(){
    if (!level || !player || !player->isAlive()) {
        return {};
    }
    QString enemyLog;
    std::vector<Enemy>& enemies = level->getEnemies();
    Grid& grid = level->getGrid();

    const int dRows[4] = {-1, 1, 0, 0};
    const int dCols[4] = {0, 0, -1, 1};

    for (size_t i = 0; i < enemies.size(); i++) {
        if (!player->isAlive()) {
            break;
        }

        Enemy& enemy = enemies[i];
        const int curRow = enemy.getRow();
        const int curCol = enemy.getCol();

        int bestRow = curRow;
        int bestCol = curCol;
        int bestDistance = std::numeric_limits<int>::max();

        for (int dir = 0; dir < 4; dir++) {
            const int newRow = curRow + dRows[dir];
            const int newCol = curCol + dCols[dir];

            if (!grid.isValidMove(newRow, newCol)
                || isMoveBlockedByWall(curRow, curCol, newRow, newCol)) {
                continue;
            }

            const bool isPlayerCell = (newRow == player->getRow() && newCol == player->getCol());
            const Cell& destination = grid.getCell(newRow, newCol);
            if (!isPlayerCell
                && (destination.hasEnemy || destination.hasPotion || destination.hasEquipment)) {
                continue;
            }

            const int distanceToPlayer = std::abs(player->getRow() - newRow) + std::abs(player->getCol() - newCol);
            if (distanceToPlayer < bestDistance) {
                bestDistance = distanceToPlayer;
                bestRow = newRow;
                bestCol = newCol;
            }
        }

        if (bestRow == player->getRow() && bestCol == player->getCol()) {
            const int rawDamage = enemy.basicAttackDamage();
            const int applied = player->reducedDamageFrom(rawDamage);
            player->takeDamage(rawDamage);
            if (!enemyLog.isEmpty()) {
                enemyLog += "\n";
            }
            enemyLog += enemy.getName() + " catches you and hits for " + QString::number(applied) + " damage!";
            if (!player->isAlive()) {
                enemyLog += "\nYou were defeated!";
            }
            continue;
        }

        if (bestRow != curRow || bestCol != curCol) {
            grid.getCell(curRow, curCol).hasEnemy = false;
            enemy.setPos(bestRow, bestCol);
            grid.getCell(bestRow, bestCol).hasEnemy = true;
        }
    }

    return enemyLog;
}
//function checks grid size and level number to select the maze structure for the level
//when next levels are added the function have more if condittionals and walls added
QVector<GameController::WallSegment> GameController::wallSegmentsForGrid(int rows, int cols) const
{
    QVector<WallSegment> validSegments;

    if (!level || LevelNumber != 1 || rows != 6 || cols != 6) {
        return validSegments;
    }
    validSegments = {
        {0, 0, 0, 1},
        {1, 1, 1, 2},
        {2, 0, 2, 1},
        {4, 0, 4, 1},
        {3, 1, 3, 2},
        {4, 1, 4, 2},
        {5, 1, 5, 2},
        {0, 2, 0, 3},
        {1, 2, 1, 3},
        {0, 3, 0, 4},
        {1, 3, 1, 4},
        {1, 4, 1, 5},
        {5, 2, 5, 3},
        {5, 4, 5, 5},
        {1, 0, 2, 0},
        {1, 1, 2, 1},
        {2, 2, 3, 2},
        {2, 3, 3, 3},
        {2, 4, 3, 4},
        {3, 0, 4, 0},
        {3, 3, 4, 3},
        {3, 4, 4, 4},
        {3, 5, 4, 5},
        {4, 3, 5, 3}

    };


    return validSegments;
}
//function checks if player/enemy collide with a wall by ceompering the players intial pos and final pos to the walls inital cell and next cell
bool GameController::isMoveBlockedByWall(int fromRow, int fromCol, int toRow, int toCol) const
{
    if (!level) {
        return false;
    }

    Grid& grid = level->getGrid();
    const QVector<WallSegment> segments = wallSegmentsForGrid(grid.getRows(), grid.getCols());
    for (int i = 0; i < segments.size(); i++) {
        const WallSegment& segment = segments[i];
        const bool sameDirection = segment.rowA == fromRow && segment.colA == fromCol&& segment.rowB == toRow && segment.colB == toCol;
        const bool oppositeDirection = segment.rowA == toRow && segment.colA == toCol&& segment.rowB == fromRow && segment.colB == fromCol;
        if (sameDirection || oppositeDirection) {
            return true;
        }
    }

    return false;
}
//by overriding QT's event filter checks all inputs and only accepts keyboard inputs and based oon them displays a message and changes player position
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
