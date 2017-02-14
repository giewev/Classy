#include "Bitwise.h"
#include <iostream>

const int EMPTY  = 0;
const int PAWN   = 1;
const int QUEEN  = 2;
const int KING   = 3;
const int BISHOP = 4;
const int KNIGHT = 5;
const int ROOK   = 6;

void bitwise::trimBottom(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
        toTrim &= ~(firstRank << i);
    }
}
void bitwise::trimTop(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
        toTrim &= ~(firstRank << (7 - i));
    }
}
void bitwise::trimLeft(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
        toTrim &= ~(firstFile << (i * 8));
    }
}
void bitwise::trimRight(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
        toTrim &= ~(firstFile << (8 * (7 - i)));
    }
}

int bitwise::countBits(bitBoard data)
{
    int count = 0;
    while (data)
    {
        data &= data - 1;
        count++;
    }
    return count;
}

bitBoard bitwise::knightSquares(int x, int y)
{
    bitBoard toReturn = knightMovement;
    //std::cout << toReturn << std::endl;
    if (x < 3)
    {
        trimLeft(toReturn, 3 - x);
    }
    else if (x > 6)
    {
        trimRight(toReturn, x - 3);
    }
    if (y < 3)
    {
        trimBottom(toReturn, 3 - y);
    }
    if (y > 6)
    {
        trimTop(toReturn, y - 3);
    }

    int toShift = -18;
    x--;
    y--;
    toShift += y;
    toShift += x * 8;
    //std::cout << "to shift " << toShift << std::endl;
    if (toShift < 0)
    {
        toReturn >>= -toShift;
    }
    else
    {
        toReturn <<= toShift;
    }
    return toReturn;
}

int bitwise::firstBit(bitBoard input)
{
    //std::cout << "info string starting first bit" << std::endl;
    int index = 0;
    while (!(input & 1) && index < 64)
    {
        input >>= 1;
        index++;
    }
    //std::cout << "info string ending first bit" << std::endl;
    if (index == 64)
    {
        return -1;
    }
    else
    {
        return index;
    }
}
int bitwise::lastBit(bitBoard input)
{
    //std::cout << "info string starting last bit" << std::endl;
    bitBoard save = input;
    while (input)
    {
        input &= (input - 1);
        if (input)
        {
            save = input;
        }
    }
    //std::cout << "info string ending last bit" << std::endl;
    return firstBit(save);
}

int bitwise::getX(int raw)
{
    return (raw >> 3) + 1;
}
int bitwise::getY(int raw)
{
    return (raw % 8) + 1;
}
