#include <vector>
#include <iostream>
#include <inttypes.h>
#include <math.h>

#include "Piece.h"
#include "Board.h"
#include "Move.h"
#include "Danger.h"
#include "Engine.h"
#include "Bitwise.h"
#include "ZobristHasher.h"

using namespace std;

#define bitBoard uint64_t

Board::Board()
{
    turn = true;
    lastMoveCapture = false;
    castlingRights = 15;
    EPdata = -1;
    allPieces = 0ull;
    for(int i=0; i<7; i++)
    {
        pieces[i] = 0ull;
    }
}

Board::Board(int null)
{
}

Board::~Board()
{
}

void Board::loadFEN(std::string fenFile)
{
    unsigned int bookmark = 0;

    //Blanking out the Board
    for(int i=0; i<7; i++)
    {
        pieces[i] = 0ull;
    }

    int x = 0;
    int y = 7;

    for(unsigned int i=0; i<fenFile.length(); i++)
    {

        //checking for every value that represents a piece
        if     (fenFile[i] == 'r')
        {
            setSquare(Piece(PieceType::Rook, x, y, false), x, y);
        }
        else if(fenFile[i] == 'R')
        {
            setSquare(Piece(PieceType::Rook, x, y, true), x, y);
        }
        else if(fenFile[i] == 'n')
        {
            setSquare(Piece(PieceType::Knight, x, y, false), x, y);
        }
        else if(fenFile[i] == 'N')
        {
            setSquare(Piece(PieceType::Knight, x, y, true), x, y);
        }
        else if(fenFile[i] == 'b')
        {
            setSquare(Piece(PieceType::Bishop, x, y, false), x, y);
        }
        else if(fenFile[i] == 'B')
        {
            setSquare(Piece(PieceType::Bishop, x, y, true), x, y);
        }
        else if(fenFile[i] == 'q')
        {
            setSquare(Piece(PieceType::Queen, x, y, false), x, y);
        }
        else if(fenFile[i] == 'Q')
        {
            setSquare(Piece(PieceType::Queen, x, y, true), x, y);
        }
        else if(fenFile[i] == 'k')
        {
            setKingLocation(false, x, y);
            setSquare(Piece(PieceType::King, x, y, false), x, y);
        }
        else if(fenFile[i] == 'K')
        {
            setKingLocation(true, x, y);
            setSquare(Piece(PieceType::King, x, y, true), x, y);
        }
        else if(fenFile[i] == 'p')
        {
            setSquare(Piece(PieceType::Pawn, x, y, false), x, y);
        }
        else if(fenFile[i] == 'P')
        {
            setSquare(Piece(PieceType::Pawn, x, y, true), x, y);
        }

        //Checking if the value was a number
        if(fenFile[i] != '/')
        {
            if(fenFile[i] >= 49 && fenFile[i] <= 56)
            {
                //If it is jump ahead some squares
                x += fenFile[i] - 48;
            }
            else
            {
                //If it isnt we just go to the next one
                x += 1;
            }
        }

        //next Rank
        if(x >= 8)
        {
            x = 0;
            y -= 1;
        }

        //Done setting pieces up
        if(y == -1)
        {
            bookmark = i+1;
            break;
        }
    }

    //setting turn
    for(; bookmark < fenFile.length(); bookmark++)
    {
        if(fenFile[bookmark] == 'w')
        {
            turn = true;
            bookmark++;
            break;
        }
        else if(fenFile[bookmark] == 'b')
        {
            turn = false;
            bookmark++;
            break;
        }
        else
        {
            continue;
        }
    }

    //Setting Castling Rights
    setCastlingRights(0);
    bool breakOut = false;
    bool startedCastling = false;
    while (bookmark < fenFile.length())
    {
        switch (fenFile[bookmark])
        {
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
        if (breakOut && startedCastling)
        {
            break;
        }
    }

    //Setting EnPassent
    while (bookmark < fenFile.length())
    {
        if (fenFile[bookmark] == ' ')
        {
            bookmark++;
            continue;
        }
        if (fenFile[bookmark] == '-')
        {
            bookmark++;
            break;
        }
        else if (bookmark + 1 < fenFile.length())
        {
            int rank;
            if (fenFile[bookmark + 1] == '3')
            {
                rank = 3;
            }
            else
            {
                rank = 4;
            }
            setEP(fenFile[bookmark]-96, rank, !turn);
            break;
        }
        bookmark += 2;
    }

    //setting halfmove clock
    while (bookmark < fenFile.length())
    {
        if (fenFile[bookmark] >= 48 && fenFile[bookmark] <= 57)
        {
            if (bookmark + 1 < fenFile.length() &&
                    fenFile[bookmark + 1] >= 48 && fenFile[bookmark + 1] <= 57)
            {
                halfMoveCounter = 10 * (fenFile[bookmark] - 48) + fenFile[bookmark + 1] - 48;
                bookmark += 2;
                break;
            }
            else
            {
                halfMoveCounter = fenFile[bookmark] - 48;
                bookmark++;
                break;
            }
        }
        else
        {
            bookmark++;
        }
    }

    //setting fullMove clock
    while (bookmark < fenFile.length())
    {
        if (fenFile[bookmark] >= 48 && fenFile[bookmark] <= 57)
        {
            if (bookmark + 1 < fenFile.length() &&
                    fenFile[bookmark + 1] >= 48 && fenFile[bookmark + 1] <= 57)
            {
                moveCounter = 10 * (fenFile[bookmark] - 48) + fenFile[bookmark + 1] - 48;
                bookmark += 2;
                break;
            }
            else
            {
                moveCounter = fenFile[bookmark] - 48;
                bookmark++;
                break;
            }
        }
        else
        {
            bookmark++;
        }
    }

    this->hasher = ZobristHasher(*this);
}

string Board::outputFEN() const
{
    string FEN = "";
    int count = 0;
    for(int y = 7; y >= 0; y--)
    {
        for(int x = 0; x < 8; x++)
        {
            PieceType pieceType = getSquareType(x, y);
            bool pieceColor = getSquareColor(x, y);
            if(pieceColor)
            {
                if(pieceType == PieceType::Empty)
                {
                    count += 1;
                }
                else
                {
                    if (count)
                    {
                        FEN += char(48 + count);
                        count = 0;
                    }

                    if (pieceType == PieceType::Pawn)
                    {
                        FEN += "P";
                    }
                    else if (pieceType == PieceType::Rook)
                    {
                        FEN += "R";
                    }
                    else if (pieceType == PieceType::Knight)
                    {
                        FEN += "N";
                    }
                    else if (pieceType == PieceType::Bishop)
                    {
                        FEN += "B";
                    }
                    else if (pieceType == PieceType::Queen)
                    {
                        FEN += "Q";
                    }
                    else if (pieceType == PieceType::King)
                    {
                        FEN += "K";
                    }
                    else
                    {
                        count = 1;
                    }
                }
            }
            else
            {
                if(pieceType == PieceType::Empty)
                {
                    count += 1;
                }
                else
                {
                    if (count)
                    {
                        FEN += char(48 + count);
                        count = 0;
                    }

                    if (pieceType == PieceType::Pawn)
                    {
                        FEN += "p";
                    }
                    else if (pieceType == PieceType::Rook)
                    {
                        FEN += "r";
                    }
                    else if (pieceType == PieceType::Knight)
                    {
                        FEN += "n";
                    }
                    else if (pieceType == PieceType::Bishop)
                    {
                        FEN += "b";
                    }
                    else if (pieceType == PieceType::Queen)
                    {
                        FEN += "q";
                    }
                    else if (pieceType == PieceType::King)
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
        if(count)
        {
            FEN += char(48 + count);
            count = 0;
        }
        FEN += "/";
    }
    if(turn)
    {
        FEN += " w";
    }
    else
    {
        FEN += " b";
    }
    return FEN;
}

//Creates a new copy of the current Board and all of its contents
Board Board::newCopy() const
{
    Board newBoard = Board(0);
    for(int i=0; i<7; i++)
    {
        newBoard.pieces[i] = pieces[i];
    }

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
    newBoard.hasher = ZobristHasher(this->hasher);

    return(newBoard);
}

Piece Board::getSquare(int x, int y) const
{
    throwIfOutOfBounds(x, y);
    int type = 0;
    bool color = (pieces[0] >> (8 * x + y)) & 1;
    //Empty SquareCheck
    if (false && !(((pieces[PieceType::Pawn] | pieces[PieceType::Bishop] | pieces[PieceType::Knight] |
                     pieces[PieceType::Queen] | pieces[PieceType::Rook] | pieces[PieceType::King]) >> (8 * x + y)) & 1))
    {
        return Piece(PieceType::Empty, x, y, color);
    }
    for(int i=1; i<7; i++)
    {
        if(((pieces[i] >> (8*x + y)) & 1))
        {
            type = i;
            break;
        }
    }
    return Piece((PieceType)type, x, y, color);
}

PieceType Board::getSquareType(int x, int y) const
{
    throwIfOutOfBounds(x, y);
    if (!squareIsPopulated(x, y))
    {
        return PieceType::Empty;
    }

    for(int i=1; i<7; i++)
    {
        if((pieces[i] >> (8*x + y)) & 1)
        {
            return (PieceType)i;
        }
    }

    return PieceType::Empty;
}

bool Board::getSquareColor(int x, int y) const
{
    throwIfOutOfBounds(x, y);
    return (pieces[0] >> (8*x + y)) & 1;
}

int Board::getKingX(bool getColor) const
{
    if(getColor)
    {
        return((kingCoordinates & 7));
    }
    else
    {
        return(((kingCoordinates >> 6) & 7));
    }
}

int Board::getKingY(bool getColor) const
{
    if(getColor)
    {
        return(((kingCoordinates >> 3) & 7));
    }
    else
    {
        return(((kingCoordinates >> 9) & 7));
    }
}

void Board::setSquare(Piece setPiece, int x, int y)
{
    setSquare(setPiece.type, setPiece.color, x, y);
}

void Board::setSquare(PieceType type, bool color, int x, int y)
{
    throwIfOutOfBounds(x, y);
    for(int i=1; i<7; i++)
    {
        pieces[i] &= ~(1ull << (8*x + y));
    }
    if(type > 0)
    {
        pieces[type] |= (1ull << (8*x + y));
        allPieces |= (1ull << (8*x + y));
    }
    else
    {
        allPieces &= ~(1ull << (8*x + y));
    }

    if(color)
    {
        pieces[0] |= (1ull << (8*x + y));
    }
    else
    {
        pieces[0] &= ~(1ull << (8*x + y));
    }
}

void Board::setKingLocation(bool setColor, int x, int y)
{
    throwIfOutOfBounds(x, y);
    int placeHolder;
    if(setColor)
    {
        placeHolder = kingCoordinates >> 6;
        kingCoordinates = (placeHolder<<6) | x | (y<<3);
    }
    else
    {
        placeHolder = kingCoordinates & ((1<<6)-1);
        kingCoordinates = placeHolder | (x<<6) | (y<<9);
    }
}

void Board::generateMoveArray(Move* finalMoveList, int& moveCounter) const
{
    for(int y = 0; y < 8; y++)
    {
        for(int x = 0; x < 8; x++)
        {
            if(getSquareColor(x, y) == turn)
            {
                Piece::appendMoveArray(finalMoveList, moveCounter, x, y, *this);
            }
        }
    }

    Danger safetyData = Danger(*this);
    for(int i = moveCounter - 1; i >= 0; i--)
    {
        if(!finalMoveList[i].isSafe(safetyData))
        {
            finalMoveList[i] = finalMoveList[moveCounter - 1];
            moveCounter--;
        }
    }
}

void Board::generateCaptureMoves(Move* moveList, int& moveCounter) const
{
    for(int y = 0; y < 8; y++)
    {
        for(int x = 0; x < 8; x++)
        {
            if(getSquareColor(x, y) == turn)
            {
                Piece::appendMoveArray(moveList, moveCounter, x, y, *this);
            }
        }
    }

    for (int i = moveCounter - 1; i >= 0; i--)
    {
        if (!moveList[i].isCapture(*this))
        {
            moveCounter--;
            moveList[i] = moveList[moveCounter];
        }
    }

    if (moveCounter > 0)
    {
        Danger safetyData = Danger(*this);
        for(int i = moveCounter - 1; i >= 0; i--)
        {
            if(!moveList[i].isSafe(safetyData))
            {
                moveList[i] = moveList[moveCounter - 1];
                moveCounter--;
            }
        }
    }
}

int Board::gameOverCheck() const
{
    int moveCounter = 0;
    Move rawMoveList[27];
    Danger safetyData = Danger(*this);
    for(int y = 0; y < 8; y++)
    {
        for(int x = 0; x < 8; x++)
        {
            //What Piece is there
            if(getSquareType(x, y) == PieceType::Empty)
            {
                continue;
            }
            //If the piece is the right color, add its moves to the list
            //if(getSquareColor(x, y) == turn){
            if (getSquareColor(x, y) == turn)
            {
                Piece target = getSquare(x, y);
                moveCounter = 0;
                target.appendMoveArray(rawMoveList, moveCounter, x, y, *this);
                for(int i=0; i<moveCounter; i++)
                {
                    if(rawMoveList[i].isSafe(safetyData))
                    {
                        return 0;
                    }
                }
            }
        }
    }
    if(safetyData.getCheck())
    {
        //CheckMate
        return 1;
    }
    else
    {
        //StaleMate
        return 2;
    }
    return 0;
}

void Board::makeMove(Move data)
{
    this->hasher.update(*this, data);

    //Picking up the Piece
    Piece movingPiece = getSquare(data.startX, data.startY);
    movingPiece.setMoved(true);
    lastMoveCapture = false;

    if(movingPiece.type == PieceType::Pawn)
    {
        //Check for double move
        if(fabs(data.endY - data.startY) == 2)
        {
            setEP(data.endX, data.endY, turn);
        }
        //Check for enPassent capture
        else
        {
            Piece ePiece = getEP();
            if(data.endX == ePiece.getX() && data.startY == ePiece.getY())
            {
                setSquare(Piece(PieceType::Empty), ePiece.getX(), ePiece.getY());
                lastMoveCapture = true;
            }
            setEP(Piece(PieceType::Empty));
        }
        if(data.endY == 7 || data.endY == 0)
        {
            movingPiece.type = data.promotion;
        }
    }
    else if(EPdata != -1)
    {
        setEP(Piece(PieceType::Empty));
    }
    if(movingPiece.type == PieceType::King)
    {
        //Move the rook if castling
        if(fabs(data.startX - data.endX) == 2)
        {
            Piece getRook;
            if(data.endX == 2)
            {
                getRook = getSquare(0, data.startY);
                setSquare(getRook, 3, data.startY);
                setSquare(Piece(PieceType::Empty), 0, data.startY);
            }
            else if(data.endX == 6)
            {
                getRook = getSquare(7, data.startY);
                setSquare(getRook, 5, data.startY);
                setSquare(Piece(PieceType::Empty), 7, data.startY);
            }
        }
        setKingLocation(movingPiece.getColor(), data.endX, data.endY);
    }

    //King moved
    if(data.startX == 4 && data.startY == 0)
    {
        setCastlingRights(true, true, false);
        setCastlingRights(true, false, false);
    }
    if(data.startX == 4 && data.startY == 7)
    {
        setCastlingRights(false, true, false);
        setCastlingRights(false, false, false);
    }

    //Castling Rights handling
    if(data.startY == 0 || data.startY == 7 || data.endY == 0 || data.endY == 7)
    {
        //Rook Moved
        if(data.startX == 0 && data.startY == 0)
        {
            setCastlingRights(true, false, false);
        }
        if(data.startX == 0 && data.startY == 7)
        {
            setCastlingRights(false, false, false);
        }
        if(data.startX == 7 && data.startY == 0)
        {
            setCastlingRights(true, true, false);
        }
        if(data.startX == 7 && data.startY == 7)
        {
            setCastlingRights(false, true, false);
        }
        //Something moved to the rooks square
        if(data.endX == 0 && data.endY == 0)
        {
            setCastlingRights(true, false, false);
        }
        if(data.endX == 7 && data.endY == 0)
        {
            setCastlingRights(true, true, false);
        }
        if(data.endX == 0 && data.endY == 7)
        {
            setCastlingRights(false, false, false);
        }
        if(data.endX == 7 && data.endY == 7)
        {
            setCastlingRights(false, true, false);
        }
    }

    setSquare(Piece(PieceType::Empty), data.startX, data.startY);
    //Capturing anything in its way
    if(!getSquare(data.endX, data.endY).isNull())
    {
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
double Board::perft(int depth) const
{
    if(depth == 0) return(1);//No moves at 0 depth
    int moveGenCount = 0;
    Move moveList[220];
    generateMoveArray(moveList, moveGenCount);

    if(depth == 1)
    {
        return(moveGenCount);//How many moves can we make RIGHT NOW
    }

    Board newBoard;
    double moveCounter = 0;

    for(int i = 0; i < moveGenCount; i++)
    {
        newBoard = this->newCopy();
        newBoard.makeMove(moveList[i]);
        moveCounter += newBoard.perft(depth-1);
    }

    return moveCounter;
}

double Board::dividePerft(int depth) const
{
    if(depth == 0) return(1);//No moves at 0 depth

    int moveGenCount = 0;
    Move moveList[220];
    generateMoveArray(moveList, moveGenCount);
    if(depth == 1)
    {
        for(int i=0; i<moveGenCount; i++)
        {
            std::cout << moveList[i].basicAlg() << " " << std::endl;
        }
        return(moveGenCount);//How many moves can we make RIGHT NOW
    }

    Board newBoard;
    double moveCounter = 0;
    double newBoardMoveCount;

    for(int i=0; i < moveGenCount; i++)
    {
        newBoard = this->newCopy();
        newBoard.makeMove(moveList[i]);
        newBoardMoveCount = newBoard.perft(depth-1);
        std::cout << moveList[i].basicAlg() << " ";
        std::cout << std::fixed << newBoardMoveCount << std::endl;
        moveCounter += newBoardMoveCount;
    }
    return moveCounter;
}

Piece Board::findKing(bool getColor) const
{
    return(getSquare(getKingX(getColor), getKingY(getColor)));
}

bool Board::getCastlingRights(bool color, bool side) const
{
    char placer = 0;
    if(color)
    {
        placer=2;
    }
    if(side)
    {
        placer+=1;
    }
    return((castlingRights >> placer) & 1);
}
char Board::getCastlingRights() const
{
    return castlingRights;
}

void Board::setEP(Piece loadEP)
{
    if(loadEP.type != PieceType::Pawn)
    {
        EPdata = -1;
        return;
    }
    if(loadEP.color)
    {
        EPdata = 1;
    }
    else
    {
        EPdata = 0;
    }
    EPdata <<= 3;
    EPdata |= loadEP.getY();
    EPdata <<= 3;
    EPdata |= loadEP.getX();
}

void Board::setEP(int x, int y, bool color)
{
    throwIfOutOfBounds(x, y);
    if (color)
    {
        EPdata = 1;
    }
    else
    {
        EPdata = 0;
    }
    EPdata <<= 3;
    EPdata |= y;
    EPdata <<= 3;
    EPdata |= x;
}

Piece Board::getEP() const
{
    if(EPdata == -1)
    {
        return Piece(PieceType::Empty);
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

    Piece ePiece = Piece(PieceType::Pawn);
    ePiece.xPos = x;
    ePiece.yPos = y;
    ePiece.color = color;
    return ePiece;
}

void Board::setCastlingRights(bool color, bool side, bool value)
{
    //FALSE PieceType::QueenSIDE TRUE PieceType::KingSIDE
    char placer = 0;
    if(color)
    {
        placer=2;
    }
    if(side)
    {
        placer+=1;
    }

    if(value)
    {
        //sets the corresponding bit
        castlingRights = castlingRights | (1 << placer);
    }
    else
    {
        //Deletes the bit at the corresponding spot
        castlingRights = castlingRights & ~(1 << placer);
    }
}

void Board::setCastlingRights(char rights)
{
    castlingRights = rights;
}

void Board::countPieces() const
{
    std::cout << "Black:  " << pieceCount(false) << std::endl;
    std::cout << "	Pawns:  " << pieceCount(PieceType::Pawn, false) << std::endl;
    std::cout << "	Queens:  " << pieceCount(PieceType::Queen, false) << std::endl;
    std::cout << "	Kings:  " << pieceCount(PieceType::King, false) << std::endl;
    std::cout << "	Bishops  " << pieceCount(PieceType::Bishop, false) << std::endl;
    std::cout << "	Knights  " << pieceCount(PieceType::Knight, false) << std::endl;
    std::cout << "	Rooks  " << pieceCount(PieceType::Rook, false) << std::endl;

    std::cout << "White:  " << pieceCount(true) << std::endl;
    std::cout << "	Pawns:  " << pieceCount(PieceType::Pawn, true) << std::endl;
    std::cout << "	Queens:  " << pieceCount(PieceType::Queen, true) << std::endl;
    std::cout << "	Kings:  " << pieceCount(PieceType::King, true) << std::endl;
    std::cout << "	Bishops  " << pieceCount(PieceType::Bishop, true) << std::endl;
    std::cout << "	Knights  " << pieceCount(PieceType::Knight, true) << std::endl;
    std::cout << "	Rooks  " << pieceCount(PieceType::Rook, true) << std::endl;
}

int Board::pieceCount() const
{
    return bitwise::countBits(allPieces);
}

int Board::pieceCount(bool color) const
{
    bitBoard mask = pieces[0];
    if (!color)
    {
        mask = ~mask;
    }

    return bitwise::countBits(allPieces & mask);
}

int Board::pieceCount(PieceType type) const
{
    return bitwise::countBits(pieces[type]);
}

int Board::pieceCount(PieceType type, bool color) const
{
    bitBoard mask = pieces[0];
    if (!color)
    {
        mask = ~mask;
    }

    return bitwise::countBits(pieces[type] & mask);
}

bool Board::squareIsPopulated(int x, int y) const
{
    throwIfOutOfBounds(x, y);
    return (allPieces >> (8*x + y)) & 1;
}

bool Board::squareIsType(int x, int y, int type) const
{
    throwIfOutOfBounds(x, y);
    return (pieces[type] >> (8 * x + y)) & 1;
}

void Board::throwIfOutOfBounds(int x, int y)
{
    if (x < 0 || x > 7 || y < 0 || y > 7)
    {
        throw "Trying to get out of bounds coordinates";
    }
}

bool Board::operator==(const Board &other) const
{
    for (int i = 1; i < 7; i++)
    {
        if (this->pieces[i] != other.pieces[i])
        {
            return false;
        }
    }

    if ((this->pieces[0] & this->allPieces) != (other.pieces[0] & other.allPieces))
    {
        return false;
    }

    if ((~this->pieces[0] & this->allPieces) != (~other.pieces[0] & other.allPieces))
    {
        return false;
    }

    return this->turn == other.turn &&
        this->EPdata == other.EPdata &&
        this->castlingRights == other.castlingRights;
}

size_t Board::getHashCode() const
{
    return this->hasher.hashValue;
}

