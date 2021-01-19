//
// Created by asia on 18.01.2021.
//

#ifndef QRGAME_SHAREDIMG_H
#define QRGAME_SHAREDIMG_H
#include "IVideo.h"
#include <sys/mman.h>
#include "../common.h"
#include <string.h>

class SharedVideo: IVideo{
private:
    void * sh_memory;
    size_t length;
public:
    SharedVideo(int filedes, size_t length):length(length){
        sh_memory = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, filedes, 0);

        errno = 0;
        if((this->consumer = sem_open(SEM_VIDEO_CONS, 0))==SEM_FAILED){
            printf("%s", strerror(errno));
        }
        if((this->producer = sem_open(SEM_VIDEO_PROD, 0))==SEM_FAILED){
            printf("%s", strerror(errno));
        }
        if(errno != 0){
            exit(-1);
        }

    }
    void getData(VideoData* data) override{
        sem_wait(this->consumer);
        memcpy(data, sh_memory, sizeof(VideoData));
        sem_post(this->producer);
    }
    void sendData (VideoData* data) override{
        sem_wait(this->producer);
        memcpy(sh_memory, data, sizeof(VideoData));
        sem_post(this->consumer);
    }
    ~SharedVideo(){
        munmap(sh_memory, length);
    }
};

#endif //QRGAME_SHAREDIMG_H
