#include "Move.h"
#include "Danger.h"
#include "Board.h"
#include "Piece.h"
#include "Engine.h"
#include <iostream>
#include <sstream>
#include <math.h>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

const int EMPTY = 0;
const int PAWN = 1;
const int QUEEN = 2;
const int KING = 3;
const int BISHOP = 4;
const int KNIGHT = 5;
const int ROOK = 6;

Move::Move(int x1, int y1, int x2, int y2){
	startX = x1;
	startY = y1;
	endX = x2;
	endY = y2;
	promotion = 0;
	score = 0;
	gameOverDepth = -1;
	scoreDepth = -1;
}

Move::Move(int x1, int y1, int x2, int y2, int promote){
	startX = x1;
	startY = y1;
	endX = x2;
	endY = y2;
	promotion = promote;
	score = 0;
	gameOverDepth = -1;
	scoreDepth = -1;
}

Move::Move(){
	startX = -1;
	startY = -1;
	endX = -1;
	endY = -1;
	promotion = 0;
	score = 0;
	gameOverDepth = -1;
	scoreDepth = -1;
}

void Move::setScore(double scoreVal){
	score = scoreVal;
}

void Move::setGameOverDepth(int depthVal){
	gameOverDepth = depthVal;
}

double Move::getScore(){
	return score;
}

int Move::getGameOverDepth(){
	return gameOverDepth;
}

bool Move::isSafe(Danger safetyData){
	if(safetyData.movePinned(*this)){
		return false;
	}
	if (startY == 4 || startY == 5){
		if (fabs(startX - endX) == 1 && fabs(startY - endY) == 1){
			if (safetyData.getBoard()->squareIsType(endX, endY, EMPTY)){
				if (safetyData.getBoard()->squareIsType(endX, startY, PAWN)){
					Board* newBoard = safetyData.getBoard()->newCopy();
					newBoard->makeMove(*this);
					if (newBoard->getSquare(newBoard->getKingX(!newBoard->turn), newBoard->getKingY(!newBoard->turn)).isSafe(newBoard)){
						delete newBoard;
						return true;
					}
					else{
						delete newBoard;
						return false;
					}
				}
			}
		}
	}
	if(endX == 7 && endY == 2){
		//std::cout << "caught" << std::endl;
	}
	//std::cout << "caught" << std::endl;

	//The king is moving, need a manual check
	if(startX == safetyData.defenderX && startY == safetyData.defenderY){
		Board* newBoard;
			//Castling Moves
		if(fabs(startX - endX) == 2){
				//Cant castle out of check
			if(safetyData.getCheck()){
				return false;
			}
				//iterates over the squares next to the king for two squares
				//The absolute value division thing finds the sign of the direction
			int direction = -1*fabs(startX-endX)/(startX-endX);
			for (int i = startX + direction; i <= startX + 2 && i >= startX - 2; i+=direction){
				newBoard = new Board();
				newBoard = safetyData.getBoard()->newCopy();
				newBoard->makeMove(Move(startX, startY, i, startY));
				if(!newBoard->getSquare(i, endY).isSafe(newBoard)){
					delete newBoard;
					return false;
				}
				delete newBoard;
			}
			return true;
		}
			//Normal King moves
		newBoard = safetyData.getBoard()->newCopy();
		newBoard->makeMove(*this);
		if(newBoard->getSquare(endX, endY).isSafe(newBoard)){
			delete newBoard;
			return true;
		}
		else{
			delete newBoard;
			return false;
		}
	}
	if(safetyData.getDoubleCheck()){
			//Only the King can move, and we checked if the king was moving earlier
		return false;
	}
	else if(safetyData.getCheck()){
		if(endX == 7 && endY == 2){
			//std::cout << "caught" << std::endl;
		}
		if(safetyData.inSafeSquares(endX, endY)){
			return true;
		}
		else{
			if(endX == 4 && endY == 2){
				//std::cout << "info string " << Engine::toAlg(startX) << startY << std::endl;
			}
			return false;
		}
	}
	return true;
}

std::string Move::basicAlg(){
	std::string move = "";
	move = Engine::toAlg(startX) + patch::to_string(startY) + "-"+ Engine::toAlg(endX) + patch::to_string(endY);
	return move;
}

bool Move::operator==(Move other){
	if(startX != other.startX){
		return false;
	}
	if(startY != other.startY){
		return false;
	}
	if(endX != other.endX){
		return false;
	}
	if(endY != other.endY){
		return false;
	}
	if(promotion != other.promotion){
		return false;
	}
	return true;
}

bool Move::operator!=(Move other){
	if(startX != other.startX){
		return true;
	}
	if(startY != other.startY){
		return true;
	}
	if(endX != other.endX){
		return true;
	}
	if(endY != other.endY){
		return true;
	}
	if(promotion != other.promotion){
		return true;
	}
	return false;
}

bool Move::operator<(Move other){
	if (score < other.score){
		return true;
	}
	else{
		return false;
	}
}

bool Move::operator>(Move other){
	if (score > other.score){
		return true;
	}
	else{
		return false;
	}
}

bool Move::bigger(Move left, Move right){
	if (left > right){
		return true;
	}
	else{
		return false;
	}
}

bool Move::isCapture(Board* safeBoard){
	int type = safeBoard->getSquareType(endX, endY);
	return (type != 0);
}
