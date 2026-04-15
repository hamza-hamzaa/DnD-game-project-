#include "GameController.h"
#include <QRandomGenerator>
#include"Enemy.h"

GameController::GameController(Player* p): LevelNumber(1), level(nullptr), player(p){
}

GameController::~GameController() {
    delete level;
}

void GameController::startGame() {
    LevelNumber = 1;
    player->setHealth(player->getMaxHealth());
    loadLevel();
}

void GameController::loadLevel() {
    delete level;
    level = new Level(LevelNumber);
    level->generateLevel();
    player->setPos(0, 0);
    levelStarted = false;
}

QString GameController::movePlayer(int dx, int dy) {
    levelStarted = true;
    int newRow = player->getRow() + dx;
    int newCol = player->getCol() + dy;

    if (level->getGrid().isValidMove(newRow, newCol)) {
        player->setPos(newRow, newCol);
        QString log = handleCellEvent();

        if (player->isAlive() && level->isExitCell(player->getRow(), player->getCol())) {
            if (LevelNumber == 5) {
                return log.isEmpty() ? "You reached the final exit!" : log + "\nYou reached the final exit!";
            }

            nextLevel();
            return log.isEmpty() ? "You reached the exit. Descending to the next level." :
                                   log + "\nYou reached the exit. Descending to the next level.";
        }

        return log;
    }
    return "Blocked!";

    player->setPos(newRow, newCol);
    return handleCellEvent();
}


QString GameController::handleCellEvent() {
    int row = player->getRow();
    int col = player->getCol();

    Cell& cell = level->getGrid().getCell(row, col);

    if (cell.hasEnemy) {
        vector<Enemy>& enemies = level->getEnemies();

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
    else if (cell.hasTrap) {
        player->takeDamage(10);
        cell.hasTrap = false;
        return "You triggered a trap! Lost 10 HP.";
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
           && level->isExitCell(player->getRow(), player->getCol());
}

bool GameController::checkLose() {
    return player->getHealth() <= 0;
}
Level* GameController::getLevel(){return level;}
int GameController::getLevelNumber(){return LevelNumber;}
bool GameController::isLevelStarted() const { return levelStarted; }
