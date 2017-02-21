#include "TranspositionCache.h"

TranspositionCache::TranspositionCache()
{
    this->depth = -1;
    this->bestMove = Move();
}

TranspositionCache::TranspositionCache(int newDepth, Move newMove)
{
    this->depth = newDepth;
    this->bestMove = newMove;
}
