#ifndef QRGAME_INPUTMANAGER_H
#define QRGAME_INPUTMANAGER_H

#include <ncurses.h>

class InputManager {
public:
    enum Command{
        UP,
        DOWN,
        LEFT,
        RIGHT,
        QUIT,
        NONE
    };

    InputManager()= default;

    void update(){
        int c = getch();

        if(c == 'Q' || c == 'q'){
            current = QUIT;
        } else if( c == 'w' || c == 'W'){
            current = UP;
        } else if( c == 's' || c == 'S'){
            current = DOWN;
        } else if( c == 'a' || c == 'A'){
            current = LEFT;
        } else if( c == 'd' || c == 'D'){
            current = RIGHT;
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
