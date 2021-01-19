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
public:
    explicit Core(){
        this->game = new Logic();
        this->window = new Window(game);
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

        double movement = 0.0;

        while(kill(getppid(), 0) == 0){
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;
            // Check resize
            window->resize();
            // TODO Read input

            // TODO Process input

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


#endif //QRGAME_CORE_H
