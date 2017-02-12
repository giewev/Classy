#include <vector>
#include <iostream>
#include <inttypes.h>

#include "Piece.h"
#include "Board.h"
#include "Move.h"
#include "Danger.h"
#include "Engine.h"
#include "Bitwise.h"
#include <math.h>

using namespace std;

#define bitBoard uint64_t

Board::Board(){
	turn = true;
	lastMoveCapture = false;
	castlingRights = 15;
	for(int i=0; i<7; i++){
		pieces[i] = 0ull;
	}
	movedBoard = 0;
	allPieces = 0;
	EPdata = -1;
}
Board::Board(int null){
}
Board::~Board(){
}

void Board::loadFEN(std::string fenFile){
	unsigned int bookmark = 0;

		//Blanking out the Board
	for(int i=0; i<7; i++){
		pieces[i] = 0ull;
	}

	int x = 1;
	int y = 8;

	for(unsigned int i=0; i<fenFile.length(); i++){

			//checking for every value that represents a piece
		if     (fenFile[i] == 'r'){
			setSquare(Piece(ROOK, x, y, false), x, y);
		}
		else if(fenFile[i] == 'R'){
			setSquare(Piece(ROOK, x, y, true), x, y);
		}
		else if(fenFile[i] == 'n'){
			setSquare(Piece(KNIGHT, x, y, false), x, y);
		}
		else if(fenFile[i] == 'N'){
			setSquare(Piece(KNIGHT, x, y, true), x, y);
		}
		else if(fenFile[i] == 'b'){
			setSquare(Piece(BISHOP, x, y, false), x, y);
		}
		else if(fenFile[i] == 'B'){
			setSquare(Piece(BISHOP, x, y, true), x, y);
		}
		else if(fenFile[i] == 'q'){
			setSquare(Piece(QUEEN, x, y, false), x, y);
		}
		else if(fenFile[i] == 'Q'){
			setSquare(Piece(QUEEN, x, y, true), x, y);
		}
		else if(fenFile[i] == 'k'){
			setKingLocation(false, x, y);
			setSquare(Piece(KING, x, y, false), x, y);
		}
		else if(fenFile[i] == 'K'){
			setKingLocation(true, x, y);
			setSquare(Piece(KING, x, y, true), x, y);
		}
		else if(fenFile[i] == 'p'){
			setSquare(Piece(PAWN, x, y, false), x, y);
		}
		else if(fenFile[i] == 'P'){
			setSquare(Piece(PAWN, x, y, true), x, y);
		}

			//Checking if the value was a number
		if(fenFile[i] != '/'){
			if(fenFile[i] >= 49 && fenFile[i] <= 56){
					//If it is jump ahead some squares
				x += fenFile[i] - 48;
			}
			else{
					//If it isnt we just go to the next one
				x += 1;
			}
		}

			//next Rank
		if(x>8){
			x=1;
			y-=1;
		}

			//Done setting pieces up
		if(y==0){
			bookmark = i+1;
			break;
		}
	}

	//setting turn
	for(; bookmark < fenFile.length(); bookmark++){
		if(fenFile[bookmark] == 'w'){
			turn = true;
			bookmark++;
			break;
		}
		else if(fenFile[bookmark] == 'b'){
			turn = false;
			bookmark++;
			break;
		}
		else{
			continue;
		}
	}

	//Setting Castling Rights
	setCastlingRights(0);
	bool breakOut = false;
	bool startedCastling = false;
	while (bookmark < fenFile.length()){
		switch (fenFile[bookmark]){
		case('K') :
			setCastlingRights(true, true, true);
			startedCastling = true;
			breakOut = false;
			break;
		case('Q') :
			setCastlingRights(true, false, true);
			startedCastling = true;
			breakOut = false;
			break;
		case('k') :
			setCastlingRights(false, true, true);
			startedCastling = true;
			breakOut = false;
			break;
		case('q') :
			setCastlingRights(false, false, true);
			startedCastling = true;
			breakOut = false;
			break;
		case('-') :
			breakOut = true;
			startedCastling = true;
			break;
		default:
			breakOut = true;
		}
		bookmark++;
		if (breakOut && startedCastling){
			break;
		}
	}

	//Setting EnPassent
	while (bookmark < fenFile.length()){
		if (fenFile[bookmark] == ' '){
			bookmark++;
			continue;
		}
		if (fenFile[bookmark] == '-'){
			bookmark++;
			break;
		}
		else if (bookmark + 1 < fenFile.length()){
			int rank;
			if (fenFile[bookmark + 1] == '3'){
				rank = 4;
			}
			else{
				rank = 5;
			}
			setEP(fenFile[bookmark]-96, rank, !turn);
			break;
		}
		bookmark += 2;
	}

	//setting halfmove clock
	while (bookmark < fenFile.length()){
		if (fenFile[bookmark] >= 48 && fenFile[bookmark] <= 57){
			if (bookmark + 1 < fenFile.length() &&
				fenFile[bookmark + 1] >= 48 && fenFile[bookmark + 1] <= 57){
				halfMoveCounter = 10 * (fenFile[bookmark] - 48) + fenFile[bookmark + 1] - 48;
				bookmark += 2;
				break;
			}
			else{
				halfMoveCounter = fenFile[bookmark] - 48;
				bookmark++;
				break;
			}
		}
		else{
			bookmark++;
		}
	}

	//setting fullMove clock
	while (bookmark < fenFile.length()){
		if (fenFile[bookmark] >= 48 && fenFile[bookmark] <= 57){
			if (bookmark + 1 < fenFile.length() &&
				fenFile[bookmark + 1] >= 48 && fenFile[bookmark + 1] <= 57){
				moveCounter = 10 * (fenFile[bookmark] - 48) + fenFile[bookmark + 1] - 48;
				bookmark += 2;
				break;
			}
			else{
				moveCounter = fenFile[bookmark] - 48;
				bookmark++;
				break;
			}
		}
		else{
			bookmark++;
		}
	}
	return;
}

