//
// Created by asia on 18.01.2021.
//

#ifndef QRGAME_SHAREDIMG_H
#define QRGAME_SHAREDIMG_H
#include "IVideo.h"
#include <sys/mman.h>

class SharedVideo: IVideo{
private:
    void * sh_memory;
public:
    SharedVideo(int filedes, size_t length){
        sh_memory = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, filedes, 0);
    }
    VideoData* getData(){


    }
    void sendData (VideoData* data){

    }
};

#endif //QRGAME_SHAREDIMG_H
