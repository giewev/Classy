#include "PositionalEvaluator.h"
#include "Evaluation Constants.h"
#include "Board.h"
#include "Bitwise.h"
#include "Engine.h"

PositionalEvaluator::PositionalEvaluator()
{
}

PositionalEvaluator::~PositionalEvaluator()
{
}

double PositionalEvaluator::evaluate(Board boardState)
{
    double score = 0;

    // Count how many pawns are doubled up on a friendly pawn
    bitBoard whitePawns = boardState.pieces[PieceType::Pawn] & boardState.pieces[0];
    bitBoard blackPawns = boardState.pieces[PieceType::Pawn] & ~boardState.pieces[0];
    score += doubledPawnValue * bitwise::countBits(whitePawns & (whitePawns << 1));
    score -= doubledPawnValue * bitwise::countBits(blackPawns & (blackPawns >> 1));

    // Count how many pawns from each player are in the center
    bitBoard centerPawns = boardState.pieces[PieceType::Pawn] & centerBoard;
    score += centerPawnValue * bitwise::countBits(centerPawns & boardState.pieces[0]);
    score -= centerPawnValue * bitwise::countBits(centerPawns & (~boardState.pieces[0]));
    score += semiCenterPawnValue * bitwise::countBits(whiteSemiCenter & boardState.pieces[0] & boardState.pieces[PieceType::Pawn]);
    score += semiCenterPawnValue * bitwise::countBits(blackSemiCenter & (~boardState.pieces[0]) & boardState.pieces[PieceType::Pawn]);

    // Knights on the rim are grim
    bitBoard sideKnights = boardState.pieces[PieceType::Knight] & (firstFile | (firstFile << 56));
    score += rimKnightValue * bitwise::countBits(sideKnights & boardState.pieces[0]);
    score -= rimKnightValue * bitwise::countBits(sideKnights & (~boardState.pieces[0]));

    //Bishop and Knight development
    bitBoard knightsAndBishops = (boardState.pieces[PieceType::Bishop] | boardState.pieces[PieceType::Knight]);
    bitBoard whiteUndeveloped = knightsAndBishops & firstRank & boardState.pieces[0];
    bitBoard blackUndeveloped = knightsAndBishops & (firstRank << 7) & (~boardState.pieces[0]);
    score -= developmentValue * bitwise::countBits(whiteUndeveloped);
    score += developmentValue * bitwise::countBits(blackUndeveloped);

    //Kings far from center during the main game, but near the center when there are few pieces left
    double kingLocationMultiplier = 1;
    if (boardState.pieceCount() < 12)
    {
        kingLocationMultiplier *= -1;
    }

    score += kingCornerValue * kingLocationMultiplier * manhattenDistance(boardState.getKingX(true), boardState.getKingY(true), 4.5, 4.5);
    score -= kingCornerValue * kingLocationMultiplier * manhattenDistance(boardState.getKingX(false), boardState.getKingY(false), 4.5, 4.5);

    return score;
}

double PositionalEvaluator::manhattenDistance(const int x1, const int y1, const double x2, const double y2)
{
    return fabs(x1 - x2) + fabs(y1 - y2);
}
