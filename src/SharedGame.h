//
// Created by asia on 18.01.2021.
//

#ifndef QRGAME_SHAREDGAME_H
#define QRGAME_SHAREDGAME_H
#
class SharedGame: public IGame{
public:
    SharedGame(){
        //initialize shared memory
    }
    void sendData(GameData data){
        //todo send data
    }
    GameData getData(){

    }
};

#endif //QRGAME_SHAREDGAME_H
