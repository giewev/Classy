#include <iostream>
#include <string>
#include <iomanip>
#include "Board.h"
#include "Engine.h"
#include "Danger.h"
#include "Move.h"

int main(int argc, char *argv[]){
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";

    Board* testBoard = new Board();
    testBoard->loadFEN(startingFEN);
    Engine player = Engine(testBoard);

    std::cout << std::fixed << std::setprecision(0);

    double expectedPerfts[10] = { 1, 20, 400, 8902, 197281, 4865609, 119060324};

    for(int i=0; i<6; i++)
    {
        std::cout << testBoard->perft(i) << std::endl;
    }

    return 0;
}
