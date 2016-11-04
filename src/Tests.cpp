#include <string>
#include <assert.h>
#include "Board.h"
#include "Piece.h"
#include "Engine.h"
#include "Move.h"

const int EMPTY  = 0;
const int PAWN   = 1;
const int QUEEN  = 2;
const int KING   = 3;
const int BISHOP = 4;
const int KNIGHT = 5;
const int ROOK   = 6;

void startingPerft_test()
{
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
    Board testBoard = Board();
    testBoard.loadFEN(startingFEN);

    double expectedPerfts[10] = { 1, 20, 400, 8902, 197281, 4865609, 119060324};

    for(int i = 0; i < 6; i++)
    {
        assert(testBoard.perft(i) == expectedPerfts[i]);
    }
}

void kiwipetePerft_test()
{
    std::string kiwipeteFEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    Board testBoard = Board();
    testBoard.loadFEN(kiwipeteFEN);

    double expectedPerfts[10] = { 1, 48, 2039, 97862, 4085603, 193690690 };

    for(int i = 0; i < 6; i++)
    {
        assert(testBoard.dividePerft(i) == expectedPerfts[i]);
    }
}

void loadStartingPosition_test()
{
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";

    Board testBoard = Board();
    testBoard.loadFEN(startingFEN);

    assert(testBoard.getSquareType(1, 1) == ROOK);
    assert(testBoard.getSquareType(2, 1) == KNIGHT);
    assert(testBoard.getSquareType(3, 1) == BISHOP);
    assert(testBoard.getSquareType(4, 1) == QUEEN);
    assert(testBoard.getSquareType(5, 1) == KING);
    assert(testBoard.getSquareType(6, 1) == BISHOP);
    assert(testBoard.getSquareType(7, 1) == KNIGHT);
    assert(testBoard.getSquareType(8, 1) == ROOK);

    assert(testBoard.getSquareType(1, 8) == ROOK);
    assert(testBoard.getSquareType(2, 8) == KNIGHT);
    assert(testBoard.getSquareType(3, 8) == BISHOP);
    assert(testBoard.getSquareType(4, 8) == QUEEN);
    assert(testBoard.getSquareType(5, 8) == KING);
    assert(testBoard.getSquareType(6, 8) == BISHOP);
    assert(testBoard.getSquareType(7, 8) == KNIGHT);
    assert(testBoard.getSquareType(8, 8) == ROOK);

    for (int i = 1; i <= 8; i++)
    {
        assert(testBoard.getSquareType(i, 2) == PAWN);
        assert(testBoard.getSquareType(i, 7) == PAWN);

        assert(testBoard.getSquareColor(i, 1) == true);
        assert(testBoard.getSquareColor(i, 2) == true);
        assert(testBoard.getSquareColor(i, 7) == false);
        assert(testBoard.getSquareColor(i, 8) == false);
    }
}

void runAllTests()
{
    loadStartingPosition_test();
    startingPerft_test();
    kiwipetePerft_test();
}
