#ifndef QRGAME_SHAREDQUEUEVIDEO_H
#define QRGAME_SHAREDQUEUEVIDEO_H

#include "../Util.h"

#include <mqueue.h>

class SharedQueueVideo {
public:
    SharedQueueVideo(bool write, bool non_block) {
#ifndef DONT_USE_PROCESSES
        auto flags = write ? (non_block ? O_WRONLY | O_NONBLOCK : O_WRONLY) : (non_block ? O_RDONLY | O_NONBLOCK
                                                                                         : O_RDONLY);
#else
        auto flags = write?(O_WRONLY|O_NONBLOCK):(O_RDONLY|O_NONBLOCK);
#endif

        this->queue = mq_open(VIDEO_MQ, flags);
        if (queue < 0) {
            std::cerr << strerror(errno) << "\n";
            exit(1);
        }

        buf = new char[bufSize];
    }

    ~SharedQueueVideo() {
        delete buf;
    }

    void sendMsg(VideoData *msg) const {
        int result = 0;
        int offset = 0;
        while (offset < bufSize && (result != -1 || errno == EAGAIN)) {
            result = mq_send(this->queue, (const char *) msg + offset, MAX_QUEUE_MESS_SIZE, 0);
            if (result != -1) {
                offset += MAX_QUEUE_MESS_SIZE;
            } else if (errno != EAGAIN) {
                std::cerr << strerror(errno) << '\n';
                exit(1);
            }
        }
    }

    void receiveMsg(VideoData *msg) const {
        int offset = 0;
        int result = 0;
        while (offset < bufSize && (result != -1 || errno == EAGAIN)) {
            result = mq_receive(this->queue, buf + offset, MAX_QUEUE_MESS_SIZE + 1, nullptr);
            if (result != -1) {
                offset += MAX_QUEUE_MESS_SIZE;
            } else if (errno != EAGAIN) {
                std::cerr << strerror(errno) << '\n';
                exit(1);
            }
        }
        if (result != -1) {
            memcpy(msg, buf, sizeof(VideoData));
        } else if (errno == EAGAIN) {
            msg->id = -1;
        } else {
            std::cerr << strerror(errno) << "\n";
            exit(1);
        }
    }

private:
    mqd_t queue;
    char *buf;
    static const int bufSize = sizeof(VideoData) + sizeof(VideoData) % MAX_QUEUE_MESS_SIZE;

};

#endif //QRGAME_SHAREDQUEUEVIDEO_H