string Board::outputFEN(){
	string FEN = "";
	int count = 0;
	for(int y=8; y>=1; y--){
		for(int x=1; x<=8; x++){
			int pieceType = getSquareType(x, y);
			bool pieceColor = getSquareColor(x, y);
			if(pieceColor){
				if(pieceType == EMPTY){
					count += 1;
				}
				else{
					if (count)
					{
						FEN += char(48 + count);
						count = 0;
					}

					if (pieceType == PAWN)
					{
                        FEN += "P";
					}
					else if (pieceType == ROOK)
					{
                        FEN += "R";
					}
					else if (pieceType == KNIGHT)
					{
                        FEN += "N";
					}
					else if (pieceType == BISHOP)
					{
                        FEN += "B";
					}
					else if (pieceType == QUEEN)
					{
                        FEN += "Q";
					}
					else if (pieceType == KING)
					{
                        FEN += "K";
					}
                    else
                    {
                        count = 1;
                    }
				}
			}
			else{
				if(pieceType == EMPTY){
					count += 1;
				}
				else{
					if (count)
					{
						FEN += char(48 + count);
						count = 0;
					}

					if (pieceType == PAWN)
					{
                        FEN += "p";
					}
					else if (pieceType == ROOK)
					{
                        FEN += "r";
					}
					else if (pieceType == KNIGHT)
					{
                        FEN += "n";
					}
					else if (pieceType == BISHOP)
					{
                        FEN += "b";
					}
					else if (pieceType == QUEEN)
					{
                        FEN += "q";
					}
					else if (pieceType == KING)
					{
                        FEN += "k";
					}
                    else
                    {
                        count = 1;
                    }
				}
			}
		}
		if(count){
			FEN += char(48 + count);
			count = 0;
		}
		FEN += "/";
	}
	if(turn){
		FEN += " w";
	}
	else{
		FEN += " b";
	}
	return FEN;
}

//Creates a new copy of the current Board and all of its contents
Board Board::newCopy(){
	Board newBoard = Board(0);
	for(int i=0; i<7; i++){
		newBoard.pieces[i] = pieces[i];
	}
	newBoard.movedBoard = movedBoard;
	newBoard.allPieces = allPieces;
	newBoard.EPdata = EPdata;

	newBoard.setCastlingRights(true,  true,  getCastlingRights(true, true));
	newBoard.setCastlingRights(true,  false, getCastlingRights(true, false));
	newBoard.setCastlingRights(false, true,  getCastlingRights(false, true));
	newBoard.setCastlingRights(false, false, getCastlingRights(false, false));
	newBoard.turn = turn;
	newBoard.lastMoveCapture = lastMoveCapture;
	newBoard.kingCoordinates = kingCoordinates;
	newBoard.moveCounter = moveCounter;
	newBoard.halfMoveCounter = halfMoveCounter;

	return(newBoard);
}

