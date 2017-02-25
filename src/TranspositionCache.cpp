#include "TranspositionCache.h"

TranspositionCache::TranspositionCache()
{
    this->depth = -1;
    this->bestMove = Move();
    this->fullyEvaluated = false;
}

TranspositionCache::TranspositionCache(int newDepth, Move newMove, bool fullyEvaluated)
{
    this->depth = newDepth;
    this->bestMove = newMove;
    this->fullyEvaluated = fullyEvaluated;
}
