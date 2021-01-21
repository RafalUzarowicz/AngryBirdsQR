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