Piece Board::getSquare(int x, int y){
	x--;
	y--;
	int type = 0;
	bool color = (pieces[0] >> (8 * x + y)) & 1;
	//Empty SquareCheck
	if (false && !(((pieces[PAWN] | pieces[BISHOP] | pieces[KNIGHT] |
		pieces[QUEEN] | pieces[ROOK] | pieces[KING]) >> (8 * x + y)) & 1)) {
		return Piece(EMPTY, x + 1, y + 1, color);
	}
	for(int i=1; i<7; i++){
		if(((pieces[i] >> (8*x + y)) & 1)){
			type = i;
			break;
		}
	}
	return Piece(type, x + 1, y + 1, color);
}
int Board::getSquareType(int x, int y){
    if (!squareIsPopulated(x, y))
    {
        return EMPTY;
    }

	x--;
	y--;
	for(int i=1; i<7; i++){
		if((pieces[i] >> (8*x + y)) & 1){
			return i;
		}
	}

	return EMPTY;
}
bool Board::getSquareColor(int x, int y){
	x--;
	y--;
	return (pieces[0] >> (8*x + y)) & 1;
}

bool Board::getSquareMoved(int x, int y)
{
    x--;
	y--;
	return (movedBoard >> (8*x + y)) & 1;
}

int Board::getKingX(bool getColor){
	if(getColor){
		return((kingCoordinates & 7) +1);
	}
	else{
		return(((kingCoordinates >> 6) & 7) +1);
	}
}
int Board::getKingY(bool getColor){
	if(getColor){
		return(((kingCoordinates >> 3) & 7)+1);
	}
	else{
		return(((kingCoordinates >> 9) & 7)+1);
	}
}

void Board::setSquare(Piece setPiece, int x, int y){
	x--;
	y--;
	if(setPiece.moved){
		movedBoard |= 1ull << (8*x + y);
	}
	else{
		movedBoard &= ~(1ull << (8*x + y));
	}
	setSquare(setPiece.type, setPiece.color, x+1, y+1);
}

void Board::setSquare(int type, bool color, int x, int y){
	x--;
	y--;
	for(int i=1; i<7; i++){
		pieces[i] &= ~(1ull << (8*x + y));
	}
	if(type > 0){
		pieces[type] |= (1ull << (8*x + y));
		allPieces |= (1ull << (8*x + y));
	}
	else{
        allPieces &= ~(1ull << (8*x + y));
	}

	if(color){
		pieces[0] |= (1ull << (8*x + y));
	}
	else{
		pieces[0] &= ~(1ull << (8*x + y));
	}
}

void Board::setKingLocation(bool setColor, int x, int y){
	if(x>8 || x<1){
		std::cout << "info string King X is being set as " << x << std::endl;
	}
	if(y>8 || y<1){
		std::cout << "info string King Y is being set as " << y << std::endl;
	}
	x-=1;
	y-=1;
	int placeHolder;
	if(setColor){
		placeHolder = kingCoordinates >> 6;
		kingCoordinates = (placeHolder<<6) | x | (y<<3);
	}
	else{
		placeHolder = kingCoordinates & ((1<<6)-1);
		kingCoordinates = placeHolder | (x<<6) | (y<<9);
	}
}

std::vector<Move> Board::generateMoves(){
    std::vector<Move> moveList;
    Move rawMoveList[220];
    int moveCounter = 0;
    generateMoveArray(rawMoveList, moveCounter);

    for (int i = 0; i < moveCounter; i++)
    {
        moveList.push_back(rawMoveList[i]);
    }

    return moveList;
}

