#ifndef QRGAME_QRGAME_H
#define QRGAME_QRGAME_H

#include "QRReader.h"
#include "ImageFactory.h"
#include "IProcess.h"
#include "Game.h"
#include "Util.h"

#include <unistd.h>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <semaphore.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <ctime>
#include <sys/resource.h>

class QRGame : public IProcess {
private:
    enum SchedulerMode : int {
        DEFAULT = SCHED_OTHER, FIFO = SCHED_FIFO, RR = SCHED_RR
    };

    SchedulerMode schedMode = SchedulerMode::DEFAULT;
    CommunicationType imageToQr = SHARED_MEMORY;
    CommunicationType qrToGame = SHARED_MEMORY;
    bool blockQueueVideoImage = false;
    bool blockQueueVideoQr = false;
    bool blockQueueGameQr = false;
    bool blockQueueGameGame = false;
    bool coreLimit = false;
    InputManager::Command command;
    InputManager *inputManager;
    Game *game;
    ImageFactory *imageFactory;
    QRReader *qrReader;
    pid_t game_proc = 0, image_proc = 0, qr_proc = 0;
    int affinity = 1;

#ifdef DONT_USE_PROCESSES // DONT DELETE AND DONT COMMENT THIS ONE
    static const int MAX_ITERATIONS = 100;
#endif // DONT DELETE AND DONT COMMENT THIS ONE
public:
    QRGame() {
        command = InputManager::NONE;
        inputManager = new InputManager();
        game = nullptr;
        imageFactory = nullptr;
        qrReader = nullptr;
    }

    ~QRGame() {
        this->end();
        delete imageFactory;
        delete game;
        delete qrReader;
        delete inputManager;
    }

    void run() override {
#ifdef MENUS_ENABLED // DONT DELETE AND DONT COMMENT THIS ONE

#ifdef SCHED_MENU_ENABLED // DONT DELETE AND DONT COMMENT THIS ONE
        this->schedMenu();
#endif // DONT DELETE AND DONT COMMENT THIS ONE
#ifdef CORE_MENU_ENABLED // DONT DELETE AND DONT COMMENT THIS ONE
        this->coresMenu();
#endif // DONT DELETE AND DONT COMMENT THIS ONE
#ifdef IMAGE_QR_MENU_ENABLED // DONT DELETE AND DONT COMMENT THIS ONE
        this->imagesToQrMenu();
        if (imageToQr == QUEUE) {
            this->videoQueueImageTypeMenu();
            this->videoQueueQrTypeMenu();
        }
#endif // DONT DELETE AND DONT COMMENT THIS ONE
#ifdef QR_GAME_MENU_ENABLED // DONT DELETE AND DONT COMMENT THIS ONE
        this->qrToGameMenu();
        if (qrToGame == QUEUE) {
            this->gameQueueQrTypeMenu();
            this->gameQueueGameTypeMenu();
        }
#endif // DONT DELETE AND DONT COMMENT THIS ONE

#endif // DONT DELETE AND DONT COMMENT THIS ONE

        QRGame::setup();
        this->createChildren();

#ifndef DONT_USE_PROCESSES // DONT DELETE AND DONT COMMENT THIS ONE

#ifdef NCURSES_INCLUDED // DONT DELETE AND DONT COMMENT THIS ONE
        QRGame::initializeWindow();
#endif // DONT DELETE AND DONT COMMENT THIS ONE

        this->runChildren();
        this->sched();
        int status;
        while (command != InputManager::QUIT) {
            // Update input
            inputManager->update();
            // Process input
            command = inputManager->getCommand();
            // Check if children are alive
            if (waitpid(0, &status, WNOHANG) != 0) {
                break;
            }
        }
#else // DONT DELETE AND DONT COMMENT THIS ONE
        for(int i = 0; i < MAX_ITERATIONS; ++i){
            imageFactory->run();
            qrReader->run();
            game->run();
        }
#endif // DONT DELETE AND DONT COMMENT THIS ONE
    }


private:
    static pid_t runProcess(IProcess *process) {
        pid_t result = fork();
        if (result == 0) {
            process->run();
            exit(0);
        } else {
            return result;
        }
    }

