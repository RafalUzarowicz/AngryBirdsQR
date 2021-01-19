

#ifndef QRGAME_WALL_H
#define QRGAME_WALL_H


class Wall {
private:
    const int width;
    const int gapHeight;
    double height;
    double x = 0;
    bool counted = false;

public:
    Wall(double height = 0.0, int width = 3, int gapHeight = 9) : height(height), width(width), gapHeight(gapHeight){

    }
    double getX() const {
        return x;
    }

    void setX(double x) {
        Wall::x = x;
    }

    int getWidth() const {
        return width;
    }

    int getGapHeight() const {
        return gapHeight;
    }

    double getHeight() const {
        return height;
    }

    void setHeight(double height) {
        Wall::height = height;
    }

    bool isCounted() const {
        return counted;
    }

    void setCounted(bool counted) {
        Wall::counted = counted;
    }
};


#endif //QRGAME_WALL_H
