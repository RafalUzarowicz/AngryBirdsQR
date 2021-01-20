#ifndef QRGAME_QRREADER_H
#define QRGAME_QRREADER_H

#include "IProcess.h"
#include "memory/SharedMemoryGame.h"
#include "memory/SharedMemoryVideo.h"
#include "memory/SharedQueueGame.h"
#include "memory/SharedQueueVideo.h"
#include <opencv2/opencv.hpp>
#include "Util.h"
#include "memory/SharedMemoryVideo.h"
using namespace cv;

struct QRData{
    cv::Mat* bbox{};
    int height{};
    bool found =false;
    int id{};
    //todo timestamp?
};
class QRReader: public IProcess {
public:
    [[noreturn]] void run() override{
        QRData data{};
        SharedMemoryVideo mem_video{};
        SharedMemoryGame mem_game{};
        SharedQueueVideo mq_video(false, false);
        SharedQueueGame mq_game(false, false);
        while(true) {
            /*----SHARED MEMORY-----*/
            mem_video.getData(findQr, &mem_video, &data);
            /*---QUEUE----*/
            VideoData videoData;
            findQr(&data, &videoData);
            if(data.found){
                GameData gameData;
                gameData.percentage = getPercent(&data);
                gameData.id = data.id;
                gameData.timestamp = std::chrono::system_clock::now();
                /*---Shared MEM---*/
                mem_game.sendData(sendPosition, &mem_game, &gameData);
                /*---send via msq---*/
                mq_game.sendMsg(& gameData);
            }
        }
    }
private:
    static void findQr(SharedMemoryVideo & shm, QRData* qrData){
        cv::Mat img(shm.data->height, shm.data->width, CV_8UC1, &shm.data->image[0]);
        qrData->id = shm.data->id;
        QRCodeDetector qrDetector;
        Mat bbox;
        if(qrDetector.detect(img, bbox)){
            if(bbox.cols==4){
                qrData->bbox = &bbox;
                qrData->height = shm.data->height;
                qrData->found = true;
            }
        }
    }
    static void findQr(QRData* qrData, VideoData* videoData){
        cv::Mat img(videoData->height, videoData->width, CV_8UC1, &videoData.image[0]);
        qrData->id = videoData->id;
        QRCodeDetector qrDetector;
        Mat bbox;
        if(qrDetector.detect(img, bbox)){
            if(bbox.cols==4){
                qrData->bbox = &bbox;
                qrData->height = videoData->height;
                qrData->found = true;
            }
        }
    }
    static void sendPosition(SharedMemoryVideo & shm, GameData* gamedata){
        memcpy(shm.data, gamedata, sizeof(GameData));
    }
    static double getPercent(QRData * data){
        int max_y=0, min_y=data->height;
        Point points[4];
        for(int i=0; i<4;i++){
            points[i] = Point(data->bbox->at<float>(2*i), data->bbox->at<float>(2*i +1));
            max_y = points[i].y>max_y?points[i].y:max_y;
            min_y = points[i].y<min_y?points[i].y:min_y;
        }
        int lower_bound = (max_y - min_y)/2;
        int upper_bound = data->height- (max_y -min_y)/2;
        int middle = (max_y + min_y)/2;
        return  (middle-lower_bound)/(double)(upper_bound-lower_bound);
    }
};


#endif //QRGAME_QRREADER_H
