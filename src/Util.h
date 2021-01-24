#ifndef QRGAME_UTIL_H
#define QRGAME_UTIL_H

#define LOGGING_ENABLED //COMMENT/UNCOMMENT AS NEEDED

//#define DONT_USE_PROCESSES // COMMENT/UNCOMMENT IF NEEDED

#ifndef DONT_USE_PROCESSES // DONT DELETE AND DONT COMMENT THIS ONE

//#define NCURSES_INCLUDED // COMMENT/UNCOMMENT IF NEEDED

#endif // DONT DELETE AND DONT COMMENT THIS ONE

#define MENUS_ENABLED // COMMENT/UNCOMMENT IF NEEDED

#ifdef MENUS_ENABLED // DONT DELETE AND DONT COMMENT THIS ONE

#define SCHED_MENU_ENABLED // COMMENT/UNCOMMENT IF NEEDED
//#define CORE_MENU_ENABLED // COMMENT/UNCOMMENT IF NEEDED
#define IMAGE_QR_MENU_ENABLED // COMMENT/UNCOMMENT IF NEEDED
#define QR_GAME_MENU_ENABLED // COMMENT/UNCOMMENT IF NEEDED

#endif // DONT DELETE AND DONT COMMENT THIS ONE

#include <chrono>

const static int MAX_WIDTH = 800;
const static int MAX_HEIGHT = 600;
const static int PIXEL_SIZE = 3;

const static int MAX_QUEUE_MESS_SIZE = 8000;

const static int CAMERA_DEV_ID = 0;

const char SEM_VIDEO_PROD[] = "/video_prod";
const char SEM_VIDEO_CONS[] = "/video_cons";
const char SEM_GAME_PROD[] = "/game_prod";
const char SEM_GAME_CONS[] = "/game_cons";
const char GAME_MEM_NAME[] = "/game_mem";
const char VIDEO_MEM_NAME[] = "/video_mem";
const char GAME_MQ[] = "/game_mq";
const char VIDEO_MQ[] = "/video_mq";
const char VIDEO_LOG_MQ[] = "/video_log_mq";
const char GAME_LOG_MQ[] = "/game_log_mq";

const char VIDEO_LOG_FILE [] = "./video-log.csv";
const char GAME_LOG_FILE [] = "./game-log.csv";

const int BUF_NUM = 10;


enum CommunicationType {
    QUEUE,
    SHARED_MEMORY
};
enum LogType{
    VIDEO,
    GAME
};

struct LogMes{
    int id{};
    std::chrono::system_clock::time_point start{};
    std::chrono::system_clock::time_point end{};
};

struct GameData {
    int id{};
    double percentage{};
    std::chrono::system_clock::time_point timestamp{};
};

struct VideoData {
    int id{};
    int height{};
    int width{};
    int type;
    std::chrono::system_clock::time_point timestamp{};
    unsigned char image[MAX_HEIGHT * MAX_WIDTH * PIXEL_SIZE];
};

#endif //QRGAME_UTIL_H