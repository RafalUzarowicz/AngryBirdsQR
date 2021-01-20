//
// Created by asia on 19.01.2021.
//

#ifndef QRGAME_SHAREDMEMORYVIDEO_H
#define QRGAME_SHAREDMEMORYVIDEO_H

#include <cstdint>
#include <chrono>
#include <semaphore.h>
#include "../Util.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <utility>

class SharedMemoryVideo {
public:
    SharedMemoryVideo();
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
