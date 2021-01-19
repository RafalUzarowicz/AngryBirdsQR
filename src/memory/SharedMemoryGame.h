//
// Created by asia on 19.01.2021.
//

#ifndef QRGAME_SHAREDMEMORYGAME_H
#define QRGAME_SHAREDMEMORYGAME_H
#include <chrono>
#include <semaphore.h>
#include <cstdint>
#include <chrono>
#include <semaphore.h>
#include "../Common.h"
#include <sys/mman.h>
#include <fcntl.h>

struct GameData{
    double percentage{};
    std::chrono::system_clock::time_point timestamp{};
};
class SharedMemoryGame {
public:
    SharedMemoryGame();
    ~SharedMemoryGame();

    template<typename Func, typename ... Args>
    void sendData(Func f, Args&&... args);
    template<typename Func,typename ... Args>
    void getData(Func f, Args&&... args);
    GameData * data;
private:
    int sh_memory;
    sem_t* consumer;
    sem_t* producer;
    size_t size;
};


#endif //QRGAME_SHAREDMEMORYGAME_H
