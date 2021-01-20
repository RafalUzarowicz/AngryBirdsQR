#ifndef QRGAME_QRGAME_H
#define QRGAME_QRGAME_H

#include "Game.h"
#include <unistd.h>
#include <csignal>
#include <wait.h>
#include <cstring>
#include <semaphore.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/mman.h>
#include "IProcess.h"
#include "Util.h"
#include "qr/QRReader.h"
#include "ImageFactory.h"

class QRGame: public IProcess {
private:
    enum SchedulerMode : int {DEFAULT = SCHED_OTHER, FIFO = SCHED_FIFO, RR = SCHED_RR};

    SchedulerMode schedMode = SchedulerMode::DEFAULT;

    CommunicationType imageToQr = SHARED_MEMORY;
    CommunicationType qrToGame = SHARED_MEMORY;

    bool blockQueueVideoImage = true;
    bool blockQueueVideoQr = true;
    bool blockQueueGameQr = true;
    bool blockQueueGameGame = true;

    bool coreLimit = false;

    InputManager::Command command;
    InputManager* inputManager;
    Game* game;
    ImageFactory* imageFactory;
    QRReader* qrReader;
    pid_t game_proc = 0, image_proc = 0, qr_proc = 0;
public:
    QRGame(){
        command = InputManager::NONE;
        inputManager = new InputManager();
        game = nullptr;
        imageFactory = nullptr;
        qrReader = nullptr;
    }
    ~QRGame(){
        this->end();
        delete imageFactory;
        delete game;
        delete qrReader;
        delete inputManager;
    }
    void run() override{
        this->schedMenu();
        this->coresMenu();
        this->imagesToQrMenu();
        if(imageToQr == QUEUE){
            this->videoQueueImageTypeMenu();
            this->videoQueueQrTypeMenu();
        }
        this->qrToGameMenu();
        if(qrToGame == QUEUE){
            this->gameQueueQrTypeMenu();
            this->gameQueueGameTypeMenu();
        }
        QRGame::initializeWindow();
        QRGame::setup();
        this->createChildren();
        this->runChildren();
        this->sched();
        int status;
        while(command != InputManager::QUIT){
            // Update input
            inputManager->update();
            // Process input
            command = inputManager->getCommand();
            // Check if children are alive
            if(waitpid(0, &status, WNOHANG) != 0){
                break;
            }
        }
    }



private:
    static pid_t runProcess(IProcess* process){
        pid_t result = fork();
        if (result == 0) {
            process->run();
            exit(0);
        }else{
            return result;
        }
    }

    static void initializeWindow(){
        WINDOW * mainwin;
        if ( (mainwin = initscr()) == NULL ) {
            exit(EXIT_FAILURE);
        }
        raw();
        cbreak();
        nodelay(stdscr, TRUE);
        keypad(stdscr, FALSE);
        noecho();
    }

    void createChildren(){
        imageFactory = new ImageFactory();
        qrReader = new QRReader();
        game = new Game(qrToGame, blockQueueGameGame);
    }

    void runChildren(){
        image_proc = runProcess(imageFactory);
        qr_proc = runProcess(qrReader);
        game_proc = runProcess(game);
    }

    void sched(){
        sched_param params{};
        if(schedMode == DEFAULT){
            params.sched_priority = 0;
        }else{
            params.sched_priority = 99;
        }
        int result = 0;

        result |= sched_setscheduler(game_proc, schedMode, &params);
        result |= sched_setscheduler(image_proc, schedMode, &params);
        result |= sched_setscheduler(qr_proc, schedMode, &params);

        if (result)
        {
            std::cout << strerror(errno) << '\n';
            this->end();
            exit(1);
        }


        if (coreLimit)
        {
            result = 0;
            cpu_set_t cpuSet;

            CPU_ZERO(&cpuSet);
            CPU_SET(3, &cpuSet);

            result |= sched_setaffinity(game_proc, sizeof(cpu_set_t), &cpuSet);

            CPU_ZERO(&cpuSet);
            CPU_SET(3, &cpuSet);

            result |= sched_setaffinity(image_proc, sizeof(cpu_set_t), &cpuSet);

            CPU_ZERO(&cpuSet);
            CPU_SET(3, &cpuSet);

            result |= sched_setaffinity(qr_proc, sizeof(cpu_set_t), &cpuSet);

            if (result)
            {
                std::cout << strerror(errno) << '\n';
                this->end();
                exit(1);
            }
        }
    }

