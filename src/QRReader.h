#ifndef QRGAME_QRREADER_H
#define QRGAME_QRREADER_H

#include "IProcess.h"

class QRReader: public IProcess {
public:
    void run() override{
        while(kill(getppid(), 0) == 0){

        }
    }
};


#endif //QRGAME_QRREADER_H
