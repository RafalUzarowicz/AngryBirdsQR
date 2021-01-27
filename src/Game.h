#ifndef QRGAME_GAME_H
#define QRGAME_GAME_H

#include "IProcess.h"
#include "game/Window.h"
#include "game/Logic.h"
#include "IProcess.h"
#include "memory/SharedMemoryGame.h"
#include "memory/SharedQueueGame.h"
#include "Util.h"

#include <ctime>
#include <chrono>
#include <unistd.h>
#include <csignal>

class Game : public IProcess {
private:
    Window *window;
    Logic *game;
    SharedMemoryGame sharedMemory;
    SharedQueueGame sharedQueue;
    CommunicationType communicationType;

    bool isBlocking = true;

    constexpr double static const step = 0.1;
    constexpr double static const maxStep = 0.1;
    constexpr double static const blockingSpeed = 0.3;
#ifdef LOGGING_ENABLED
    SharedQueueTimestamp* mq_game_log;
    std::promise<void> exitSignal;
#endif //LOGGING ENABLED

public:
    explicit Game(CommunicationType communicationType, bool isQueueBlocking = false) : communicationType(
            communicationType), sharedQueue(false, !isQueueBlocking) {
        this->game = new Logic();
        this->window = new Window(game);
        if (communicationType == SHARED_MEMORY) {
            isBlocking = true;
        } else {
            isBlocking = isQueueBlocking;
        }
#ifdef LOGGING_ENABLED
        mq_game_log = new SharedQueueTimestamp(GAME_LOG_MQ, true);
#endif //LOGGING_ENABLED
    }

    ~Game() {
        delete window;
        delete game;
    }

    void run() override {
#ifdef LOGGING_ENABLED
        std::future<void> futureObj = exitSignal.get_future();
        std::thread thread (&runLogger, std::move(futureObj));
#endif //LOGGING_ENABLED
#ifdef NCURSES_INCLUDED // DONT DELETE AND DONT COMMENT THIS ONE
        window->initialize();
#endif // DONT DELETE AND DONT COMMENT THIS ONE
        game->initialize(window->getHeight(), window->getWidth());

        double lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        double deltaTime, nowTime;

        double movement = 0.0;
        GameData data;
        int prev_id = 0;
        LogMes logMes{};
#ifndef DONT_USE_PROCESSES // DONT DELETE AND DONT COMMENT THIS ONE
        while (kill(getppid(), 0) == 0) {
#endif // DONT DELETE AND DONT COMMENT THIS ONE
            // Measure time
            nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            deltaTime = nowTime - lastTime;
#ifdef NCURSES_INCLUDED // DONT DELETE AND DONT COMMENT THIS ONE
            // Check resize
            window->resize();
#endif // DONT DELETE AND DONT COMMENT THIS ONE
            // Read input
            if (communicationType == SHARED_MEMORY) {
                sharedMemory.getData(readData, sharedMemory, &data);
            } else {
                sharedQueue.receiveMsg(&data);
            }
            logMes.end =std::chrono::system_clock::now();

#ifdef LOGGING_ENABLED
            if(data.id != prev_id && data.id >0){
                logMes.id = data.id;
                logMes.start = data.timestamp;
                mq_game_log->sendMsg(&logMes);
                prev_id = logMes.id;
            }
#endif
            if (data.id >= 0) {
                // Process input
                if (std::abs(movement - data.percentage) > maxStep) {
                    if (data.percentage > movement) {
                        movement += maxStep;
                    } else {
                        movement -= maxStep;
                    }
                } else {
                    movement = data.percentage;
                }
            }

#ifndef NCURSES_INCLUDED // DONT DELETE AND DONT COMMENT THIS ONE
            std::cout << data.percentage << '\n';
#endif // DONT DELETE AND DONT COMMENT THIS ONE

            // Update game
            if (isBlocking) {
                game->update(movement, window->getHeight(), window->getWidth(), blockingSpeed);
            } else {
                game->update(movement, window->getHeight(), window->getWidth(), deltaTime / 1000.0);

            }
#ifdef NCURSES_INCLUDED // DONT DELETE AND DONT COMMENT THIS ONE
            // Clear screen
            Window::clear();
            // Draw
            window->draw();
#endif // DONT DELETE AND DONT COMMENT THIS ONE
            lastTime = nowTime;
#ifndef DONT_USE_PROCESSES // DONT DELETE AND DONT COMMENT THIS ONE
        }
#endif // DONT DELETE AND DONT COMMENT THIS ONE
#ifdef LOGGING_ENABLED
        std::cout<<"Sending close signal\n";
        exitSignal.set_value();
        std::cout<<"Waiting for thread to join\n";
        thread.join();
#endif //LOGGING ENABLED
    }

private:
    static void runLogger(std::future<void> futureObj){
        Logger logger(GAME_LOG_MQ, GAME);
        while(futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout){
            logger.runOnce();
        }
        std::cout<<"Got close signal\n";
        logger.close();
    }
    static void readData(SharedMemoryGame &sharedMemoryGame, GameData *data) {
        memcpy(data, sharedMemoryGame.data, sizeof(GameData));
    }

};

#endif //QRGAME_GAME_H