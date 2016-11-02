
#include "Piece.h"
#include "Move.h"
#include <iostream>
#include <string>
#include "Board.h"



const int EMPTY  = 0;
const int PAWN   = 1;
const int QUEEN  = 2;
const int KING   = 3;
const int BISHOP = 4;
const int KNIGHT = 5;
const int ROOK   = 6;

Piece::Piece(){
	xPos = -1;
	yPos = -1;
	color = false;
	type = 0;
	setMoved(false);
}
Piece::Piece(bool newColor){
	xPos = -1;
	yPos = -1;
	color = newColor;
	type = 0;
	setMoved(false);
}
Piece::Piece(int newX, int newY, bool newColor){
	xPos = newX;
	yPos = newY;
	color = newColor;
	type = 0;
	setMoved(false);
}
Piece::Piece(int name, int newX, int newY, bool newColor){
	xPos = newX;
	yPos = newY;
	color = newColor;
	type = name;
	setMoved(false);
}
Piece::Piece(int throwaway){
	xPos = -1;
	yPos = -1;
	color = true;
	type = 0;
	setMoved(false);
}
Piece::~Piece(){
}

bool Piece::operator!=(Piece other){
	if(type != other.type){
		return true;
	}
	if(xPos != other.xPos){
		return true;
	}
	if(yPos != other.yPos){
		return true;
	}
	if(color != other.color){
		return true;
	}
	return false;
}

const bool Piece::isNull(){
	return (type == EMPTY);
}


bool Piece::hasMoved(){
	return(moved);
}
void Piece::setMoved(bool x){
	moved = x;
	return;
}

void Piece::generateMoves(std::vector<Move>& moveList, int x, int y, Board& gameBoard){
    Move moveArray[220];
    int moveCounter = 0;
    appendMoveArray(moveArray, moveCounter, x, y, gameBoard);
    for (int i = 0; i < moveCounter; i++)
    {
        moveList.push_back(moveArray[i]);
    }
}

void Piece::appendMoveArray(Move* moveList, int& moveCounter, int x, int y, Board& gameBoard){
    int type = gameBoard.getSquareType(x, y);
	if(type == EMPTY){
	}
	else if(type == PAWN){
		pawnMoveArray(moveList, moveCounter, x, y, gameBoard);
	}
	else if(type == KING){
		kingMoveArray(moveList, moveCounter, x, y, gameBoard);
	}
	else if(type == QUEEN){
		queenMoveArray(moveList, moveCounter, x, y, gameBoard);
	}
	else if(type == BISHOP){
		bishopMoveArray(moveList, moveCounter, x, y, gameBoard);
	}
	else if(type == KNIGHT){
		knightMoveArray(moveList, moveCounter, x, y, gameBoard);
	}
	else if(type == ROOK){
		rookMoveArray(moveList, moveCounter, x, y, gameBoard);
	}
	else{
		std::cout << "No matching piece in appendMoveArray" << std::endl;
	}
	return;
}

void Piece::kingMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, Board& gameBoard){
	bool ownColor = gameBoard.getSquareColor(xPos, yPos);
	for (int j=-1; j<=1; j++){
		for (int k=-1; k<=1; k++){
			if(j==0 && k==0){//if not moving
				continue;
			}

			if(xPos+j > 8 || xPos+j<1){//if moving horizontally off the board
				continue;
			}
			if(yPos+k > 8 || yPos+k<1){//if moving vertically off the board
				continue;
			}

			//Check to see if Friendly Piece is there
			if(gameBoard.getSquareType(xPos + j, yPos + k) != EMPTY){
				if(gameBoard.getSquareColor(xPos + j, yPos + k) == ownColor){
					continue;
				}
			}
			//Everything is good, so make a move and add it to the moveList
			moveList[moveCounter++] = Move(xPos, yPos, xPos+j, yPos+k, 0);
		}
	}

		//CastlingMoves
	for(int i=0; i<=1; i++){
		if(gameBoard.getCastlingRights(ownColor, i==1)){
			if(i){
				if(gameBoard.getSquareType(xPos+1, yPos) != EMPTY){
					continue;
				}
				if(gameBoard.getSquareType(xPos+2, yPos) != EMPTY){
					continue;
				}

				moveList[moveCounter++] = Move(xPos, yPos, xPos + 2, yPos);
			}
			else{
				if(gameBoard.getSquareType(xPos-1, yPos) != EMPTY){
					continue;
				}
				if(gameBoard.getSquareType(xPos-2, yPos) != EMPTY){
					continue;
				}

				if(gameBoard.getSquareType(xPos-3, yPos) != EMPTY){
					continue;
				}
				moveList[moveCounter++] = Move(xPos, yPos, xPos - 2, yPos);
			}
		}
	}
}

