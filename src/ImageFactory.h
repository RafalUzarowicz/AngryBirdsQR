#ifndef QRGAME_IMAGEFACTORY_H
#define QRGAME_IMAGEFACTORY_H

#include "memory/SharedMemoryVideo.h"
#include "image/CameraHandler.h"
#include "memory/SharedQueueVideo.h"
#include "IProcess.h"
#include "Util.h"

#include <thread>

class ImageFactory : public IProcess {
private:
    SharedMemoryVideo sharedMemory;
    SharedQueueVideo sharedQueue;
    CommunicationType communicationType;
    VideoData data;

public:

    explicit ImageFactory(CommunicationType communicationType, bool isQueueBlocking) :
            communicationType(communicationType), sharedQueue(true, isQueueBlocking) {
    }

    void run() override {
        int id = 0;
        double nowTime, deltaTime, lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        cv::Mat frame;
        cv::VideoCapture videoCapture;
#ifndef DONT_USE_PROCESSES
        while (kill(getppid(), 0) == 0) {
#endif
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;

            videoCapture.open(CAMERA_DEV_ID, cv::CAP_ANY);
            videoCapture.read(frame);
            data.height = frame.rows;
            data.width = frame.cols;
            data.type = frame.type();
            data.id = id;

            memcpy(data.image, frame.data, frame.elemSize() * frame.total());

            data.timestamp = std::chrono::system_clock::now();
            //std::cout<<"[IMG]"<<id<<"\n";

            if (communicationType == QUEUE) {
                sharedQueue.sendMsg(&data);
            } else {
                sharedMemory.sendData(sendImage, sharedMemory, &data);
            }
            id += 1;
            lastTime = nowTime;
#ifndef DONT_USE_PROCESSES
        }
#endif
    }

    static void sendImage(SharedMemoryVideo &shm, VideoData *data) {
        memcpy(shm.data, data, sizeof(VideoData));
    }
};

#endif //QRGAME_IMAGEFACTORY_H