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
        Board board = Board();
        board.loadFEN(argParser.argumentContent("-fen"));
        Engine engine = Engine(board);

        if (argParser.flagExists("-depth"))
        {
            int moveDepth = std::stoi(argParser.argumentContent("-depth"));
            std::cout << engine.alphaBeta(moveDepth).basicAlg() << std::endl;
        }
        else if (argParser.flagExists("-time"))
        {
            double searchTime = std::stod(argParser.argumentContent("-time")) * 1000;
            std::cout << engine.iterativeSearch(searchTime).basicAlg() << std::endl;
        }
    }

    return 0;
}
