#ifndef POSITIONALEVALUATOR_H
#define POSITIONALEVALUATOR_H

#include <Evaluator.h>

class PositionalEvaluator : public Evaluator
{
    public:
        PositionalEvaluator();
        virtual ~PositionalEvaluator();
        double evaluate(Board);
    protected:
    private:
};

#endif // POSITIONALEVALUATOR_H
