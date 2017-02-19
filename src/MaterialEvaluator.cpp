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

    double knightBishopMod = knightBishopWeight * (bitwise::countBits(boardState.pieces[PieceType::Pawn] & centerBoard) - 2);
    score += (knightValue + knightBishopMod) * boardState.pieceCount(PieceType::Knight, WHITE);
    score -= (knightValue + knightBishopMod) * boardState.pieceCount(PieceType::Knight, BLACK);
    score += (bishopValue - knightBishopMod) * boardState.pieceCount(PieceType::Bishop, WHITE);
    score -= (bishopValue - knightBishopMod) * boardState.pieceCount(PieceType::Bishop, BLACK);

    score += pawnValue * boardState.pieceCount(PieceType::Pawn, WHITE);
    score -= pawnValue * boardState.pieceCount(PieceType::Pawn, BLACK);
    score += rookValue * boardState.pieceCount(PieceType::Rook, WHITE);
    score -= rookValue * boardState.pieceCount(PieceType::Rook, BLACK);
    score += queenValue * boardState.pieceCount(PieceType::Queen, WHITE);
    score -= queenValue * boardState.pieceCount(PieceType::Queen, BLACK);

    return score;
}
