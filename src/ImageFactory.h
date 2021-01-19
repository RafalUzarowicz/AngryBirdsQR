#ifndef QRGAME_IMAGEFACTORY_H
#define QRGAME_IMAGEFACTORY_H

#include "IProcess.h"
#include "image/CameraHandler.h"

class ImageFactory: public IProcess {
public:
    void run() override{

        CameraHandler * handler = new CameraHandler();

        handler->setWidth(640);
        handler->setHeight(480);

        unsigned char * buffer = new unsigned char[handler->getHeight() * handler->getWidth() * 3];

        // externalBuffer.resize(handler->getHeight * handler->getWidth * 3);
        handler->setFilename(OUTPUT_FILENAME);
        handler->setFrameDelay(2000);
        
        SharedMemory &shm;
        while(kill(getppid(), 0) == 0){
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;

            // Capture image
            buffer = handler->buffer;
            shm->sendData();


            lastTime = nowTime;
        }

    }

    void getImage()
};


#endif //QRGAME_IMAGEFACTORY_H
