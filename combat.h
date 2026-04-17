#ifndef COMBAT_H
#define COMBAT_H

#include <QString>

class Player;
class Enemy;

// Small combat helper namespace: one place for "you hit, they hit back" until someone drops.
namespace Combat {

QString runMeleeEncounter(Player& player, Enemy& enemy);

}

#endif
