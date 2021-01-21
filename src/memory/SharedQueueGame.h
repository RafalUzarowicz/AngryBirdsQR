#ifndef QRGAME_SHAREDQUEUEGAME_H
#define QRGAME_SHAREDQUEUEGAME_H

#include <mqueue.h>
#include <fcntl.h>
#include "../Util.h"

class SharedQueueGame {
public:
    SharedQueueGame( bool write, bool non_block) {
        auto flags = write?(non_block?O_WRONLY|O_NONBLOCK:O_WRONLY):(non_block?O_RDONLY|O_NONBLOCK:O_RDONLY);
        queue = mq_open(GAME_MQ, flags);
        if(queue< 0){
            std::cerr<<strerror(errno)<<"\n";
            exit(1);
        }

        buf = new char[sizeof (GameData) /sizeof (char)];
    }

    ~SharedQueueGame() = default;

    void sendMsg(GameData* msg) const{
        mq_send(queue, (const char*)msg, sizeof(GameData), 0);
    }

    void receiveMsg(GameData* msg) const{
        auto result = mq_receive(this->queue, buf, sizeof(GameData), nullptr);
        if(result!=-1){
            memcpy(msg, buf, sizeof(GameData));
        }else{
            std::cerr<<strerror(errno)<<"\n";
            exit(1);
        }
    }

private:
    mqd_t queue;
    char* buf;

};


#endif //QRGAME_SHAREDQUEUEGAME_H
