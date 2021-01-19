#ifndef QRGAME_QRGAME_H
#define QRGAME_QRGAME_H

#include <unistd.h>
#include <csignal>
#include <wait.h>
#include "IProcess.h"
#include "Game.h"

class QRGame: public IProcess {
private:
    InputManager::Command command;
    InputManager* inputManager;
    pid_t game_proc = 0, image_proc = 0, qr_proc = 0, log1_proc = 0, log2_proc = 0;
public:
    QRGame(){
        inputManager = new InputManager();
        QRGame::initialize();
    }
    ~QRGame(){
        this->end();
        delete inputManager;
    }
    void run() override{
        // TODO utworz pamiec wspoldzielona/kolejki

        // TODO utworz poszczegolne procesy i przekaz im mechanizmy synchro
        Game* game = new Game();


        // TODO ustaw szeregowanie - tutaj czy dynamicznie w trakcie dziaania programu?

        // TODO uruchom procesy dzieci
        game_proc = runProcess(game);


        // TODO oczekuj na input z klawiatury z ncurses zeby wiedziec co masz robic i aktualizuj rzeczy na podstawie niego
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

    static void initialize(){
        initscr();
        raw();
        cbreak();
        nodelay(stdscr, TRUE);
        keypad(stdscr, FALSE);
        noecho();
    }

    void end() const{
        if(image_proc != 0){
            kill(image_proc, SIGTERM);
        }
        if(qr_proc != 0){
            kill(qr_proc, SIGTERM);
        }
        if(game_proc != 0){
            kill(game_proc, SIGTERM);
        }
        if(log1_proc != 0){
            kill(log1_proc, SIGTERM);
        }
        if(log2_proc != 0){
            kill(log2_proc, SIGTERM);
        }
        endwin();
    }


};


#endif //QRGAME_QRGAME_H
