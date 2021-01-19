

#ifndef QRGAME_SPRITE_H
#define QRGAME_SPRITE_H


#include <vector>
#include <string>

class Sprite {
public:
    Sprite(){
        sprite.emplace_back("    __");
        sprite.emplace_back("___( o)>");
        sprite.emplace_back("\\ <_. )");
        sprite.emplace_back(" `---'");

        length = 0;
        for(auto & i : sprite){
            length = i.length() > length ? i.length() : length;
        }
    }

    int length;

    std::vector<std::string> sprite;
};


#endif //QRGAME_SPRITE_H
