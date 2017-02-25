#ifndef TRANSPOSITIONCACHE_H
#define TRANSPOSITIONCACHE_H
#include "Move.h"

struct TranspositionCache
{
    public:
        int bestDepth;
        Move bestMove;

        int cutoffDepth;
        Move cutoffMove;

        TranspositionCache();
        TranspositionCache(int bestDepth, Move newMove, int cutoffDepth, Move cutoffMove);
    protected:
    private:
};

#endif // TRANSPOSITIONCACHE_H
