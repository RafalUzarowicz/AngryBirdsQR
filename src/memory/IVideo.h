//
// Created by asia on 18.01.2021.
//

#ifndef QRGAME_IIMG_H
#define QRGAME_IIMG_H
#include <opencv2/opencv.hpp>
#include "../common.h"
#include <semaphore.h>
#include <cstdint>

struct VideoData{
    int height{};
    int width{};
    std::chrono::system_clock::time_point timestamp{};
    uint8_t buffer[MAX_WIDTH * MAX_HEIGHT * 3]{};
};
class IVideo{
public:
    virtual void sendData(VideoData* buffer)=0;
    virtual void getData(VideoData* buffer)=0;
protected:
    sem_t* consumer;
    sem_t* producer;
};

#endif //QRGAME_IIMG_H
