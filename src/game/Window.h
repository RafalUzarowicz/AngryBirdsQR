#ifndef QRGAME_WINDOW_H
#define QRGAME_WINDOW_H

#include <ncurses.h>
#include <iostream>
#include "Player.h"
#include "Logic.h"

class Window {
private:
    void drawUI(){
        Window::drawScoreMessage(game->getScore());
        this->drawBorder();
    }

    void drawBorder() const{
        for(int i = 0; i < width; ++i){
            mvaddch(0,i,VERTICAL_BORDER);
            mvaddch(height - 1, i, VERTICAL_BORDER);
        }
        for(int i = 0; i < height; ++i){
            mvaddch(i,0,HORIZONTAL_BORDER);
            mvaddch(i, width - 1, HORIZONTAL_BORDER);
        }
        mvaddch(0, 0, CORNER);
        mvaddch(0, width - 1, CORNER);
        mvaddch(height - 1, 0, CORNER);
        mvaddch(height - 1, width - 1, CORNER);
    }

    static void drawScoreMessage(int score){
        mvprintw(2, 3, "Score: %d", score);
    }

    void drawPlayer(){
        for( int i = 0; i < game->getPlayer().getSprite().sprite.size(); ++i ){
            mvprintw((game->getPlayer().getY()+i) % height, game->getPlayer().getX() % width, game->getPlayer().getSprite().sprite[i].c_str());
        }
    }

    void drawWalls(){
        for( auto wall : game->getWalls() ){
            if(wall.getX() < width){
                for( int i = 0; i < wall.getWidth(); ++i){
                    int gap = height*wall.getHeight();
                    mvvline(0, wall.getX()+i, WALL, gap);
                    mvvline(gap + wall.getGapHeight() , wall.getX()+i, WALL, height - gap - wall.getGapHeight());
                }
            }
        }
    }

    const static char VERTICAL_BORDER = '-';
    const static char HORIZONTAL_BORDER = '|';
    const static char CORNER = '+';
    const static char WALL = '#';

    const static int MIN_HEIGHT = 20;
    const static int MIN_WIDTH = 30;

    int height{}, width{};

    Logic* game;
public:
    explicit Window(Logic* game) : game(game){
        //resize();
    }

    void initialize(){
        this->resize();
        Window::clear();
    }

    void resize(){
        int h, w;
        getmaxyx( stdscr, h, w );
        if( h < MIN_HEIGHT || w < MIN_WIDTH ){
            endwin();
            throw std::invalid_argument( "Window is too small." );
        }else if( h != this->height || w != this->width ){
           this->height = h;
           this->width = w;
           this->game->initialize(h, w);
        }
    }

    void draw(){
        this->drawWalls();
        this->drawPlayer();
        this->drawUI();
        refresh();
    }

    static void clear(){
        ::erase();
    }

    int getHeight() const{
        return height;
    }

    int getWidth() const{
        return width;
    }
};


#endif //QRGAME_WINDOW_H
