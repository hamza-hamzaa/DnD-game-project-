#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>

// Shared "actor" for combat: name, HP, attack, defense, and grid position.
// Player and Enemy inherit from this; subclasses decide how hard they hit (basicAttackDamage)
// and how their attack reads in the log (attackText).
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

    // How much HP this character would lose from a raw hit (defense + minimum 1 rule).
    int reducedDamageFrom(int rawDamage) const;

    QString getName() const;
    void setHealth(int amount);
    int getHealth() const;
    int getMaxHealth() const;
    int getAttackPower() const;
    int getDefense() const;
    void addAttack(int amount);
    void addDefense(int amount);
    int getRow() const;
    int getCol() const;
    void setPos(int x, int y);
};

#endif
