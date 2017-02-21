#ifndef TRANSPOSITIONCACHE_H
#define TRANSPOSITIONCACHE_H
#include "Move.h"

struct TranspositionCache
{
    public:
        int depth;
        Move bestMove;

        TranspositionCache();
        TranspositionCache(int newDepth, Move newMove);
    protected:
    private:
};

#endif // TRANSPOSITIONCACHE_H
