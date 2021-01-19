//
// Created by asia on 18.01.2021.
//

#ifndef QRGAME_SHAREDGAME_H
#define QRGAME_SHAREDGAME_H
#include <sys/mman.h>
#include "IGame.h"
#include  <stdio.h>

class SharedGame: public IGame{
private:
    void* sh_memory;
public:
    SharedGame(int filedes, size_t length){
        sh_memory = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, filedes, 0);

        errno = 0;
        if((this->consumer = sem_open(SEM_GAME_CONS, 0))==SEM_FAILED){
            printf("%s", stderr(errno));
        }
        if((this->producer = sem_open(SEM_GAME_PROD, 0))==SEM_FAILED){
            printf("%s", stderr(errno));
        }
        if(errno != 0){
            exit(-1);
        }
    }
    void sendData(GameData* data) override{
        sem_wait(this->producer);
        memcpy(sh_memory, data, sizeod(GameData));
        sem_post(this->consumer);

    }
    void getData(GameData * data) override{
        sem_wait(this->consumer);
        memcpy( data, sh_memory, sizeof(GameData));
        sem_post(this->producer;
    }
    ~SharedGame(){
        munmap(sh_memory, length);
    }
};

#endif //QRGAME_SHAREDGAME_H
