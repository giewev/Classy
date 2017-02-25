#ifndef TRANSPOSITIONCACHE_H
#define TRANSPOSITIONCACHE_H
#include "Move.h"

struct TranspositionCache
{
    public:
        int depth;
        Move bestMove;
        bool fullyEvaluated;

        TranspositionCache();
        TranspositionCache(int newDepth, Move newMove, bool fullyEvaluated);
    protected:
    private:
};

#endif // TRANSPOSITIONCACHE_H
