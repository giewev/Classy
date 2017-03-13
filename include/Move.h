#ifndef MOVE_H
#define MOVE_H

#include <string>
#include "Bitwise.h"
class Danger;
class Board;

struct Move
{
    int startX, startY;
    int endX, endY;
    PieceType promotion;
    double score;
    int gameOverDepth;
    int scoreDepth;

public:

    Move();
    Move(int, int, int, int);
    Move(int, int, int, int, PieceType);

    //void setEnPassent(bool);
    void setScore(double);
    void setGameOverDepth(int);

    double getScore();
    int getGameOverDepth();

    bool operator==(Move);
    bool operator!=(Move);

    bool operator<(Move);
    bool operator>(Move);
    static bool bigger(Move, Move);

    bool isSafe(Danger);
    std::string basicAlg();
    bool isCapture(Board*);
};

#endif
