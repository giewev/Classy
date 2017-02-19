#include <random>
#include "ZobristHasher.h"
#include "Board.h"
#include "Move.h"
#include "Bitwise.h"
#include "Piece.h"

std::mt19937_64 ZobristHasher::hashGenerator;

ZobristHasher::ZobristHasher()
{
    for (int i = 0; i < 8; i++)
    {
        this->enPassantHashCodes[i] = generateHashCode();
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 6; k++)
            {
                this->whitePieceHashCodes[i][j][k] = generateHashCode();
                this->blackPieceHashCodes[i][j][k] = generateHashCode();
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        this->castlingHashCodes[i] = generateHashCode();
    }

    this->turnHashCode = generateHashCode();
}

ZobristHasher::ZobristHasher(Board board) : ZobristHasher()
{
    this->load(board);
}

ZobristHasher::ZobristHasher(const ZobristHasher& original)
{
    for (int i = 0; i < 8; i++)
    {
        this->enPassantHashCodes[i] = original.enPassantHashCodes[i];
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 6; k++)
            {
                this->whitePieceHashCodes[i][j][k] = original.whitePieceHashCodes[i][j][k];
                this->blackPieceHashCodes[i][j][k] = original.blackPieceHashCodes[i][j][k];
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        this->castlingHashCodes[i] = original.castlingHashCodes[i];
    }

    this->turnHashCode = original.turnHashCode;
    this->hashValue = original.hashValue;
}

void ZobristHasher::load(Board board)
{
    this->hashValue = 0ll;
    this->loadPieces(board);
    this->loadCastlingRights(board);
    this->loadEnPassant(board);
    if (board.turn)
    {
        this->hashValue ^= this->turnHashCode;
    }
}

void ZobristHasher::loadPieces(Board board)
{
    for (int i = 1; i <= 8; i ++)
    {
        for (int j = 1; j <= 8; j++)
        {
            int pieceType = board.getSquareType(i, j);
            if (pieceType != EMPTY)
            {
                this->togglePiece(i - 1, j - 1, pieceType, board.getSquareColor(i, j));
            }
        }
    }
}

void ZobristHasher::loadCastlingRights(Board board)
{
    bool boolValues[2] = { true, false };
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (board.getCastlingRights(boolValues[i], boolValues[j]))
            {
                this->toggleCastlingRights(boolValues[i], boolValues[j]);
            }
        }
    }
}

void ZobristHasher::loadEnPassant(Board board)
{
    Piece ep = board.getEP();
    if (ep.type != EMPTY)
    {
        this->toggleEnPassant(ep.getY() - 1);
    }
}

void ZobristHasher::togglePiece(int x, int y, int pieceType, bool color)
{
    if (x > 7 || x < 0 || y > 7 || y < 0 || pieceType > 6 || pieceType < 0)
    {
        throw "Coordinates out of bounds";
    }

    if (color)
    {
        this->hashValue ^= this->whitePieceHashCodes[x][y][pieceType - 1];
    }
    else
    {
        this->hashValue ^= this->blackPieceHashCodes[x][y][pieceType - 1];
    }
}

void ZobristHasher::toggleTurn()
{
    this->hashValue ^= this->turnHashCode;
}

void ZobristHasher::toggleEnPassant(int file)
{
    this->hashValue ^= this->enPassantHashCodes[file];
}

void ZobristHasher::toggleCastlingRights(bool color, bool side)
{
    int castlingIndex = 0;
    if (color)
    {
        castlingIndex += 2;
    }

    if (side)
    {
    castlingIndex++;
    }

    this->hashValue ^= this->castlingHashCodes[castlingIndex];
}

void ZobristHasher::update(Board prevBoard, Move nextMove)
{
    this->toggleTurn();
    this->updateEnPassant(prevBoard, nextMove);
    this->updateCastling(prevBoard, nextMove);
    this->updatePieces(prevBoard, nextMove);
}