void Board::generateMoveArray(Move* finalMoveList, int& moveCounter){
	for(int y=1; y<=8; y++){
		for(int x=1; x<=8; x++){
			if(getSquareColor(x, y) == turn){
				Piece::appendMoveArray(finalMoveList, moveCounter, x, y, *this);
			}
		}
	}

	Danger safetyData = Danger(this);
	for(int i = moveCounter - 1; i >= 0; i--){
		if(!finalMoveList[i].isSafe(safetyData)){
            finalMoveList[i] = finalMoveList[moveCounter - 1];
            moveCounter--;
		}
	}
}

int Board::gameOverCheck(){
	bitBoard rightColor;
	if (turn){
		rightColor = pieces[0];
	}
	else{
		rightColor = ~pieces[0];
	}

	int moveCounter = 0;
	Move rawMoveList[27];
	Danger safetyData = Danger(this);
	for(int y=1; y<=8; y++){
		for(int x=1; x<=8; x++){
				//What Piece is there
			if(getSquareType(x, y) == 0){
				continue;
			}
				//If the piece is the right color, add its moves to the list
			//if(getSquareColor(x, y) == turn){
			if ((rightColor >> (x * 8 + y - 9)) & 1){
				Piece target = getSquare(x, y);
				moveCounter = 0;
				target.appendMoveArray(rawMoveList, moveCounter, x, y, *this);
				for(int i=0; i<moveCounter; i++){
					if(rawMoveList[i].isSafe(safetyData)){
						return 0;
					}
				}
			}
		}
	}
	if(safetyData.getCheck()){
		//CheckMate
		return 1;
	}
	else{
		//StaleMate
		return 2;
	}
	return 0;
}

void Board::makeMove(Move data){
		//Picking up the Piece
	Piece movingPiece = getSquare(data.startX, data.startY);
	movingPiece.setMoved(true);
	lastMoveCapture = false;

	if(movingPiece.type == PAWN){
			//Check for double move
		if(fabs(data.endY - data.startY) == 2){
			setEP(data.endX, data.endY, turn);
		}
			//Check for enPassent capture
		else{
			Piece ePiece = getEP();
			if(data.endX == ePiece.getX() && data.startY == ePiece.getY()){
				setSquare(Piece(EMPTY), ePiece.getX(), ePiece.getY());
				lastMoveCapture = true;
			}
			setEP(Piece(EMPTY));
		}
		if(data.endY == 8 || data.endY == 1){
			switch(data.promotion){
			case(1) :
				movingPiece.type = QUEEN;
				break;
			case(2):
				movingPiece.type = KNIGHT;
				break;
			case(3):
				movingPiece.type = BISHOP;
				break;
			case(4):
				movingPiece.type = ROOK;
				break;
			default:
				break;
			}
		}
	}
	else if(EPdata != -1){
		setEP(Piece(EMPTY));
	}
	if(movingPiece.type == KING){
		//Move the rook if castling
		if(fabs(data.startX - data.endX) == 2){
			Piece getRook;
			if(data.endX == 3){
				getRook = getSquare(1, data.startY);
				setSquare(getRook, 4, data.startY);
				setSquare(Piece(EMPTY), 1, data.startY);
			}
			else if(data.endX == 7){
				getRook = getSquare(8, data.startY);
				setSquare(getRook, 6, data.startY);
				setSquare(Piece(EMPTY), 8, data.startY);
			}
			if(getRook.type != ROOK){
				std::cout << "info string rook type " << getRook.type << std::endl;
			}
		}
		setKingLocation(movingPiece.getColor(), data.endX, data.endY);
	}
		//Castling Rights handling
	if(data.startY == 1 || data.startY == 8 || data.endY == 1 || data.endY == 8){
			//King moved
		if(data.startX == 5 && data.startY == 1){
			setCastlingRights(true, true, false);
			setCastlingRights(true, false, false);
		}
		if(data.startX == 5 && data.startY == 8){
			setCastlingRights(false, true, false);
			setCastlingRights(false, false, false);
		}
			//Rook Moved
		if(data.startX == 1 && data.startY == 1){
			setCastlingRights(true, false, false);
		}
		if(data.startX == 1 && data.startY == 8){
			setCastlingRights(false, false, false);
		}
		if(data.startX == 8 && data.startY == 1){
			setCastlingRights(true, true, false);
		}
		if(data.startX == 8 && data.startY == 8){
			setCastlingRights(false, true, false);
		}
			//Something moved to the rooks square
		if(data.endX == 1 && data.endY == 1){
			setCastlingRights(true, false, false);
		}
		if(data.endX == 8 && data.endY == 1){
			setCastlingRights(true, true, false);
		}
		if(data.endX == 1 && data.endY == 8){
			setCastlingRights(false, false, false);
		}
		if(data.endX == 8 && data.endY == 8){
			setCastlingRights(false, true, false);
		}
	}

	setSquare(Piece(EMPTY), data.startX, data.startY);
		//Capturing anything in its way
	if(!getSquare(data.endX, data.endY).isNull()){
		lastMoveCapture = true;
	}
		//And placing it down
	movingPiece.xPos = data.endX;
	movingPiece.yPos = data.endY;
	setSquare(movingPiece, data.endX, data.endY);

	turn = !turn;
}

