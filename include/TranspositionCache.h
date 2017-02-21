#ifndef TRANSPOSITIONCACHE_H
#define TRANSPOSITIONCACHE_H
#include "Move.h"

struct TranspositionCache
{
    public:
        int depth;
        int score;
        Move bestMove;

        TranspositionCache(int newDepth, int newScore, Move newMove);
    protected:
    private:
};

#endif // TRANSPOSITIONCACHE_H
