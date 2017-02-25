#include "Danger.h"
#include "Engine.h"
#include "Evaluation Constants.h"
#include "Piece.h"
#include "Bitwise.h"
#include "FullEvaluator.h"

#include <iostream>
#include <math.h>
#include <random>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string.h>
#include <strings.h>

using namespace std;

Engine::Engine()
{
    gameBoard = Board();
    score = 0;
    evaluator = FullEvaluator();
}

Engine::Engine(Board loadBoard)
{
    gameBoard = loadBoard;
    score = 0;
    evaluator = FullEvaluator();
}

Engine::~Engine()
{
}


Board Engine::getBoard()
{
    return gameBoard;
}
int Engine::getScore()
{
    return score;
}

void Engine::setBoard(Board loadBoard)
{
    gameBoard = loadBoard;
}
void Engine::setScore(int loadScore)
{
    score = loadScore;
}

Move Engine::minMax(int depth, Board searchBoard)
{
    int moveCount = 0;
    Move moveList[220];
    searchBoard.generateMoveArray(moveList, moveCount);

    Board newBoard;
    double moveScore;
    double bestScore;
    unsigned int bestIndex = 0;
    bestScore = 999;
    if(searchBoard.turn)
    {
        bestScore = -999;
    }
    for(int i=0; i<moveCount; i++)
    {
        newBoard = searchBoard.newCopy();
        newBoard.makeMove(moveList[i]);
        if(depth == 1)
        {
            moveScore = evaluator.evaluate(newBoard);
        }
        else
        {
            moveScore = minMax(depth-1, newBoard).getScore();
        }
        moveList[i].setScore(moveScore);
        if(moveScore > bestScore && searchBoard.turn)
        {
            bestScore = moveScore;
            bestIndex = i;
        }
        else if(moveScore < bestScore && !searchBoard.turn)
        {
            bestScore = moveScore;
            bestIndex = i;
        }
        else if(moveScore == bestScore && rand() % 2)
        {
            bestScore = moveScore;
            bestIndex = i;
        }
    }
    return moveList[bestIndex];
}
Move Engine::minMax(int depth)
{
    return(minMax(depth, gameBoard));
}

