//
// Created by asia on 18.01.2021.
//

#ifndef QRGAME_IMEMORY_H
#define QRGAME_IMEMORY_H

#include <semaphore.h>
#include "../common.h"

struct GameData{
    std::chrono::system_clock::time_point timestamp{};
    double percent{};
};
class IGame{
public:
    virtual void sendData(GameData* data) = 0;
    virtual void getData (GameData* buffer) = 0;
protected:
    sem_t* consumer;
    sem_t* producer;
};

#endif //QRGAME_IMEMORY_H
