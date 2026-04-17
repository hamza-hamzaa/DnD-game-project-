#include "combat.h"

#include "enemy.h"
#include "player.h"

namespace Combat {

// Auto-resolve a stand-up fight: player swings first each round, then the enemy, repeat.
// Log lines use real damage after defense so they match the HP bar.
QString runMeleeEncounter(Player& player, Enemy& enemy)
{
    QString log;

    while (player.isAlive() && enemy.isAlive()) {
        const int rawPlayerHit = player.basicAttackDamage();
        const int dmgToEnemy = enemy.reducedDamageFrom(rawPlayerHit);
        enemy.takeDamage(rawPlayerHit);

        log += "You hit " + enemy.getName() + " for "
               + QString::number(dmgToEnemy) + " damage. ";

        if (!enemy.isAlive()) {
            log += enemy.getName() + " was defeated!";
            break;
        }

        const int rawEnemyHit = enemy.basicAttackDamage();
        const int dmgToPlayer = player.reducedDamageFrom(rawEnemyHit);
        player.takeDamage(rawEnemyHit);

        log += enemy.getName() + " hits back for "
               + QString::number(dmgToPlayer) + " damage. ";

        if (!player.isAlive()) {
            log += "You were defeated!";
            break;
        }

        log += "\n";
    }

    return log;
}

}