Move Engine::alphaBeta(int depth, Board searchBoard, double bound)
{
    TranspositionCache transposition = this->getTransposition(searchBoard);
    if (transposition.depth >= depth)
    {
        if (transposition.fullyEvaluated || // Guarenteed to be at least as good as answer as we would get
            (transposition.bestMove.score > bound && searchBoard.turn) || // We know it's a white cutoff
            (transposition.bestMove.score < bound && !searchBoard.turn)) // We know it's a black cutoff
        {
            return transposition.bestMove;
        }
    }

    int moveCount = 0;
    Move moveList[220];
    searchBoard.generateMoveArray(moveList, moveCount);
    if (transposition.depth >= 0)
    {
        for (int i = 0; i < moveCount; i++)
        {
            if (moveList[i] == transposition.bestMove)
            {
                moveList[i] = moveList[0];
                moveList[0] = transposition.bestMove;
                break;
            }
        }
    }

    Board newBoard;
    double moveScore;
    double bestScore;
    int bestDepth = -1;
    unsigned int bestIndex = 0;
    Move returnedMove;

    bestScore = 999;
    if(searchBoard.turn)
    {
        bestScore = -999;
    }
    //Checkmate or StaleMate
    if(moveCount == 0)
    {
        returnedMove = Move();
        returnedMove.setGameOverDepth(0);

        returnedMove.setScore(evaluator.evaluate(searchBoard));
        this->updateTranspositionBestIfDeeper(searchBoard, depth, returnedMove);
        return returnedMove;
    }

    for(int i=0; i<moveCount; i++)
    {
        newBoard = searchBoard.newCopy();
        newBoard.makeMove(moveList[i]);

        if(depth == 1)
        {
            moveScore = evaluator.evaluate(newBoard);
            if(moveScore != 1000)
            {
                moveList[i].setScore(moveScore);
            }
            else
            {
                moveScore = 0;
                moveList[i].setGameOverDepth(1);
                moveList[i].setScore(0);
            }
            if(moveScore == 999 || moveScore == -999)
            {
                moveList[i].setGameOverDepth(1);
            }
        }
        else
        {
            returnedMove = alphaBeta(depth - 1, newBoard, bestScore);
            moveScore = returnedMove.getScore();
            if(returnedMove.getGameOverDepth() != -1)
            {
                moveList[i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
                if (moveScore > 0)
                {
                    moveScore--;
                }
                else
                {
                    moveScore++;
                }
            }
            moveList[i].setScore(moveScore);
        }
        if(searchBoard.turn)
        {
            //Alpha Beta break
            if(moveScore > bound)
            {
                return(moveList[i]);
            }
            //Best move weve found
            if(moveScore > bestScore)
            {
                bestDepth = moveList[i].getGameOverDepth();
                bestScore = moveScore;
                bestIndex = i;
            }
        }
        //Its blacks turn
        else
        {
            //Alpha Beta Break
            if(moveScore < bound)
            {
                return(moveList[i]);
            }
            //Best move weve found
            if(moveScore < bestScore)
            {
                bestDepth = moveList[i].getGameOverDepth();
                bestScore = moveScore;
                bestIndex = i;
            }
        }
        //Exactly equal, so flip a coin
        if(moveScore == bestScore)
        {

            //GameOver score handling
            int modifier = -1;
            if(searchBoard.turn)
            {
                modifier = 1;
            }
            //Good result
            if(moveScore == 999 * modifier)
            {
                if(moveList[i].getGameOverDepth() < bestDepth || bestDepth == -1)
                {
                    bestScore = moveScore;
                    bestIndex = i;
                    bestDepth = moveList[i].getGameOverDepth();
                }
            }	//Bad result
            else if(moveScore == -999 * modifier)
            {
                if(moveList[i].getGameOverDepth() > bestDepth || bestDepth == -1)
                {
                    bestScore = moveScore;
                    bestIndex = i;
                    bestDepth = moveList[i].getGameOverDepth();
                }
            }
            else if(rand() % 2)
            {
                bestScore = moveScore;
                bestIndex = i;
                bestDepth = moveList[i].getGameOverDepth();
            }
        }
    }

    this->updateTranspositionBestIfDeeper(searchBoard, depth, moveList[bestIndex]);
    return moveList[bestIndex];
}

Move Engine::alphaBeta(int depth)
{
    if(gameBoard.turn)
    {
        return alphaBeta(depth, gameBoard, 999);
    }
    else
    {
        return alphaBeta(depth, gameBoard, -999);
    }
}
Move Engine::alphaBeta(int depth, double bound)
{
    return alphaBeta(depth, gameBoard, bound);
}

Move Engine::iterativeSearch(int milliseconds)
{
    time_t timer = time(NULL);
    Move bestMove;
    int depth = 1;
    while (difftime(time(NULL), timer) < milliseconds / 2000.0)
    {
        bestMove = alphaBeta(depth++);
        std::cout << "Searched a depth of " << depth << " in " << difftime(time(NULL), timer) << " Seconds" << std::endl;
    }

    return bestMove;
}

std::string Engine::toAlg(int val)
{
    if(val < 0 || val > 7)
    {
        return "z";
    }

    std::string alpha[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    return(alpha[val]);
}
int Engine::fromAlg(char val)
{
    if(val > 95 && val < 105)
    {
        return int(9-(105 - val));
    }
    else if(val == '1')
    {
        return 1;
    }
    else if(val == '2')
    {
        return 2;
    }
    else if(val == '3')
    {
        return 3;
    }
    else if(val == '4')
    {
        return 4;
    }
    else if(val == '5')
    {
        return 5;
    }
    else if(val == '6')
    {
        return 6;
    }
    else if(val == '7')
    {
        return 7;
    }
    else if(val == '8')
    {
        return 8;
    }
    return 0;
}

void Engine::sortMoveList(Move* rawList, int moveCount, Board* sortBoard)
{
    int captures = 0;
    int nonCaptures = 0;
    Move captureList[230];
    Move nonCaptureList[230];

    for(int i = 0; i < moveCount; i++)
    {
        if(rawList[i].isCapture(sortBoard))
        {
            captureList[captures] = rawList[i];
            captures++;
        }
        else
        {
            nonCaptureList[nonCaptures] = rawList[i];
            nonCaptures++;
        }
    }

    for(int i = 0; i < captures; i++)
    {
        rawList[i] = captureList[i];
    }

    for(int i = 0; i < nonCaptures; i++)
    {
        rawList[i + captures] = nonCaptureList[i];
    }
}

double Engine::manhattenDistance(int x1, int y1, double x2, double y2)
{
    return fabs(x1 - x2) + fabs(y1 - y2);
}

void Engine::printMove(Move toPrint)
{
    std::cout << Engine::toAlg(toPrint.startX) << toPrint.startY
              << Engine::toAlg(toPrint.endX) << toPrint.endY << std::endl;
}

void Engine::updateBoard(Board newBoard)
{
    gameBoard = newBoard;
}

void Engine::updateTranspositionBestIfDeeper(Board newBoard, int depth, Move newMove)
{
    bool firstTime = this->transpositionTable.find(newBoard) == this->transpositionTable.end();
    if (firstTime || this->transpositionTable[newBoard].depth < depth)
    {
        this->transpositionTable[newBoard] = TranspositionCache(depth, newMove, true);
    }
}

void Engine::updateTranspositionCutoffIfDeeper(Board newBoard, int depth, Move newMove)
{
    bool firstTime = this->transpositionTable.find(newBoard) == this->transpositionTable.end();
    bool deeperAndAlreadyCutoff = this->transpositionTable[newBoard].depth < depth && !this->transpositionTable[newBoard].fullyEvaluated;
    if (firstTime || deeperAndAlreadyCutoff)
    {
        this->transpositionTable[newBoard] = TranspositionCache(depth, newMove, false);
    }
}

TranspositionCache Engine::getTransposition(Board lookupBoard)
{
    bool notFound = this->transpositionTable.find(lookupBoard) == this->transpositionTable.end();
    if (notFound)
    {
        return TranspositionCache();
    }
    else
    {
        return this->transpositionTable[lookupBoard];
    }
}
