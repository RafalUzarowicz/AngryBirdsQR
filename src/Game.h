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

    bool isBlocking = true;

    constexpr double static const step = 0.1;
    constexpr double static const maxStep = 0.05;

public:
    explicit Game(CommunicationType communicationType, bool isQueueBlocking = false) : communicationType(communicationType), sharedQueue(false, isQueueBlocking){
        this->game = new Logic();
        this->window = new Window(game);
        if(communicationType == SHARED_MEMORY){
            isBlocking = true;
        }else{
            isBlocking = isQueueBlocking;
        }
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
        GameData data;

        while(kill(getppid(), 0) == 0){
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;

            // Check resize
            window->resize();
            // Read input
            if(communicationType == SHARED_MEMORY){
                sharedMemory.getData(readData, sharedMemory, &data);
            }else{
                // TODO  queue

            }
            // Process input
            if(std::abs(movement - data.percentage) > maxStep){
                if(data.percentage > movement){
                    movement += maxStep;
                }else{
                    movement -= maxStep;
                }
            }else{
                movement = data.percentage;
            }
            // TODO time stamp save


            // Update game
            if(isBlocking){
                game->update(movement, window->getHeight(), window->getWidth(), 0.1);
            }else{
                game->update(movement, window->getHeight(), window->getWidth(), deltaTime / 1000.0);
            }

            // Clear screen
            Window::clear();
            // Draw
            window->draw();

            lastTime = nowTime;
        }
    }

private:
    static void readData(SharedMemoryGame& sharedMemoryGame, GameData* data){
        memcpy(data, sharedMemoryGame.data, sizeof(GameData));
    }

};


#endif //QRGAME_GAME_H