void ZobristHasher::updateEnPassant(Board prevBoard, Move nextMove)
{
    Piece prevEP = prevBoard.getEP();
    if (prevEP.type != EMPTY)
    {
        this->toggleEnPassant(prevEP.getY() - 1);
    }

    if (prevBoard.getSquareType(nextMove.startX, nextMove.startY) == PAWN &&
        abs(nextMove.endY - nextMove.startY) == 2)
    {
        this->toggleEnPassant(nextMove.startY - 1);
    }
}

void ZobristHasher::updateCastling(Board prevBoard, Move nextMove)
{
    if (prevBoard.getSquareType(nextMove.startX, nextMove.startY) == KING)
    {
        bool kingColor = prevBoard.getSquareColor(nextMove.startX, nextMove.startY);
        if (prevBoard.getCastlingRights(kingColor, true))
        {
            this->toggleCastlingRights(kingColor, true);
        }

        if (prevBoard.getCastlingRights(kingColor, false))
        {
            this->toggleCastlingRights(kingColor, false);
        }
    }
    else if (prevBoard.getSquareType(nextMove.startX, nextMove.startY) == ROOK)
    {
        bool rookColor = prevBoard.getSquareColor(nextMove.startX, nextMove.startY);
        if ((nextMove.startY == 1 && rookColor == true) ||
            (nextMove.startY == 8 && rookColor == false))
        {
            if (nextMove.startX == 1 && prevBoard.getCastlingRights(rookColor, false))
            {
                this->toggleCastlingRights(rookColor, false);
            }
            else if (nextMove.startX == 8 && prevBoard.getCastlingRights(rookColor, true))
            {
                this->toggleCastlingRights(rookColor, true);
            }
        }
    }
}

void ZobristHasher::updatePieces(Board prevBoard, Move nextMove)
{
    int movingPieceType = prevBoard.getSquareType(nextMove.startX, nextMove.startY);
    bool movingPieceColor = prevBoard.getSquareColor(nextMove.startX, nextMove.startY);
    this->togglePiece(nextMove.startX - 1, nextMove.startY - 1, movingPieceType, movingPieceColor);
    this->togglePiece(nextMove.endX - 1, nextMove.endY - 1, movingPieceType, movingPieceColor);

    int capturePieceType = prevBoard.getSquareType(nextMove.endX, nextMove.endY);
    if (capturePieceType != EMPTY)
    {
        this->togglePiece(nextMove.endX - 1, nextMove.endY - 1, capturePieceType, !movingPieceColor);
    }

    if (movingPieceType == PAWN)
    {
        Piece prevEP = prevBoard.getEP();
        if (prevEP.type != EMPTY &&
            nextMove.startY == prevEP.yPos &&
            nextMove.endX == prevEP.xPos)
        {
            this->togglePiece(prevEP.xPos - 1, prevEP.yPos - 1, PAWN, prevEP.color);
        }

        if (nextMove.endY == 1 || nextMove.endY == 8)
        {
            int promotionType;
            switch (nextMove.promotion)
            {
            case(1):
                promotionType = QUEEN;
                break;
            case(2):
                promotionType = KNIGHT;
                break;
            case(3):
                promotionType = BISHOP;
                break;
            case(4):
                promotionType = ROOK;
                break;
            default:
                throw "Unknown Promotion Value";
            }

            this->togglePiece(nextMove.endX - 1, nextMove.endY - 1, movingPieceType, movingPieceColor);
            this->togglePiece(nextMove.endX - 1, nextMove.endY - 1, promotionType, movingPieceColor);
        }
    }

    if (movingPieceType == KING && abs(nextMove.startX - nextMove.endX) == 2)
    {
        if (nextMove.endX == 7)
        {
            this->togglePiece(7, nextMove.endY - 1, ROOK, movingPieceColor);
            this->togglePiece(5, nextMove.endY - 1, ROOK, movingPieceColor);
        }
        else if (nextMove.endX == 3)
        {
            this->togglePiece(0, nextMove.endY - 1, ROOK, movingPieceColor);
            this->togglePiece(3, nextMove.endY - 1, ROOK, movingPieceColor);
        }
    }
}

long long ZobristHasher::generateHashCode()
{
    std::uniform_int_distribution<long long> distribution;
    return distribution(ZobristHasher::hashGenerator);
}
