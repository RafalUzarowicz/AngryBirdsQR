#ifndef QRGAME_CORE_H
#define QRGAME_CORE_H


#include <ctime>
#include <chrono>
#include "Window.h"
#include "Game.h"
#include "InputManager.h"

class Core {
private:
    Window* window;
    Game* game;
    InputManager* inputManager;
    InputManager::Command command;

    const double FPS = 60.0;
    const double frameTime = 1000.0/FPS;
public:
    Core(InputManager* inputManager = nullptr){
        this->game = new Game();
        this->window = new Window(game);
        this->inputManager = inputManager == nullptr ? new InputManager() : inputManager;
        this->command = InputManager::NONE;
    }

    ~Core(){
        delete window;
        delete game;
        delete inputManager;
    }

    void run(){
        window->initialize();
        game->initialize(window->getHeight(), window->getWidth());

        double lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double deltaTime = 0, nowTime = 0;
        double dTime = 0, lTime = lastTime;

        while(command != InputManager::QUIT){
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;
            dTime = nowTime - lTime;
            // Check resize
            window->resize();
            // Update input
            inputManager->update();
            // Process input
            command = inputManager->getCommand();
            // Update game
            game->update(command, window->getHeight(), window->getWidth(), deltaTime / 1000.0);

            if(dTime > frameTime){
                // Clear screen
                window->clear();
                // Draw
                window->draw();

                lTime = nowTime;
            }
            lastTime = nowTime;
        }
    }
};


#endif //QRGAME_CORE_H
