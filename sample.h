#ifndef SAMPLE_H
#define SAMPLE_H
#include <vector>
#include <QString>

using namespace std;

class Player;

// =========================
// Character
// =========================
class Character {
protected:
    int health;
    int attackPower;
    int positionRow;
    int positionCol;

public:
    Character(int hp = 100, int atk = 10, int row = 0, int col = 0)
        : health(hp), attackPower(atk), positionRow(row), positionCol(col) {}

    virtual ~Character() {}

    void setPosition(int row, int col) {
        positionRow = row;
        positionCol = col;
    }

    int getRow() const {
        return positionRow;
    }

    int getCol() const {
        return positionCol;
    }

    int getHealth() const {
        return health;
    }

    int getAttackPower() const {
        return attackPower;
    }

    void setHealth(int hp) {
        health = hp;
    }

    void takeDamage(int dmg) {
        health -= dmg;
        if (health < 0) {
            health = 0;
        }
    }

    bool isAlive() const {
        return health > 0;
    }
};

// =========================
// Item
// =========================
class Item {
protected:
    QString name;

public:
    Item(QString itemName = "Item") : name(itemName) {}
    virtual ~Item() {}

    QString getName() const {
        return name;
    }

    virtual void applyEffect(Player& player) = 0;
};

// =========================
// Player
// =========================
class Player : public Character {
private:
    QString playerName;
    QString race;
    QString style;
    vector<Item*> inventory;

public:
    Player(QString name = "Player",
           QString race = "Human",
           QString style = "Warrior",
           int hp = 100,
           int atk = 15,
           int row = 0,
           int col = 0)
        : Character(hp, atk, row, col),
        playerName(name),
        race(race),
        style(style) {}

    QString getName() const {
        return playerName;
    }

    QString getRace() const {
        return race;
    }

    QString getStyle() const {
        return style;
    }

    void move(int dRow, int dCol) {
        positionRow += dRow;
        positionCol += dCol;
    }

    void heal(int amount) {
        health += amount;
    }

    void attack(Character& target) {
        target.takeDamage(attackPower);
    }

    void addItem(Item* item) {
        inventory.push_back(item);
    }
};

// =========================
// Enemy
// =========================
class Enemy : public Character {
private:
    QString enemyType;

public:
    Enemy(QString type = "Goblin", int hp = 40, int atk = 10, int row = 0, int col = 0)
        : Character(hp, atk, row, col), enemyType(type) {}

    QString getType() const {
        return enemyType;
    }

    void attackPlayer(Character& player) {
        player.takeDamage(attackPower);
    }

    void takeTurn(Character& player) {
        attackPlayer(player);
    }
};



// =========================
// Equipment
// =========================
class Equipment : public Item {
private:
    int bonusAttack;
    int bonusHealth;

public:
    Equipment(QString itemName = "Equipment", int atk = 5, int hp = 10)
        : Item(itemName), bonusAttack(atk), bonusHealth(hp) {}

    void applyEffect(Player& player) override {
        player.setHealth(player.getHealth() + bonusHealth);
        bonusAttack+=0;
    }
};


class Cell {
public:
    bool hasEnemy;
    bool hasPotion;
    bool hasEquipment;
    bool visited;

    Cell()
        : hasEnemy(false),
        hasPotion(false),
        hasEquipment(false),
        visited(false) {}

    void clearEvent() {
        hasEnemy = false;
        hasPotion = false;
        hasEquipment = false;
    }
};

// =========================
// Grid
// =========================
class Grid {
private:
    int rows;
    int cols;
    vector<vector<Cell>> cells;

public:
    Grid(int r = 4, int c = 4) : rows(r), cols(c), cells(r, vector<Cell>(c)) {}

    int getRows() const {
        return rows;
    }

    int getCols() const {
        return cols;
    }

    Cell& getCell(int row, int col) {
        return cells[row][col];
    }

    const Cell& getCell(int row, int col) const {
        return cells[row][col];
    }

    bool isInside(int row, int col) const {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }

    bool isValidMove(int row, int col) const {
        return isInside(row, col);
    }
};


#endif // SAMPLE_H