//Depth:    Expect:
// 0        1
// 1        20
// 2        400
// 3        8902
// 4        197281
// 5        4865609
// 6        119060324
double Board::perft(int depth){
	if(depth == 0) return(1);//No moves at 0 depth
	int moveGenCount = 0;
	Move moveList[220];
	generateMoveArray(moveList, moveGenCount);

	if(depth == 1) {
		return(moveGenCount);//How many moves can we make RIGHT NOW
	}

	Board newBoard;
	double moveCounter = 0;

	for(int i = 0; i < moveGenCount; i++){
		newBoard = this->newCopy();
		newBoard.makeMove(moveList[i]);
		moveCounter += newBoard.perft(depth-1);
	}

	return moveCounter;
}

double Board::dividePerft(int depth){
	if(depth == 0) return(1);//No moves at 0 depth

	int moveGenCount = 0;
	Move moveList[220];
	generateMoveArray(moveList, moveGenCount);
	if(depth == 1) {
		for(int i=0; i<moveGenCount; i++){
			std::cout << Engine::toAlg(moveList[i].startX) << moveList[i].startY << " "
				<< Engine::toAlg(moveList[i].endX) << moveList[i].endY << " " << std::endl;
		}
		return(moveGenCount);//How many moves can we make RIGHT NOW
	}

	Board newBoard;
	double moveCounter = 0;
	double newBoardMoveCount;

	for(int i=0; i < moveGenCount; i++){
		newBoard = this->newCopy();
		newBoard.makeMove(moveList[i]);
		newBoardMoveCount = newBoard.perft(depth-1);
		std::cout << Engine::toAlg(moveList[i].startX) << moveList[i].startY << " ";
		std::cout << Engine::toAlg(moveList[i].endX) << moveList[i].endY << " ";
		std::cout << std::fixed << newBoardMoveCount << std::endl;
		moveCounter += newBoardMoveCount;
	}
	return moveCounter;
}

Piece Board::findKing(bool getColor){
	return(getSquare(getKingX(getColor), getKingY(getColor)));
}

bool Board::getCastlingRights(bool color, bool side){
	char placer = 0;
	if(color){
		placer=2;
	}
	if(side){
		placer+=1;
	}
	return((castlingRights >> placer) & 1);
}
char Board::getCastlingRights(){
	return castlingRights;
}

void Board::setEP(Piece loadEP){
	if(loadEP.type == 0){
		EPdata = -1;
		return;
	}
	if(loadEP.color){
		EPdata = 1;
	}
	else{
		EPdata = 0;
	}
	EPdata <<= 3;
	EPdata |= loadEP.getY()-1;
	EPdata <<= 3;
	EPdata |= loadEP.getX()-1;
}
void Board::setEP(int x, int y, bool color){
	if (color){
		EPdata = 1;
	}
	else{
		EPdata = 0;
	}
	EPdata <<= 3;
	EPdata |= y - 1;
	EPdata <<= 3;
	EPdata |= x - 1;
}

Piece Board::getEP(){
	if(EPdata == -1){
		return Piece(EMPTY);
	}
	int x;
	int y;
	bool color;
	int copyData = EPdata;
	x = copyData & 7;
	copyData >>= 3;
	y = copyData & 7;
	copyData >>= 3;
	color = copyData & 1;

	Piece ePiece = Piece(PAWN);
	ePiece.xPos = x+1;
	ePiece.yPos = y+1;
	ePiece.color = color;
	return ePiece;
}

