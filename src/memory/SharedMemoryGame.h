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
#include <utility>

class SharedMemoryGame {
public:
    SharedMemoryGame();
    ~SharedMemoryGame() = default;

    template<typename Func, typename ... Args>
    void sendData(Func f, Args&&... args){
        sem_wait(this->producer);
        f(std::forward<Args>(args)...);
        sem_post(this->consumer);
    }
    template<typename Func,typename ... Args>
    void getData(Func f, Args&&... args){
        sem_wait(this->consumer);
        f(std::forward<Args>(args)...);
        sem_post(this->producer);
    }
    GameData * data;
    sem_t* consumer;
    sem_t* producer;
private:
    int sh_memory;
    size_t size;
};


#endif //QRGAME_SHAREDMEMORYGAME_H
