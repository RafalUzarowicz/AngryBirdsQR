#ifndef QRGAME_CORE_H
#define QRGAME_CORE_H

#include <ctime>
#include <chrono>
#include "Window.h"
#include "Logic.h"
#include "../IProcess.h"

class Core{
private:
    Window* window;
    Logic* game;
    InputManager::Command command;

    const double FPS = 60.0;
    const double frameTime = 1000.0/FPS;
public:
    explicit Core(){
        this->game = new Logic();
        this->window = new Window(game);
        this->command = InputManager::NONE;
    }

    ~Core(){
        delete window;
        delete game;
    }

    void run(){
        window->initialize();
        game->initialize(window->getHeight(), window->getWidth());

        double lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double deltaTime, nowTime;
        double dTime, lTime = lastTime;

        while(command != InputManager::QUIT){
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;
            dTime = nowTime - lTime;
            // Check resize
            window->resize();
            // TODO Read input

            // TODO Process input

            // Update game
            game->update(command, window->getHeight(), window->getWidth(), deltaTime / 1000.0);

            if(dTime > frameTime){
                // Clear screen
                Window::clear();
                // Draw
                window->draw();

                lTime = nowTime;
            }
            lastTime = nowTime;
        }
    }
};


#endif //QRGAME_CORE_H
