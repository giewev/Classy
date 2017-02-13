#include <iostream>
#include <string>
#include <iomanip>
#include "Board.h"
#include "Engine.h"
#include "Danger.h"
#include "Move.h"
#include "ArgumentParser.h"

void runAllTests();

int main(int argc, char *argv[])
{
    ArgumentParser argParser = ArgumentParser(argc, argv);
    if (argParser.flagExists("-t"))
    {
        runAllTests();
    }
    else if (argParser.flagExists("-fen"))
    {
        int moveDepth = 5;
        if (argParser.flagExists("-depth"))
        {
            moveDepth = std::stoi(argParser.argumentContent("-depth"));
        }

        Board board = Board();
        board.loadFEN(argParser.argumentContent("-fen"));
        Engine engine = Engine(board);
        std::cout << engine.alphaBeta(moveDepth).basicAlg() << std::endl;
    }

    return 0;
}
