//
// Created by asia on 23.01.2021.
//

#ifndef QRGAME_SHAREDQUEUETIMESTAMP_H
#define QRGAME_SHAREDQUEUETIMESTAMP_H


#include <mqueue.h>
#include <iostream>
#include <cstring>
#include "../Util.h"

class SharedQueueTimestamp {
public:
    SharedQueueTimestamp(const char * name, bool write){
        auto flags = write?O_WRONLY:O_RDONLY;
        queue = mq_open(name, flags);
        if(queue < 0){
            std::cerr << strerror(errno) << "\n";
            exit(1);
        }
        buf = new char[bufSize];
    }
    void sendMsg(LogMes *msg) const {
        mq_send(queue, (const char *) msg, sizeof(LogMes), 0);
    }

    int receiveMsg(LogMes *msg) const {
        auto result = mq_receive(this->queue, buf, bufSize, nullptr);
        if (result != -1) {
            memcpy(msg, buf, sizeof(LogMes));
        } else if (errno == EAGAIN) {
            msg->id = -1;
        } else {
            std::cerr << strerror(errno) << "\n";
            exit(1);
        }
        return result;
    }
private:
    mqd_t queue;
    char * buf;
    static const int bufSize = sizeof (LogMes) +2;
};


#endif //QRGAME_SHAREDQUEUETIMESTAMP_H
