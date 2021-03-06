#include <inttypes.h>
#define bitBoard uint64_t
#define EVAL_CONST_COUNT 18

const double pawnValue = 1;
const double knightValue = 3;
const double bishopValue = 3;
const double rookValue = 5;
const double queenValue = 9;
const double knightBishopWeight = 0.1;
const double doubledPawnValue = -0.15;

const double centerPawnValue = 0.3;
const double semiCenterPawnValue = 0.2;
const double rimKnightValue = -0.25;
const double developmentValue = 0.15;
const double kingCornerValue = 0.1;
