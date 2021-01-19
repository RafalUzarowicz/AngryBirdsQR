
#include "SharedMemoryVideo.h"
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <ftw.h>

SharedMemoryVideo::SharedMemoryVideo(){
    sh_memory = shm_open(VIDEO_MEM_NAME, O_RDWR, 0660);

    struct stat mem_stat{};
    fstat(sh_memory, &mem_stat);
    size = mem_stat.st_size;

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