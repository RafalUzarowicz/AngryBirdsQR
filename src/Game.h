#ifndef QRGAME_GAME_H
#define QRGAME_GAME_H

#include "IProcess.h"
#include "game/Core.h"

class Game: public IProcess {
    Core* core;
public:
    Game(){
        core = new Core();
    }
    ~Game(){
        delete core;
    }
    void run() override{
        core->run();
    }

};


#endif //QRGAME_GAME_H
