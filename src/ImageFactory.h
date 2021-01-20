#ifndef QRGAME_IMAGEFACTORY_H
#define QRGAME_IMAGEFACTORY_H

#include <thread>
#include "IProcess.h"
#include "image/CameraHandler.h"
#include "memory/SharedMemoryVideo.h"
#include "memory/SharedQueueVideo.h"
#include "Util.h"

class ImageFactory: public IProcess {
private:
    CameraHandler *handler;
    SharedMemoryVideo sharedMemory;
    SharedQueueVideo sharedQueue;
    CommunicationType communicationType;
    VideoData data;

public:

    explicit ImageFactory(CommunicationType communicationType, bool isQueueBlocking):
    communicationType(communicationType), sharedQueue(true, isQueueBlocking) {
        this->handler = new CameraHandler();
    }

    void run() override{

//        handler->setWidth(640);
//        handler->setHeight(480);
//
//        handler->configure();
//        handler->setFrameDelay(200);

        int _id = 0;

//        int width = handler->getWidth();
//        int height = handler->getHeight();
        double nowTime, deltaTime, lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        while(kill(getppid(), 0) == 0){
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;
//            handler->captureFrame();
//            unsigned char * buffer = handler->getBuffer();
//            data.id = _id;
//            data.width = width;
//            data.height = height;
//            for (int i = 0; i < height * width * 2; i++) {
//                data.image[i] = buffer[i];
//            }
            data.timestamp = std::chrono::system_clock::now();
            cv::Mat frame;
            cv::VideoCapture videoCapture;
            videoCapture.open(0, cv::CAP_ANY);
            videoCapture.read(frame);
            data.height = frame.rows;
            data.width = frame.cols;
            data.type = frame.type();

            memcpy(data.image, frame.data, frame.elemSize()*frame.total());

            if (communicationType == QUEUE) {
                sharedQueue.sendMsg(&data);
            } else {
                sharedMemory.sendData(sendImage, sharedMemory, &data);
            }
            _id += 1;
        }

    }

    static void sendImage(SharedMemoryVideo &shm, VideoData * data) {
        memcpy(shm.data, data, sizeof(VideoData));
    }

    void getImage(){}
};


#endif //QRGAME_IMAGEFACTORY_H
