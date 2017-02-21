#include "TranspositionCache.h"

TranspositionCache::TranspositionCache(int newDepth, int newScore, Move newMove)
{
    this->depth = newDepth;
    this->score = newScore;
    this->bestMove = newMove;
}