    static void setup(){
        QRGame::unlink();
        sem_open(SEM_VIDEO_PROD, O_CREAT, 0660, 0);
        sem_open(SEM_VIDEO_CONS, O_CREAT, 0660, 1);
        sem_open(SEM_GAME_PROD, O_CREAT, 0660, 0);
        sem_open(SEM_GAME_CONS, O_CREAT, 0660, 1);

        int gameDataSize = sizeof(GameData);
        int videoDataSize = sizeof(VideoData);

        mq_attr gameAttr{};
        gameAttr.mq_maxmsg = 1;
        gameAttr.mq_msgsize = gameDataSize;
        mq_open(GAME_MQ, O_CREAT | O_RDWR | O_NONBLOCK, 0660, gameAttr);

        mq_attr videoAttr{};
        videoAttr.mq_maxmsg = 1;
        videoAttr.mq_msgsize = videoDataSize;
        mq_open(VIDEO_MQ, O_CREAT | O_RDWR | O_NONBLOCK, 0660, videoAttr);

        int gameSgmFd = shm_open(GAME_MEM_NAME, O_CREAT | O_RDWR, 0660);
        ftruncate(gameSgmFd, gameDataSize);
        int videoSgmFd = shm_open(VIDEO_MEM_NAME, O_CREAT | O_RDWR, 0660);
        ftruncate(videoSgmFd, videoDataSize);
    }

    static void unlink(){
        mq_unlink(GAME_MQ);
        mq_unlink(VIDEO_MQ);
        sem_unlink(SEM_VIDEO_PROD);
        sem_unlink(SEM_VIDEO_CONS);
        sem_unlink(SEM_GAME_PROD);
        sem_unlink(SEM_GAME_CONS);
        shm_unlink(GAME_MEM_NAME);
        shm_unlink(VIDEO_MEM_NAME);
    }

    void killChildren() const{
        if(image_proc != 0){
            kill(image_proc, SIGTERM);
        }
        if(qr_proc != 0){
            kill(qr_proc, SIGTERM);
        }
        if(game_proc != 0){
            kill(game_proc, SIGTERM);
        }
    }

    void end() {
        this->killChildren();
        endwin();
        QRGame::unlink();
    }

    void schedMenu(){
        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. Default scheduling\n";
            std::cout << "2. FIFO scheduling\n";
            std::cout << "3. Round Robin scheduling\n";
            std::cout << "Select scheduling: ";

            std::cin >> c;

            switch (c) {
                case '1':
                    schedMode = SchedulerMode::DEFAULT;
                    return;
                case '2':
                    schedMode = SchedulerMode::FIFO;
                    return;
                case '3':
                    schedMode = SchedulerMode::RR;
                    return;
                default:
                    break;
            }
        } while (c != 'q');
        exit(0);
    }

    void coresMenu(){
        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. No core limit\n";
            std::cout << "2. Core limit - 2\n";
            std::cout << "Select core limit: ";

            std::cin >> c;

            switch (c) {
                case '1':
                    coreLimit = false;
                    return;
                case '2':
                    // TODO: zmienic na true i ogarnac te cpu
                    coreLimit = false;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }

    void imagesToQrMenu(){
        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. Shared memory\n";
            std::cout << "2. Queue\n";
            std::cout << "Select communication mechanism between image reading and image analysis: ";

            std::cin >> c;

            switch (c) {
                case '1':
                    imageToQr = SHARED_MEMORY;
                    return;
                case '2':
                    imageToQr = QUEUE;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }

    void videoQueueImageTypeMenu(){
        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. Blocking\n";
            std::cout << "2. Not blocking\n";
            std::cout << "Select queue type between image reading and image analysis for image reading side: ";

            std::cin >> c;

            switch (c) {
                case '1':
                    blockQueueVideoImage = true;
                    return;
                case '2':
                    blockQueueVideoImage = false;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }

    void videoQueueQrTypeMenu(){
        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. Blocking\n";
            std::cout << "2. Not blocking\n";
            std::cout << "Select queue type between image reading and image analysis for image analysis side: ";

            std::cin >> c;

            switch (c) {
                case '1':
                    blockQueueVideoQr = true;
                    return;
                case '2':
                    blockQueueVideoQr = false;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }

    void qrToGameMenu(){
        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. Shared memory\n";
            std::cout << "2. Queue\n";
            std::cout << "Select communication mechanism between image analysis and game: ";

            std::cin >> c;

            switch (c) {
                case '1':
                    qrToGame = SHARED_MEMORY;
                    return;
                case '2':
                    qrToGame = QUEUE;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }

    void gameQueueQrTypeMenu(){
        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. Blocking\n";
            std::cout << "2. Not blocking\n";
            std::cout << "Select queue type between image analysis and game for image analysis side: ";

            std::cin >> c;

            switch (c) {
                case '1':
                    blockQueueGameQr = true;
                    return;
                case '2':
                    blockQueueGameQr = false;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }

    void gameQueueGameTypeMenu(){
        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. Blocking\n";
            std::cout << "2. Not blocking\n";
            std::cout << "Select queue type between image analysis and game for game side: ";

            std::cin >> c;

            switch (c) {
                case '1':
                    blockQueueGameGame = true;
                    return;
                case '2':
                    blockQueueGameGame = false;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }
};

#endif //QRGAME_QRGAME_H