void Piece::queenMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, Board& gameBoard){
    rookMoveArray(moveList, moveCounter, xPos, yPos, gameBoard);
    bishopMoveArray(moveList, moveCounter, xPos, yPos, gameBoard);
}

void Piece::bishopMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, Board& gameBoard){
	bool ownColor = gameBoard.getSquareColor(xPos, yPos);
	for(int i=-1; i<=1; i+=2){
		for(int j=-1; j<=1; j+=2){
			if(i==0 || j==0){
				continue;
			}
			for(int k=1; k<=7; k++){

				if(xPos+(k*i)<1 || xPos+(k*i)>8 || yPos+(k*j)<1 || yPos+(k*j)>8){//Too Far
					break;
				}

				if(gameBoard.getSquareType(xPos+(k*i), yPos+(k*j)) != EMPTY){
					if(gameBoard.getSquareColor(xPos+(k*i), yPos+(k*j)) == ownColor){
						break;
					}
					else{
						moveList[moveCounter++] = Move(xPos, yPos, xPos+(k*i), yPos+(k*j));
						break;
					}
				}

				moveList[moveCounter++] = Move(xPos, yPos, xPos+(k*i), yPos+(k*j));

			}
		}
	}
}
void Piece::knightMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, Board& gameBoard){
	bool ownColor = gameBoard.getSquareColor(xPos, yPos);
	for(int j=-2; j<=2; j++){
		for(int k=-2; k<=2; k++){
			if(j==0 || k==0){//Polar movement
				continue;
			}
			if(j==k || j==-1*k){//square movement
				continue;
			}
			if(xPos+j>8 || xPos+j<1 || yPos+k>8 || yPos+k<1){//trying to move outside board
				continue;
			}

			if(gameBoard.getSquareType(xPos + j, yPos + k) != EMPTY){
				if(gameBoard.getSquareColor(xPos + j, yPos + k) == ownColor){
					continue;
				}
			}

			moveList[moveCounter++] = Move(xPos, yPos, xPos+j, yPos+k, 0);
		}
	}
}

void Piece::rookMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, Board& gameBoard){
	bool ownColor = gameBoard.getSquareColor(xPos, yPos);
	for(int i=-1; i<=1; i++){
		for(int j=-1; j<=1; j++){
			if(!i == !j){// diagonal or no motion
				continue;
			}
			for(int k=1; k<8; k++){

				if(xPos+(i*k) < 1 || xPos+(i*k) > 8){//out of bounds X
					continue;
				}
				if(yPos+(j*k) < 1 || yPos+(j*k) > 8){//out of Bounds Y
					continue;
				}

				if(gameBoard.getSquareType(xPos+(i*k), yPos+(j*k)) != EMPTY){
					if(gameBoard.getSquareColor(xPos+(i*k), yPos+(j*k)) == ownColor){
						break;
					}
					else{
						moveList[moveCounter++] = Move(xPos, yPos, xPos+(k*i), yPos+(k*j));
						break;
					}
				}

				moveList[moveCounter++] = Move(xPos, yPos, xPos+(k*i), yPos+(k*j));
			}
		}
	}
}