    static void initializeWindow() {
        if (initscr() == nullptr) {
            exit(EXIT_FAILURE);
        }
        raw();
        cbreak();
        nodelay(stdscr, TRUE);
//        keypad(stdscr, FALSE);
        noecho();
    }

    void createChildren() {
        imageFactory = new ImageFactory(imageToQr, blockQueueVideoImage);
        qrReader = new QRReader(imageToQr, qrToGame, blockQueueVideoQr, blockQueueGameQr);
        game = new Game(qrToGame, blockQueueGameGame);
    }

    void runChildren() {
        image_proc = runProcess(imageFactory);
        qr_proc = runProcess(qrReader);
        game_proc = runProcess(game);
    }

    void sched() {
        sched_param params{};
        if (schedMode == DEFAULT) {
            params.sched_priority = 0;
        } else if (schedMode == FIFO) {
            params.sched_priority = 99;
        } else if (schedMode == RR) {
            params.sched_priority = 99;
        }
        int result = 0;

        int policy = schedMode;

        result |= sched_setscheduler(game_proc, policy, &params);
        result |= sched_setscheduler(image_proc, policy, &params);
        result |= sched_setscheduler(qr_proc, policy, &params);

        if (result) {
            std::cout << strerror(errno) << '\n';
            this->end();
            exit(1);
        }


        if (coreLimit) {
            // FIXME to nie dziala
            result = 0;
            cpu_set_t set;

            CPU_ZERO(&set);
            CPU_SET(affinity, &set);

            result = sched_setaffinity(game_proc, sizeof(set), &set);
            if (result < 0) {
                std::cout << strerror(errno) << '\n';
                this->end();
                exit(1);
            }

            CPU_ZERO(&set);
            CPU_SET(affinity, &set);

            result = sched_setaffinity(image_proc, sizeof(set), &set);
            if (result < 0) {
                std::cout << strerror(errno) << '\n';
                this->end();
                exit(1);
            }

            CPU_ZERO(&set);
            CPU_SET(affinity, &set);

            result = sched_setaffinity(qr_proc, sizeof(set), &set);
            if (result < 0) {
                std::cout << strerror(errno) << '\n';
                this->end();
                exit(1);
            }
        }
    }

