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

	switch(boardState.gameOverCheck()){
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
	for (int i = 1; i < 7; i++){
		pieceCounts[i][0] = boardState.pieceCount(i, false);
		pieceCounts[i][1] = boardState.pieceCount(i, true);
	}

    // Check if there isn't enough material on the board for a mate
    // Only condition is one or fewer minor pieces on each side
	if (!pieceCounts[PAWN][WHITE] && !pieceCounts[PAWN][BLACK]){
		if (!pieceCounts[QUEEN][WHITE] && !pieceCounts[QUEEN][BLACK]){
			if (!pieceCounts[ROOK][WHITE] && !pieceCounts[ROOK][BLACK]){
				if (pieceCounts[KNIGHT][WHITE] + pieceCounts[BISHOP][WHITE] <= 1){
					if (pieceCounts[KNIGHT][BLACK] + pieceCounts[BISHOP][BLACK] <= 1){
						return 1000;
					}
				}
			}
		}
	}

	return 0;
}
