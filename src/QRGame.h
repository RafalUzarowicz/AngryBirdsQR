#ifndef QRGAME_QRGAME_H
#define QRGAME_QRGAME_H

#include "IProcess.h"
#include "game/InputManager.h"

class QRGame: public IProcess {
private:
    InputManager::Command command;
    InputManager* inputManager;
public:
    QRGame(){
        inputManager = new InputManager();
    }
    void run() override{
        // TODO utworz pamiec wspoldzielona/kolejki

        // TODO utworz poszczegolne procesy i przekaz im mechanizmy synchro

        // TODO ustaw szeregowanie

        // TODO oczekuj na input z klawiatury z ncurses zeby wiedziec co masz robic i aktualizuj rzeczy na podstawie niego

        while(command != InputManager::QUIT){
            // Update input
            inputManager->update();
            // Process input
            command = inputManager->getCommand();
        }
    }
};


#endif //QRGAME_QRGAME_H
