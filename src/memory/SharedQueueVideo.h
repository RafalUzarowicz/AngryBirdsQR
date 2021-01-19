//
// Created by asia on 19.01.2021.
//

#ifndef QRGAME_SHAREDQUEUEVIDEO_H
#define QRGAME_SHAREDQUEUEVIDEO_H

#include <mqueue.h>
#include "../Common.h"
class SharedQueueVideo {
public:
    SharedQueueVideo(bool write, bool non_block, long msgNum=1);
    ~SharedQueueVideo();

    void sendMsg(VideoData* msg) const;

    void receiveMsg(VideoData* msg) const;
private:
    mqd_t queue;
};


#endif //QRGAME_SHAREDQUEUEVIDEO_H
