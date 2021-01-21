#ifndef QRGAME_INPUTMANAGER_H
#define QRGAME_INPUTMANAGER_H

#include <ncurses.h>
#include "Util.h"

class InputManager {
public:
    enum Command{
        QUIT,
        NONE
    };

    InputManager()= default;

    void update(){
#ifdef NCURSES_INCLUDED
        int c = getch();
#else
        char c = 0;
        std::cout<<" q - quit\n";
        std::cout<<"Command: ";
        std::cin>>c;
#endif
        if(c == 'Q' || c == 'q'){
            current = QUIT;
        } else {
            current = NONE;
        }
    }

    Command getCommand(){
        return this->current;
    }
private:
    Command current;
};


#endif //QRGAME_INPUTMANAGER_H
