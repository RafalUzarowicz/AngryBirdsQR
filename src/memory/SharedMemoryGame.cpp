//
// Created by asia on 19.01.2021.
//

#include "SharedMemoryGame.h"
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <ftw.h>

SharedMemoryGame::SharedMemoryGame(){
    sh_memory = shm_open(GAME_MEM_NAME, O_RDWR, 0660);

    struct stat mem_stat{};
    fstat(sh_memory, &mem_stat);
    size = mem_stat.st_size;

    data = static_cast<GameData *>(mmap(nullptr,sh_memory, size, PROT_WRITE | PROT_READ, MAP_SHARED, 0));

    errno = 0;
    if((this->producer = sem_open(SEM_GAME_PROD, 0))==SEM_FAILED){
        std::cout<<strerror(errno);
    }
    if((this->consumer = sem_open(SEM_GAME_CONS, 0))==SEM_FAILED){
        std::cout<<strerror(errno);
    }
    if(errno != 0){
        exit(-1);
    }
}
template<typename Func, typename ... Args>
void SharedMemoryGame::sendData(Func f, Args&&... args){
    sem_wait(this->consumer);
    f(std::forward<Args>(args)...);
    sem_post(this->producer);
}
template<typename Func,typename ... Args>
void SharedMemoryGame::getData(Func f, Args&&... args){
    sem_wait(this->producer);
    f(std::forward<Args>(args)...);
    sem_post(this->consumer);
}