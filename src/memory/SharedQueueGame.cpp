

#include <cstring>
#include <iostream>
#include "SharedQueueGame.h"
SharedQueueGame::SharedQueueGame( bool write, bool non_block, long msgNum) {
    auto flags = write?(non_block?O_WRONLY|O_NONBLOCK:O_WRONLY):(non_block?O_RDONLY|O_NONBLOCK:O_RDONLY);
    errno = 0;
    queue = mq_open(GAME_MQ, flags);
    if(queue< 0){
        std::cout<<"Could not open "<<queue<<"\n";
        std::cout<<strerror(errno)<<"\n";
        exit(0);
    }
}
void SharedQueueGame::sendMsg(GameData* msg) const{
    mq_send(queue, (const char*)msg, sizeof(GameData), 0);
}

void SharedQueueGame::receiveMsg(GameData* msg) const{
    errno = 0;
    char buf [sizeof (GameData) /sizeof (char)]{};
    auto result = mq_receive(this->queue, &buf[0], sizeof(GameData), nullptr);
    if(result!=-1){
        memcpy(msg, buf, sizeof(GameData));
    }else{
        std::cout<<"Error reading msg\n";
        std::cout<<strerror(errno)<<"\n";
        msg = nullptr;
    }
}
