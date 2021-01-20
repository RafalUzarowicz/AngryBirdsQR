#ifndef QRGAME_UTIL_H
#define QRGAME_UTIL_H

#include <chrono>

const static int MAX_WIDTH = 1920;
const static int MAX_HEIGHT = 1080;
const static int PIXEL_SIZE = 3;

const char SEM_VIDEO_PROD[] =  "/video_prod";
const char SEM_VIDEO_CONS[] = "/video_cons";
const char  SEM_GAME_PROD[] =  "/game_prod";
const char  SEM_GAME_CONS[] =  "/game_cons";
const char GAME_MEM_NAME[] = "/game_mem";
const char VIDEO_MEM_NAME[] = "/video_mem";
const char GAME_MQ[] ="/game_mq";
const char VIDEO_MQ[] ="/video_mq";

enum CommunicationType{
    QUEUE,
    SHARED_MEMORY
};

struct GameData{
    int id{};
    double percentage{};
    std::chrono::system_clock::time_point timestamp{};
};

struct VideoData{
    int id{};
    int height{};
    int width{};
    int type;
    std::chrono::system_clock::time_point timestamp{};
    unsigned char image[MAX_HEIGHT * MAX_WIDTH * PIXEL_SIZE];
};

#endif //QRGAME_UTIL_H