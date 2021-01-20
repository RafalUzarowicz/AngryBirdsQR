

#include <cstring>
#include "SharedQueueGame.h"
SharedQueueGame::SharedQueueGame( bool write, bool non_block, long msgNum) {
    auto flags = write?(non_block?O_WRONLY|O_NONBLOCK:O_WRONLY):(non_block?O_RDONLY|O_NONBLOCK:O_RDONLY);

    queue = mq_open(GAME_MQ, flags);
}
void SharedQueueGame::sendMsg(GameData* msg) const{
    mq_send(queue, (const char*)msg, sizeof(GameData), 0);
}

void SharedQueueGame::receiveMsg(GameData* msg) const{
    char buf [sizeof (GameData) /sizeof (char)]{};
    auto result = mq_receive(this->queue, &buf[0], sizeof(GameData), nullptr);
    if(result!=-1){
        memcpy(msg, buf, sizeof(GameData));
    }else{
        msg = nullptr;
    }
}
