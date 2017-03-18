#include <algorithm>
#include "MaterialEvaluator.h"
#include "MoveSorter.h"

MoveSorter::MoveSorter(Move* moveList, int moveCount, Board boardState, TranspositionCache transposition)
{
    this->moveList = moveList;
    this->moveCount = moveCount;
    this->boardState = boardState;
    this->transposition = transposition;
}

void MoveSorter::sortMoves()
{
    std::sort(this->moveList, this->moveList + this->moveCount,
        [this] (const Move& left, const Move& right) {
            return this->moveBetter(left, right);
        });
}

bool MoveSorter::moveBetter(const Move& left, const Move& right)
{
    if (left == transposition.bestMove)
    {
        return true;
    }
    else if (left == transposition.cutoffMove && right != transposition.bestMove)
    {
        return true;
    }
    else if (right == transposition.bestMove || right == transposition.cutoffMove)
    {
        return false;
    }

    PieceType leftVictimType = boardState.getSquareType(left.endX, left.endY);
    PieceType rightVictimType = boardState.getSquareType(right.endX, right.endY);
    double leftVictimValue = MaterialEvaluator::pieceValue(leftVictimType);
    double rightVictimValue = MaterialEvaluator::pieceValue(rightVictimType);
    if (leftVictimValue != rightVictimValue)
    {
        return leftVictimValue > rightVictimValue;
    }
    else if (leftVictimType != PieceType::Empty)
    {
        PieceType leftAttackerType = boardState.getSquareType(left.startX, left.startY);
        PieceType rightAttackerType = boardState.getSquareType(right.startX, right.startY);
        double leftAttackerValue = MaterialEvaluator::pieceValue(leftAttackerType);
        double rightAttackerValue = MaterialEvaluator::pieceValue(rightAttackerType);
        if (leftAttackerValue != rightAttackerValue)
        {
            return leftAttackerValue < rightAttackerValue;
        }
    }

    return false;
}
