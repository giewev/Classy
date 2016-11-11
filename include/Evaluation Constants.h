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

const double centerPawnValue = 0.309;
const double semiCenterPawnValue = 0.266;
const double rimKnightValue = -0.221;
const double developmentValue = 0.127;
const double closedRookValue = -0.237;
const double bishopSquareTableValue = -0.009;
const double kingCornerValue = 0.132;
const double isolatedPawnValue = -0.15;
const double passedPawnValue = 0.135;
const double passerPushValue = 0.1;
const double pawnChainValue = -0.034;
