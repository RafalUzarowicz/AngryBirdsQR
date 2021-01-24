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
   /* void run(int bufNum){
        bufNum = std::max(1, bufNum);
        struct LogMes logMes[bufNum];
        int i= 0;
        //FIXME finish when a signal is send
        while(futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout){
            log_q.receiveMsg(&logMes[i]);

            if(i == bufNum){
                for (int k=0; k<bufNum;k++){
                    auto latency = duration_cast<milliseconds>(logMes[k].end - logMes[k].start).count();
                    output<<logMes[k].id<<","<<latency<<'\n';
                }
                i = 0;
            }
            ++i;
        }
        output.close();
    }*/
    void runOnce(){
        //std::cout<<"Logger running\n";
        if(log_q.receiveMsg(&logMes[i]) != -1){
            //std::cout<<"[L]:"<<logMes->id<<"\n";
            ++i;
        };
        //std::cout<<"Received Msg\n";
        if(i == bufNum){
            //std::cout<<"Saving data to file\n";
            for (int k=0; k<bufNum;k++){
                auto latency = duration_cast<milliseconds>(logMes[k].end - logMes[k].start).count();
                output<<logMes[k].id<<","<<latency<<'\n';
            }
            output<<std::flush;
            i = 0;
        }
    }
    void close(){
        //std::cout<<"Closing stream\n";
        output.close();
    }


};

#endif //QRGAME_LOGGER_H