void Board::setCastlingRights(bool color, bool side, bool value){
	//FALSE QUEENSIDE TRUE KINGSIDE
	char placer = 0;
	if(color){
		placer=2;
	}
	if(side){
		placer+=1;
	}

	if(value){
			//sets the corresponding bit
		castlingRights = castlingRights | (1 << placer);
	}
	else{
			//Deletes the bit at the corresponding spot
		castlingRights = castlingRights & ~(1 << placer);
	}
}
void Board::setCastlingRights(char rights){
	castlingRights = rights;
}

void Board::countPieces(){
	int whitePieces[7];
	int blackPieces[7];
	int whiteCount = 0;
	int blackCount = 0;
	for(int i=0; i<7; i++){
		whitePieces[i] = 0;
		blackPieces[i] = 0;
	}
	int targetType;
	bool targetColor;
	for(int x=1; x<=8; x++){
		for(int y=1; y<=8; y++){
			targetType = getSquareType(x, y);
			targetColor = getSquareColor(x, y);

			if(targetColor){
				whitePieces[targetType]++;
				whiteCount++;
			}
			else{
				blackPieces[targetType]++;
				blackCount++;
			}
		}
	}
	std::cout << "Black:  " << blackCount << std::endl;
	std::cout << "	Pawns:  " << blackPieces[PAWN] << std::endl;
	std::cout << "	Queens:  " << blackPieces[QUEEN] << std::endl;
	std::cout << "	Kings:  " << blackPieces[KING] << std::endl;
	std::cout << "	Bishops  " << blackPieces[BISHOP] << std::endl;
	std::cout << "	Knights  " << blackPieces[KNIGHT] << std::endl;
	std::cout << "	Rooks  " << blackPieces[ROOK] << std::endl;

	std::cout << "White:  " << whiteCount << std::endl;
	std::cout << "	Pawns:  " << whitePieces[PAWN] << std::endl;
	std::cout << "	Queens:  " << whitePieces[QUEEN] << std::endl;
	std::cout << "	Kings:  " << whitePieces[KING] << std::endl;
	std::cout << "	Bishops  " << whitePieces[BISHOP] << std::endl;
	std::cout << "	Knights  " << whitePieces[KNIGHT] << std::endl;
	std::cout << "	Rooks  " << whitePieces[ROOK] << std::endl;
}

int Board::pieceCount(){
	int count = 0;

	for (bitBoard pieceSet = pieces[PAWN] | pieces[ROOK] | pieces[KNIGHT]
		| pieces[BISHOP] | pieces[QUEEN] | pieces[KING]; pieceSet; count++){
		pieceSet &= pieceSet - 1;
	}

	return count;
}
int Board::pieceCount(bool color){
	bitBoard mask = pieces[0];
	if (!color){
		mask = ~mask;
	}

	int count = 0;
	for (int i = 1; i < 7; i++){
		for (bitBoard pieceSet = pieces[i] & mask; pieceSet; count++){
			pieceSet &= pieceSet - 1;
		}
	}
	return count;
}
int Board::pieceCount(int type){
	int count = 0;
	for (bitBoard pieceSet = pieces[type]; pieceSet; count++){
		pieceSet &= pieceSet - 1;
	}
	return count;
}
int Board::pieceCount(int type, bool color){
	bitBoard mask = pieces[0];
	if (!color){
		mask = ~mask;
	}

	int count = 0;
	for (bitBoard pieceSet = pieces[type] & mask; pieceSet; count++){
		pieceSet &= pieceSet - 1;
	}
	return count;
}

bool Board::nullSquare(int x, int y){
	x--;
	y--;
	return  !(((pieces[1] | pieces[2] | pieces[3] | pieces[4] | pieces[5] | pieces[6])
		>> (8*x + y)) & 1);
}

bool Board::squareIsPopulated(int x, int y)
{
    x--;
	y--;
	return (allPieces >> (8*x + y)) & 1;
}

bool Board::squareIsType(int x, int y, int type){
	x--;
	y--;
	return (pieces[type] >> (8 * x + y)) & 1;
}
