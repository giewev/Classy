#include <iostream>
#include <string>
#include <iomanip>
#include "Board.h"
#include "Engine.h"
#include "Danger.h"
#include "Move.h"

void runAllTests();

int main(int argc, char *argv[])
{
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 't')
    {
        runAllTests();
        return 0;
    }

    return 0;
}
