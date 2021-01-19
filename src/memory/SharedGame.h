#ifndef QRGAME_SHAREDGAME_H
#define QRGAME_SHAREDGAME_H
#include <sys/mman.h>
class SharedGame: public IGame{
private:
    void* sh_memory;
public:
    SharedGame(int filedes, size_t length){
        sh_memory = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, filedes, 0);
        //initialize shared memory
    }
    void sendData(GameData* data){
        //todo send data
    }
    GameData* getData(){

    }
};

#endif //QRGAME_SHAREDGAME_H
