#ifndef QRGAME_QRREADER_H
#define QRGAME_QRREADER_H

#include "IProcess.h"
#include "memory/SharedMemoryGame.h"
#include "memory/SharedMemoryVideo.h"
#include "memory/SharedQueueGame.h"
#include "memory/SharedQueueVideo.h"
#include "Util.h"
#include "memory/SharedMemoryVideo.h"

#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <csignal>

using namespace cv;

class QRReader : public IProcess {
private:
    struct QRData {
        int id{-1};
        int height{};
        bool found = false;
        Point points[4];
        std::chrono::system_clock::time_point timestamp{};
    };

    SharedMemoryVideo mem_video;
    SharedMemoryGame mem_game;
    SharedQueueVideo mq_video;
    SharedQueueGame mq_game;
    CommunicationType commsTypeImageToQr;
    CommunicationType commsTypeQrToGame;

public:
    QRReader(CommunicationType commsTypeImageToQr, CommunicationType commsTypeQrToGame,
             bool isQueueBlockingVideo = false, bool isQueueBlockingGame = false) : commsTypeImageToQr(
            commsTypeImageToQr), commsTypeQrToGame(commsTypeQrToGame), mq_video(false, isQueueBlockingVideo),
                                                                                    mq_game(true, isQueueBlockingGame) {
    }

    void run() override {
        QRData data{};
        VideoData videoData{};
        GameData gameData{};
#ifndef DONT_USE_PROCESSES
        while (kill(getppid(), 0) == 0) {
#endif
            if (commsTypeImageToQr == SHARED_MEMORY) {
                mem_video.getData(findQrFromMemory, mem_video, &data);
            } else {
                mq_video.receiveMsg(&videoData);
                findQrFromQueue(&data, &videoData);
            }
            if (data.found && data.id >= 0) {
                gameData.percentage = getPercent(&data);
                gameData.id = data.id;
                gameData.timestamp = data.timestamp;

                // TODO time stamp save - nie musi byc tutaj

                if (commsTypeQrToGame == SHARED_MEMORY) {
                    mem_game.sendData(sendPosition, mem_game, &gameData);
                } else {
                    mq_game.sendMsg(&gameData);
                }
            }
#ifndef DONT_USE_PROCESSES
        }
#endif
    }

private:
    static void findQrFromMemory(SharedMemoryVideo &shm, QRData *qrData) {
        cv::Mat img(shm.data->height, shm.data->width, shm.data->type, shm.data->image);

        qrData->found = false;

        static QRCodeDetector qrDetector;
        static Mat bbox;

        if (qrDetector.detect(img, bbox)) {
            if (bbox.cols == 4) {
                qrData->id = shm.data->id;
                qrData->height = shm.data->height;
                qrData->found = true;
                for (int i = 0; i < 4; i++) {
                    qrData->points[i] = Point(bbox.at<float>(2 * i), bbox.at<float>(2 * i + 1));
                }
            }
        }
    }

    static void findQrFromQueue(QRData *qrData, VideoData *videoData) {
        cv::Mat img(videoData->height, videoData->width, videoData->type, videoData->image);
        qrData->id = videoData->id;

        qrData->found = false;

        static QRCodeDetector qrDetector;
        static Mat bbox;

        if (qrDetector.detect(img, bbox)) {
            if (bbox.cols == 4) {
                qrData->id = videoData->id;
                qrData->height = videoData->height;
                qrData->found = true;
                for (int i = 0; i < 4; i++) {
                    qrData->points[i] = Point(bbox.at<float>(2 * i), bbox.at<float>(2 * i + 1));
                }
            }
        }

    }

    static void sendPosition(SharedMemoryGame &shm, GameData *gameData) {
        memcpy(shm.data, gameData, sizeof(GameData));
    }

    static double getPercent(QRData *data) {
        if (data == nullptr) {
            return 0.0;
        }
        int max_y = 0, min_y = data->height;
        for (auto &point : data->points) {
            max_y = std::max(max_y, point.y);
            max_y = std::min(max_y, data->height);
            min_y = std::min(min_y, point.y);
            min_y = std::max(min_y, 0);
        }
        int lower_bound = (max_y - min_y) / 2;
        int upper_bound = data->height - (max_y - min_y) / 2;
        int middle = (max_y + min_y) / 2;
        return (middle - lower_bound) / (double) (upper_bound - lower_bound);
    }
};

#endif //QRGAME_QRREADER_H