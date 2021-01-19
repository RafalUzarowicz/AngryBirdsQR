//
// Created by asia on 19.01.2021.
//

#include "SharedMemoryVideo.h"
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

SharedMemoryVideo::SharedMemoryVideo(){
    sh_memory = shm_open(VIDEO_MEM_NAME, O_CREAT|O_RDWR, 0660);
    size = sizeof (VideoData);

    ftruncate(sh_memory, size);

    data = static_cast<VideoData *>(mmap(nullptr,sh_memory, size, PROT_WRITE | PROT_READ, MAP_SHARED, 0));

    errno = 0;
    if((this->producer = sem_open(SEM_VIDEO_PROD, 0))==SEM_FAILED){
        std::cout<<strerror(errno);
    }
    if((this->consumer = sem_open(SEM_VIDEO_CONS, 0))==SEM_FAILED){
        std::cout<<strerror(errno);
    }
    if(errno != 0){
        exit(-1);
    }
}
SharedMemoryVideo::~SharedMemoryVideo(){
    shm_unlink(MEM_NAME);
}

template<typename Func, typename... Args>
void SharedMemoryVideo::getData(Func f, Args&&... args){
    sem_wait(this->consumer);
    f(std::forward<Args>(args)...);
    sem_post(this->producer);
}

template<typename Func, typename... Args>
void SharedMemoryVideo::sendData(Func f, Args&&... args){
    sem_wait(this->producer);
    f(std::forward<Args>(args)...);
    sem_post(this->consumer);
}