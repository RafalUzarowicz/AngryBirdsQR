#ifndef QRGAME_SHAREDQUEUEGAME_H
#define QRGAME_SHAREDQUEUEGAME_H

#include "../Util.h"

#include <mqueue.h>
#include <fcntl.h>

class SharedQueueGame {
public:
    SharedQueueGame(bool write, bool non_block) {
#ifndef DONT_USE_PROCESSES
        auto flags = write ? (non_block ? O_WRONLY | O_NONBLOCK : O_WRONLY) : (non_block ? O_RDONLY | O_NONBLOCK
                                                                                         : O_RDONLY);
#else
        auto flags = write?(O_WRONLY|O_NONBLOCK):(O_RDONLY|O_NONBLOCK);
#endif

        queue = mq_open(GAME_MQ, flags);
        if (queue < 0) {
            std::cerr << strerror(errno) << "\n";
            exit(1);
        }

        buf = new char[bufSize];
    }

    ~SharedQueueGame() {
        delete buf;
    }

    void sendMsg(GameData *msg) const {
        mq_send(queue, (const char *) msg, sizeof(GameData), 0);
    }

    void receiveMsg(GameData *msg) const {
        auto result = mq_receive(this->queue, buf, bufSize, nullptr);
        if (result != -1) {
            memcpy(msg, buf, sizeof(GameData));
        } else if (errno == EAGAIN) {
            msg->id = -1;
        } else {
            std::cerr << strerror(errno) << "\n";
            exit(1);
        }
    }

private:
    mqd_t queue;
    static const int bufSize = sizeof(GameData) + 2;
    char *buf;
};

#endif //QRGAME_SHAREDQUEUEGAME_H