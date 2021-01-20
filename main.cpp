#include "src/QRGame.h"

int main()
{
    QRGame qrGame;

    qrGame.run();
//    QRReader qrReader(SHARED_MEMORY, SHARED_MEMORY, false, false);
//
//    qrReader.run();

//char game[] = "/xd";
//    unlink(GAME_MQ);
//
//    int gameDataSize = sizeof(GameData);
//    int videoDataSize = sizeof(VideoData);
//
//    struct mq_attr gameAttr;
//    gameAttr.mq_maxmsg = 1;
//    gameAttr.mq_msgsize = gameDataSize;
//    errno = 0;
//    int x = mq_open(GAME_MQ, O_CREAT | O_RDWR, 0660, &gameAttr);
//    std::cout<<strerror(errno)<<'\n';
//
//    unlink(GAME_MQ);

    return 0;
}