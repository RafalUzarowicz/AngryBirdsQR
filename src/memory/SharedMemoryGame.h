#ifndef QRGAME_SHAREDMEMORYGAME_H
#define QRGAME_SHAREDMEMORYGAME_H
#include <chrono>
#include <semaphore.h>
#include <cstdint>
#include <chrono>
#include <semaphore.h>
#include "../Util.h"
#include <sys/mman.h>
#include <fcntl.h>

class SharedMemoryGame {
public:
    SharedMemoryGame();
    ~SharedMemoryGame() = default;

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
