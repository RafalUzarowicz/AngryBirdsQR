#ifndef QRGAME_PLAYER_H
#define QRGAME_PLAYER_H

#include "Sprite.h"

#include <string>
#include <vector>

class Player {
private:
    int x, y;
    Sprite sprite;
public:

    Player(int start_x, int start_y) : x(start_x), y(start_y) {

    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    void setX(int x) {
        Player::x = x;
    }

    void setY(int y) {
        Player::y = y;
    }

    const Sprite &getSprite() const {
        return sprite;
    }

};


#endif //QRGAME_PLAYER_H
