#include "MaterialEvaluator.h"
#include "Board.h"
#include "Bitwise.h"
#include "Evaluation Constants.h"

MaterialEvaluator::MaterialEvaluator()
{
}

MaterialEvaluator::~MaterialEvaluator()
{
}

double MaterialEvaluator::evaluate(Board boardState)
{
    double score = 0;

	double knightBishopMod = knightBishopWeight * (bitwise::countBits(boardState.pieces[PAWN] & centerBoard) - 2);
	score += (knightValue + knightBishopMod) * boardState.pieceCount(KNIGHT, WHITE);
	score -= (knightValue + knightBishopMod) * boardState.pieceCount(KNIGHT, BLACK);
	score += (bishopValue - knightBishopMod) * boardState.pieceCount(BISHOP, WHITE);
	score -= (bishopValue - knightBishopMod) * boardState.pieceCount(BISHOP, BLACK);

    score += pawnValue * boardState.pieceCount(PAWN, WHITE);
	score -= pawnValue * boardState.pieceCount(PAWN, BLACK);
	score += rookValue * boardState.pieceCount(ROOK, WHITE);
	score -= rookValue * boardState.pieceCount(ROOK, BLACK);
	score += queenValue * boardState.pieceCount(QUEEN, WHITE);
	score -= queenValue * boardState.pieceCount(QUEEN, BLACK);

    return score;
}
