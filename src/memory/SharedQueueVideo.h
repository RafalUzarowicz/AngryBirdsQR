#ifndef QRGAME_SHAREDQUEUEVIDEO_H
#define QRGAME_SHAREDQUEUEVIDEO_H

#include <mqueue.h>
#include "../Util.h"
class SharedQueueVideo {
public:
    SharedQueueVideo(bool write, bool non_block){
        auto flags = write?(non_block?O_WRONLY|O_NONBLOCK:O_WRONLY):(non_block?O_RDONLY|O_NONBLOCK:O_RDONLY);

        queue = mq_open(VIDEO_MQ, flags);
        if(queue< 0){
            std::cerr<<strerror(errno)<<"\n";
            exit(1);
        }

        buf = new char[sizeof (VideoData) /sizeof (char)];
    }

    ~SharedQueueVideo()=default;

    void sendMsg(VideoData* msg) const{
        mq_send(queue, (const char*)msg, sizeof(VideoData), 0);
    }

    void receiveMsg(VideoData* msg) const{
        auto result = mq_receive(this->queue, this->buf, sizeof(VideoData), nullptr);
        if(result!=-1){
            memcpy(msg, buf, sizeof(VideoData));
        }else{
            std::cerr<<strerror(errno)<<"\n";
            exit(1);
        }
    }
private:
    mqd_t queue;
    char* buf;
};


#endif //QRGAME_SHAREDQUEUEVIDEO_H
