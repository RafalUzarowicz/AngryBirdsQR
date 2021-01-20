#ifndef QRGAME_UTIL_H
#define QRGAME_UTIL_H

#include <chrono>

#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080
#define PIXEL_SIZE 3

const char SEM_VIDEO_PROD[] =  "/video_prod";
const char SEM_VIDEO_CONS[] = "/video_cons";
const char  SEM_GAME_CONS[] =  "/game_cons";
const char  SEM_GAME_PROD[] =  "/game_prod";
const char GAME_MEM_NAME[] = "/game_mem";
const char VIDEO_MEM_NAME[] = "/video_mem";
const char GAME_MQ[] ="/game_mq";
const char VIDEO_MQ[] ="/video_mq";
using namespace std::chrono;

//data structures
struct GameData{
    double percentage{};
    system_clock::time_point timestamp{};
    int id;
};

struct VideoData{
    int height{};
    int width{};
    system_clock::time_point timestamp{};
    unsigned char image[MAX_HEIGHT * MAX_WIDTH * PIXEL_SIZE];
    int id;
};

#endif //QRGAME_UTIL_H
