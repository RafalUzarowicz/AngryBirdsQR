//
// Created by asia on 23.01.2021.
//

#ifndef QRGAME_LOGGER_H
#define QRGAME_LOGGER_H
#include "Util.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include "./memory/SharedQueueTimestamp.h"
using namespace std::chrono;
class Logger{
private:
    std::ofstream output;
    SharedQueueTimestamp log_q;
    struct LogMes * logMes;
    int i;
    int bufNum;

public:
    Logger(const char * queueName, LogType type): log_q(queueName, false){
        if (type == VIDEO){
            output.open(VIDEO_LOG_FILE);
            output<<"id,timestamp\n";
        }else if(type == GAME){
            output.open(GAME_LOG_FILE);
            output<<"id,timestamp\n";
        }
        bufNum = std::max(1, BUF_NUM);
        logMes =  new struct LogMes[bufNum];
        i = 0;
    }
    void runOnce(){
        if(log_q.receiveMsg(&logMes[i]) != -1){
            ++i;
        };
        if(i == bufNum){
            for (int k=0; k<bufNum;k++){
                auto latency = duration_cast<microseconds>(logMes[k].end - logMes[k].start).count();
                output<<logMes[k].id<<","<<latency<<'\n';
            }
            output<<std::flush;
            i = 0;
        }
    }
    void close(){
        output.close();
    }


};

#endif //QRGAME_LOGGER_H
