#include "GameOverEvaluator.h"
#include "Bitwise.h"
#include "Board.h"

GameOverEvaluator::GameOverEvaluator()
{
}

GameOverEvaluator::~GameOverEvaluator()
{
}

double GameOverEvaluator::evaluate(Board boardState)
{
    // We want negative scores for white turns and positive scores for black turns
    // Because if it's white's turn and there is a checkmate, it's good for black, and vice versa
    int modifier = 1 - (boardState.turn * 2);

    switch(boardState.gameOverCheck())
    {
    case(1):
        //CheckMate
        return 999 * modifier;
        break;
    case(2):
        //StaleMate
        return 1000;
        break;
    default:
        //Neither. Move along.
        break;
    }

    int pieceCounts[7][2];
    for (int i = 1; i < 7; i++)
    {
        pieceCounts[i][0] = boardState.pieceCount(i, false);
        pieceCounts[i][1] = boardState.pieceCount(i, true);
    }

    // Check if there isn't enough material on the board for a mate
    // Only condition is one or fewer minor pieces on each side
    if (!pieceCounts[PieceType::Pawn][WHITE] && !pieceCounts[PieceType::Pawn][BLACK])
    {
        if (!pieceCounts[PieceType::Queen][WHITE] && !pieceCounts[PieceType::Queen][BLACK])
        {
            if (!pieceCounts[PieceType::Rook][WHITE] && !pieceCounts[PieceType::Rook][BLACK])
            {
                if (pieceCounts[PieceType::Knight][WHITE] + pieceCounts[PieceType::Bishop][WHITE] <= 1)
                {
                    if (pieceCounts[PieceType::Knight][BLACK] + pieceCounts[PieceType::Bishop][BLACK] <= 1)
                    {
                        return 1000;
                    }
                }
            }
        }
    }

    return 0;
}
