//
// Created by asia on 18.01.2021.
//

#ifndef QRGAME_IIMG_H
#define QRGAME_IIMG_H
#include <opencv2/opencv.hpp>

using namespace cv;

struct VideoData{
    Mat img;
    double timestamp;
};
class IVideo{
public:
    virtual void sendData(VideoData data)=0;
    virtual VideoData getData()=0;
};

#endif //QRGAME_IIMG_H
