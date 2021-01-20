#ifndef QRGAME_QRREADER_H
#define QRGAME_QRREADER_H

#include "../IProcess.h"
#include "../memory/SharedMemoryGame.h"
#include "../memory/SharedMemoryVideo.h"
#include "../memory/SharedQueueGame.h"
#include "../memory/SharedQueueVideo.h"
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <csignal>
#include "../Util.h"
#include "../memory/SharedMemoryVideo.h"
using namespace cv;

struct QRData{
    cv::Mat* bbox{};
    int height{};
    bool found =false;
    int id{};
    //todo timestamp?
};
class QRReader: public IProcess {
private:
    SharedMemoryVideo mem_video;
    SharedMemoryGame mem_game;
    SharedQueueVideo mq_video;
    SharedQueueGame mq_game;
    CommunicationType commsTypeImageToQr;
    CommunicationType commsTypeQrToGame;
public:
    QRReader(CommunicationType commsTypeImageToQr, CommunicationType commsTypeQrToGame, bool isQueueBlockingVideo = false, bool isQueueBlockingGame = false) : commsTypeImageToQr(commsTypeImageToQr), commsTypeQrToGame(commsTypeQrToGame), mq_video(false, isQueueBlockingVideo), mq_game(true, isQueueBlockingGame){
//        std::cout<<"xdkonst\n";
    }
    void run() override{
//        std::cout<<"xd0\n";
        QRData data{};
        VideoData videoData{};
        while(kill(getppid(), 0) == 0) {
//            std::cout<<"xd1\n";
            if(commsTypeImageToQr == SHARED_MEMORY){
//                std::cout<<"xd2\n";
                mem_video.getData(findQr, mem_video, &data);
            }else{
//                std::cout<<"xd3\n";
                mq_video.receiveMsg(&videoData);
                findQr2(&data, &videoData);
            }
            if(data.found){
//                std::cout<<"xd5\n";
                GameData gameData{};
                gameData.percentage = getPercent(&data);
                gameData.id = data.id;
                gameData.timestamp = std::chrono::system_clock::now();
                if(commsTypeQrToGame ==SHARED_MEMORY){
                    mem_game.sendData(sendPosition, mem_game, &gameData);
                }else{
                    mq_game.sendMsg(& gameData);
                }
            }
        }
    }
private:
    static void findQr(SharedMemoryVideo & shm, QRData* qrData){
//        std::cout<<shm.data->height<<'\n';
//        std::cout<<shm.data<<"\n";
        cv::Mat img(shm.data->height, shm.data->width, CV_8UC1, &shm.data->image[0]);
//        std::cout<<"xd4\n";
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
    static void findQr2(QRData* qrData, VideoData* videoData){
        cv::Mat img(videoData->height, videoData->width, CV_8UC1, &videoData->image[0]);
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
    static void sendPosition(SharedMemoryGame & shm, GameData* gameData){
        memcpy(shm.data, gameData, sizeof(GameData));
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
