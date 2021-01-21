#include <cstring>
#include "SharedQueueVideo.h"

SharedQueueVideo::SharedQueueVideo(bool write, bool non_block, long msgNum){
    auto flags = write?(non_block?O_WRONLY|O_NONBLOCK:O_WRONLY):(non_block?O_RDONLY|O_NONBLOCK:O_RDONLY);

    mq_attr attr{};
    attr.mq_flags = flags;
    attr.mq_maxmsg = msgNum;
    attr.mq_msgsize = sizeof (VideoData);
    queue = mq_open(VIDEO_MQ, flags, attr);
}

void SharedQueueVideo::sendMsg(VideoData* msg) const{
    mq_send(queue, (const char*)msg, sizeof(VideoData), 0);
}

void SharedQueueVideo::receiveMsg(VideoData* msg) const{
    char buf [sizeof (VideoData) /sizeof (char)]{};
    auto result = mq_receive(this->queue, &buf[0], sizeof(VideoData), nullptr);
    if(result!=-1){
        memcpy(msg, buf, sizeof(VideoData));
    }else{
        msg = nullptr;
    }
}