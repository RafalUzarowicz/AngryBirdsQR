#ifndef QRGAME_SHAREDMEMORYGAME_H
#define QRGAME_SHAREDMEMORYGAME_H

#include "../Util.h"

#include <semaphore.h>
#include <cstdint>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <utility>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <ftw.h>

class SharedMemoryGame {
public:
    SharedMemoryGame(){
        sh_memory = shm_open(GAME_MEM_NAME, O_RDWR, 0777);

        struct stat mem_stat{};
        fstat(sh_memory, &mem_stat);
        size = mem_stat.st_size;

        data = static_cast<GameData *>(mmap(nullptr, size, PROT_WRITE | PROT_READ, MAP_SHARED,sh_memory, 0));

        errno = 0;
        if((this->producer = sem_open(SEM_GAME_PROD, 0))==SEM_FAILED){
            std::cerr<<strerror(errno)<<"\n";
            exit(1);
        }
        if((this->consumer = sem_open(SEM_GAME_CONS, 0))==SEM_FAILED){
            std::cerr<<strerror(errno)<<"\n";
            exit(1);
        }
    }
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
private:
    sem_t* consumer;
    sem_t* producer;
    int sh_memory;
    size_t size;
};


#endif //QRGAME_SHAREDMEMORYGAME_H