    static void setup() {

        QRGame::unlink();
        if (sem_open(SEM_VIDEO_PROD, O_CREAT, 0660, 1) == SEM_FAILED) {
            std::cerr << strerror(errno) << "\n";
            unlink();
            exit(1);
        }
        if (sem_open(SEM_VIDEO_CONS, O_CREAT, 0660, 0) == SEM_FAILED) {
            std::cerr << strerror(errno) << "\n";
            unlink();
            exit(1);
        }
        if (sem_open(SEM_GAME_PROD, O_CREAT, 0660, 1) == SEM_FAILED) {
            std::cerr << strerror(errno) << "\n";
            unlink();
            exit(1);
        }
        if (sem_open(SEM_GAME_CONS, O_CREAT, 0660, 0) == SEM_FAILED) {
            std::cerr << strerror(errno) << "\n";
            unlink();
            exit(1);
        }

        int gameDataSize = sizeof(GameData) + 1;
        int videoDataSize = sizeof(VideoData) + 1;

        mq_attr gameAttr{};
        gameAttr.mq_maxmsg = 1;
        gameAttr.mq_msgsize = gameDataSize;
        gameAttr.mq_curmsgs = 0;
        if (mq_open(GAME_MQ, O_CREAT | O_RDWR | O_NONBLOCK, 0660, &gameAttr) == (mqd_t) -1) {
            std::cerr << strerror(errno) << "\n";
            unlink();
            exit(1);
        }
        mq_attr videoAttr{};
        videoAttr.mq_maxmsg = 10;
        videoAttr.mq_msgsize = MAX_QUEUE_MESS_SIZE;
        videoAttr.mq_curmsgs = 0;
        if (mq_open(VIDEO_MQ, O_CREAT | O_RDWR | O_NONBLOCK, 0660, &videoAttr) == (mqd_t) -1) {
            std::cerr << strerror(errno) << "\n";
            unlink();
            exit(1);
        }

        int gameSgmFd = shm_open(GAME_MEM_NAME, O_CREAT | O_RDWR, 0660);
        if (gameSgmFd < 0) {
            std::cerr << strerror(errno) << "\n";
            unlink();
            exit(1);
        }
        ftruncate(gameSgmFd, gameDataSize);
        int videoSgmFd = shm_open(VIDEO_MEM_NAME, O_CREAT | O_RDWR, 0660);
        if (videoSgmFd < 0) {
            std::cerr << strerror(errno) << "\n";
            unlink();
            exit(1);
        }
        ftruncate(videoSgmFd, videoDataSize);

#ifdef LOGGING_ENABLED
        auto logMesSize = sizeof(LogMes) + 2;
        mq_attr gameLog{};
        gameLog.mq_maxmsg = 10;
        gameLog.mq_msgsize = logMesSize;
        gameLog.mq_curmsgs = 0;
        errno = 0;
        if (mq_open(GAME_LOG_MQ, O_CREAT | O_RDWR | O_NONBLOCK, 0660, &gameLog) == (mqd_t) -1) {
            std::cerr << "Could not create game logging queue: "<<strerror(errno) << "\n";
            unlink();
            exit(1);
        }
        errno = 0;
        mq_attr videoLog{};
        videoLog.mq_maxmsg = 10;
        videoLog.mq_msgsize = logMesSize;
        videoLog.mq_curmsgs = 0;
        if(mq_open(VIDEO_LOG_MQ, O_CREAT|O_RDWR, 0660, &videoLog)== (mqd_t)-1){
            std::cerr<<"Could not create video logging queue: "<<strerror(errno)<<"\n";
            unlink();
            exit(1);
        }
#endif //LOGGING_ENABLED

    }

    static void unlink() {
        mq_unlink(GAME_MQ);
        mq_unlink(VIDEO_MQ);
        sem_unlink(SEM_VIDEO_PROD);
        sem_unlink(SEM_VIDEO_CONS);
        sem_unlink(SEM_GAME_PROD);
        sem_unlink(SEM_GAME_CONS);
        shm_unlink(GAME_MEM_NAME);
        shm_unlink(VIDEO_MEM_NAME);
#ifdef LOGGING_ENABLED
        mq_unlink(GAME_LOG_MQ);
        mq_unlink(VIDEO_LOG_MQ);
#endif //LOGGING_ENABLED
    }

    void killChildren() const {
        if (image_proc != 0) {
            kill(image_proc, SIGTERM);
        }
        if (qr_proc != 0) {
            kill(qr_proc, SIGTERM);
        }
        if (game_proc != 0) {
            kill(game_proc, SIGTERM);
        }
    }

    void end() {
        this->killChildren();
        endwin();
        QRGame::unlink();
    }

    void schedMenu() {
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

    void coresMenu() {
        // FIXME to jest zdobywanie affinity ale nic nie daje :( albo nie wiem ze daje
//        cpu_set_t mask;
//        if (sched_getaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
//            std::cerr<<strerror(errno)<<'\n';
//            exit(1);
//        }
//        long nproc = sysconf(_SC_NPROCESSORS_ONLN);
//        int affinities[nproc];
//        for (int i = 0; i < nproc; i++) {
//            affinities[i] = CPU_ISSET(i, &mask);
//        }

        char c;
        do {
            std::cout << "q - Quit\n";
            std::cout << "1. No core limit\n";
            std::cout << "2. Core limit - 2\n";
            std::cout << "Select core limit: ";

            std::cin >> c;
            affinity = 1;

            switch (c) {
                case '1':
                    coreLimit = false;
                    return;
                case '2':
                    // FIXME zmienic na true i ogarnac te cpu
                    coreLimit = false;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }

    void imagesToQrMenu() {
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

    void videoQueueImageTypeMenu() {
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

    void videoQueueQrTypeMenu() {
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

    void qrToGameMenu() {
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

    void gameQueueQrTypeMenu() {
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

    void gameQueueGameTypeMenu() {
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