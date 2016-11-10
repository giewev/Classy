#ifndef MATERIALEVALUATOR_H
#define MATERIALEVALUATOR_H

#include <Evaluator.h>

class MaterialEvaluator : public Evaluator
{
    public:
        MaterialEvaluator();
        virtual ~MaterialEvaluator();
        double evaluate(Board);
    protected:
    private:
};

#endif // MATERIALEVALUATOR_H
