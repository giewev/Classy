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
        return 0;
    }

    return 0;
}
