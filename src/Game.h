#ifndef QRGAME_GAME_H
#define QRGAME_GAME_H

#include "IProcess.h"
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <csignal>
#include "game/Window.h"
#include "game/Logic.h"
#include "IProcess.h"
#include "memory/SharedMemoryGame.h"
#include "memory/SharedQueueGame.h"
#include "Util.h"

class Game: public IProcess {
private:
    Window* window;
    Logic* game;
    SharedMemoryGame sharedMemory;
    SharedQueueGame sharedQueue;
    CommunicationType communicationType;

public:
    explicit Game(CommunicationType communicationType, bool isQueueBlocking = false) : communicationType(communicationType), sharedQueue(false, isQueueBlocking){
        this->game = new Logic();
        this->window = new Window(game);
    }

    ~Game(){
        delete window;
        delete game;
    }

    void run() override{
        window->initialize();
        game->initialize(window->getHeight(), window->getWidth());

        double lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double deltaTime, nowTime;

        double movement = 0.0;

        while(kill(getppid(), 0) == 0){
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;

            // Check resize
            window->resize();
            // TODO Read input
            if(communicationType == QUEUE){

            }else{

            }
            // TODO Process input

            // TODO time stamp save
            // Update game
            game->update(movement, window->getHeight(), window->getWidth(), deltaTime / 1000.0);

            // Clear screen
            Window::clear();
            // Draw
            window->draw();

            lastTime = nowTime;
        }
    }

};


#endif //QRGAME_GAME_H
