#ifndef QRGAME_CORE_H
#define QRGAME_CORE_H


#include <ctime>
#include <chrono>
#include "Window.h"
#include "Logic.h"
#include "InputManager.h"
#include "../IProcess.h"

class Core: public IProcess {
private:
    Window* window;
    Logic* game;
    InputManager* inputManager;
    InputManager::Command command;

    const double FPS = 60.0;
    const double frameTime = 1000.0/FPS;
public:
    explicit Core(InputManager* inputManager) :{
        this->game = new Logic();
        this->window = new Window(game);
        this->inputManager = inputManager == nullptr ? new InputManager() : inputManager;
        this->command = InputManager::NONE;
    }

    ~Core(){
        delete window;
        delete game;
        delete inputManager;
    }

    void run() override{
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
            // Update input
            inputManager->update();
            // Process input
            command = inputManager->getCommand();
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
