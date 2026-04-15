#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"
#include <QString>

class Enemy : public Character {
private:
    QString type;

public:
    Enemy(const QString& name,const QString& type,int health,int attackPower,int defense = 0,int x = 0,int y = 0);
    ~Enemy() override = default;
    QString getType() const;
    QString decideActionText() const;
    QString attackText() const override;
    int basicAttackDamage() const override;
};




#endif