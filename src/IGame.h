//
// Created by asia on 18.01.2021.
//

#ifndef QRGAME_IMEMORY_H
#define QRGAME_IMEMORY_H
struct GameData{
    double timestamp;
    double percent;
};
class IGame{
public:
    virtual void sendData(GameData data) = 0;
    virtual GameData getData ()=0;
};

#endif //QRGAME_IMEMORY_H