void Piece::pawnMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, Board& gameBoard){
    bool ownColor = gameBoard.getSquareColor(xPos, yPos);
	int direction = -1;
	if(ownColor){
		direction = 1;
	}

	if(gameBoard.getSquareType(xPos, yPos + direction) == EMPTY){//Normal Moves
		if(yPos+direction == 8 || yPos+direction == 1){
			for(int i=1; i<=4; i++){
				moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+1, i);
			}
		}
		else{
			moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+direction);
		}

		if((yPos == 2 && ownColor) || (yPos == 7 && !ownColor)){//Double Moves
			if(gameBoard.getSquareType(xPos, yPos + 2 * direction) == EMPTY){
				moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+2*direction);
			}
		}
	}

	//Capture Moves
	for(int i = -1; i<=1; i+=2){
		if(xPos+i > 8 || xPos+i < 1){
			continue;
		}

		if(gameBoard.getSquareType(xPos + i, yPos + direction) == EMPTY){
			if(yPos == gameBoard.getEP().getY()){
				if(xPos + i == gameBoard.getEP().getX()){
					if(gameBoard.getEP().getColor() != ownColor){
						moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction);
					}
				}
			}
			continue;
		}

		if(gameBoard.getSquareColor(xPos + i, yPos + direction) == ownColor){
			continue;
		}

		if(yPos+direction == 8 || yPos+direction == 1){
			for(int j=1; j<=4; j++){
				moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction, j);
			}
		}
		else{
			moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction);
		}
	}
}

//Returns false if the Piece is in check, true otherwise
bool Piece::isSafe(Board* gameBoard){
		//Check if next to a king
	for(int x=-1; x<=1; x++){
		for(int y=-1; y<=1; y++){

			if(!x && !y){//No motion
				continue;
			}
			if(xPos+x > 8 || xPos+x<1){//if checking horizontally off the board
				continue;
			}
			if(yPos+y > 8 || yPos+y<1){//if checking vertically off the board
				continue;
			}

			if(gameBoard->getSquareType(xPos+x, yPos+y) == KING){
                if (gameBoard->getSquareColor(xPos+x, yPos+y) != color)
                {
                    return(false);
				}
			}
		}
	}

		//Check if Pawn ahead
	int direction = -1;
	if(color) direction = 1;

	for(int x=-1; x<=1; x+=2){
		if(xPos + x > 8 || xPos + x <1){
			continue;
		}

		if(gameBoard->getSquareType(xPos+x, yPos+direction) == PAWN){
			if(gameBoard->getSquareColor(xPos+x, yPos+direction) != color){
				return(false);
			}
		}
	}
		//Check for Rook or Queen
	for(int i=-1; i<=1; i++){
		for(int j=-1; j<=1; j++){
			if(!i == !j){
				continue;
			}

			for(int k=1; k<=7; k++){
					//Out of bounds
				if((xPos+(i*k)>8) || (xPos+(i*k)<1) || (yPos+(j*k)>8) || (yPos+(j*k)<1)){
					break;
				}

					//Found a piece
                int targetType = gameBoard->getSquareType(xPos+(i*k), yPos+(j*k));
                int targetColor = gameBoard->getSquareColor(xPos+(i*k), yPos+(j*k));
				if(targetType != EMPTY){
					if(targetColor != color){
						if(targetType == ROOK || targetType == QUEEN){
							return(false);
						}
						break;
					}
					break;
				}
			}
		}
	}

		//Check for Bishop or Queen
	for(int i=-1; i<=1; i++){
		for(int j=-1; j<=1; j++){
			if(i*j == 0){//Linear or no Motion
				continue;
			}

			for(int k=1; k<=7; k++){
					//Out of bounds
				if(xPos+(i*k)>8 || xPos+(i*k)<1 || yPos+(j*k)>8 || yPos+(j*k)<1){
					break;
				}

					//Found a piece
                int targetType = gameBoard->getSquareType(xPos+(i*k), yPos+(j*k));
                bool targetColor = gameBoard->getSquareColor(xPos+(i*k), yPos+(j*k));
				if(targetType != EMPTY){
					if(targetColor != color){
						if(targetType == BISHOP || targetType == QUEEN){
							return(false);
						}
						break;
					}
					break;
				}
			}
		}
	}

		//Check for Knight
	for(int i=-2; i<=2; i++){
		for(int j=-2; j<=2; j++){
			if(i == 0 || j == 0){//Linear Motion
				continue;
			}
			if(i == j || i==-j){//Diagonal Motion
				continue;
			}

			if(xPos+i<1 || xPos+i>8 || yPos+j<1 || yPos+j>8){//out of bounds
				continue;
			}

			int targetType = gameBoard->getSquareType(xPos+i, yPos+j);
			bool targetColor =gameBoard->getSquareColor(xPos+i, yPos+j);
			if(targetType != EMPTY){
				if(targetColor != color){
					if(targetType == KNIGHT){
						return false;
					}
				}
			}
		}
	}

	return true;
}

