#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>

class Character {
protected:
    QString name;
    int health;
    int maxHealth;
    int attackPower;
    int defense;
    int posX;
    int posY;

public:

    Character(const QString& name, int health, int attackPower, int defense = 0, int x=0, int y=0);
    virtual ~Character()= default;

    virtual int basicAttackDamage() const = 0;
    virtual QString attackText() const = 0;

    void takeDamage(int damage);
    void heal(int amount);
    bool isAlive() const;

    QString getName() const;
    void setHealth(int amount);
    int getHealth() const;
    int getMaxHealth() const;
    int getAttackPower() const;
    int getDefense() const;
    void addAttack(int amount);
    void addDefense(int amount);
    int getRow();
    int getCol();
    void setPos(int x, int y);
};

#endif