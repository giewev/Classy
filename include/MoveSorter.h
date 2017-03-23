#ifndef MOVESORTER_H
#define MOVESORTER_H

#include "Board.h"
#include "TranspositionCache.h"
struct Move;

class MoveSorter
{
public:
    static const double piecePriorities[];

    MoveSorter(Move* moveList, int moveCount, Board boardState, TranspositionCache transposition);
    void sortMoves();
protected:
private:
    Move* moveList;
    int moveCount;
    Board boardState;
    TranspositionCache transposition;

    bool moveBetter(const Move& left, const Move& right);
    void assignOrderingScores();
};

#endif // MOVESORTER_H
