#ifndef QRGAME_QRGAME_H
#define QRGAME_QRGAME_H

#include <unistd.h>
#include <csignal>
#include <wait.h>
#include <cstring>
#include "IProcess.h"
#include "Game.h"

class QRGame: public IProcess {
private:
    enum SchedulerMode : int {DEFAULT = SCHED_OTHER, FIFO = SCHED_FIFO, RR = SCHED_RR};

    SchedulerMode schedMode = SchedulerMode::DEFAULT;

    bool coreLimit = false;

    InputManager::Command command;
    InputManager* inputManager;
    Game* game;
    pid_t game_proc = 0, image_proc = 0, qr_proc = 0;
public:
    QRGame(){
        command = InputManager::NONE;
        inputManager = new InputManager();
    }
    ~QRGame(){
        this->end();
        delete inputManager;
    }
    void run() override{
        this->schedMenu();
        this->coresMenu();

        this->initializeWindow();
        this->setup();
        this->createChildren();
        this->runChildren();
        this->sched();

        // TODO ustaw szeregowanie - tutaj czy dynamicznie w trakcie dziaania programu?
        // TODO oczekuj na input z klawiatury z ncurses zeby wiedziec co masz robic i aktualizuj rzeczy na podstawie niego
        int status;
        while(command != InputManager::QUIT){
            // Update input
            inputManager->update();
            // Process input
            command = inputManager->getCommand();

            // TODO: scheduler


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

    void initializeWindow(){
        initscr();
        raw();
        cbreak();
        nodelay(stdscr, TRUE);
        keypad(stdscr, FALSE);
        noecho();
    }

    void createChildren(){
        // TODO utworz poszczegolne procesy i przekaz im mechanizmy synchro
        game = new Game();
    }

    void runChildren(){
        // TODO dodac pozostale procesy
        game_proc = runProcess(game);
    }

    void sched(){
        // TODO: ustaw szeregowanie itp
        sched_param params{99};
        int result = 0;

        result |= sched_setscheduler(game_proc, schedMode, &params);
        result |= sched_setscheduler(image_proc, schedMode, &params);
        result |= sched_setscheduler(qr_proc, schedMode, &params);

        if (result)
        {
            std::cout << strerror(errno) << '\n';
            exit(1);
        }

        if (coreLimit)
        {
            result = 0;
            cpu_set_t cpuSet;

            CPU_ZERO(&cpuSet);
            CPU_SET(2, &cpuSet);

            result |= sched_setaffinity(game_proc, sizeof(cpu_set_t), &cpuSet);

            CPU_ZERO(&cpuSet);
            CPU_SET(2, &cpuSet);

            result |= sched_setaffinity(image_proc, sizeof(cpu_set_t), &cpuSet);

            CPU_ZERO(&cpuSet);
            CPU_SET(2, &cpuSet);

            result |= sched_setaffinity(qr_proc, sizeof(cpu_set_t), &cpuSet);

            if (result)
            {
                std::cout << strerror(errno) << '\n';
                exit(1);
            }
        }
    }

    void setup(){
        this->unlink();
        // TODO: utworz semafory i kolejki
//        sem_t *producer = sem_open(SEM_PROD_NAME, O_CREAT, 0660, 0);
//        sem_t *consumer = sem_open(SEM_CONS_NAME, O_CREAT, 0660, 1);
//        sem_t *producer = sem_open(SEM_PROD_NAME, O_CREAT, 0660, 0);
//        sem_t *consumer = sem_open(SEM_CONS_NAME, O_CREAT, 0660, 1);
    }

    void unlink(){
        // TODO zamykanie wszystkiego itp
//        mq_unlink(MY_Q);
//        mq_unlink(LOG_Q);
//        shm_unlink(FILE_NAME);
//        sem_unlink(SEM_CONS_NAME);
//        sem_unlink(SEM_PROD_NAME);
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
        this->unlink();
    }

    void schedMenu()
    {
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

    void coresMenu()
    {
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
                    coreLimit = true;
                    return;
                default:
                    break;
            }

        } while (c != 'q');

        exit(0);
    }
};

#endif //QRGAME_QRGAME_H
