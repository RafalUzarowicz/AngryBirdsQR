#ifndef QRGAME_SHAREDMEMORYVIDEO_H
#define QRGAME_SHAREDMEMORYVIDEO_H

#include <cstdint>
#include <chrono>
#include <semaphore.h>
#include "../Util.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <utility>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <ftw.h>


class SharedMemoryVideo {
public:
    SharedMemoryVideo(){
        sh_memory = shm_open(VIDEO_MEM_NAME, O_RDWR, 0660);

        struct stat mem_stat{};
        fstat(sh_memory, &mem_stat);
        size = mem_stat.st_size;

        data = static_cast<VideoData *>(mmap(nullptr, size, PROT_WRITE | PROT_READ, MAP_SHARED, sh_memory, 0));

        errno = 0;
        if((this->producer = sem_open(SEM_VIDEO_PROD, 0))==SEM_FAILED){
            std::cerr<<strerror(errno);
            exit(1);
        }
        if((this->consumer = sem_open(SEM_VIDEO_CONS, 0))==SEM_FAILED){
            std::cerr<<strerror(errno);
            exit(1);
        }
    }
    ~SharedMemoryVideo() = default;

    template<typename Func, typename... Args>
    void getData(Func f, Args&&... args){
        sem_wait(this->consumer);
        f(std::forward<Args>(args)...);
        sem_post(this->producer);
    }

    template<typename Func, typename... Args>
    void sendData(Func f, Args&&... args){
        sem_wait(this->producer);
        f(std::forward<Args>(args)...);
        sem_post(this->consumer);
    }

    VideoData * data;
private:
    int sh_memory;
    sem_t* consumer;
    sem_t* producer;
    size_t size;
};


#endif //QRGAME_SHAREDMEMORYVIDEO_H
