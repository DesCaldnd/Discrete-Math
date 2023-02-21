//
// Created by ivanl on 21.02.2023.
//

#ifndef DISCRETEMATH_VARIABLE_H
#define DISCRETEMATH_VARIABLE_H

#include "ExpressionSymbol.h"

class Variable : public ExpressionSymbol{
    proxy data;


public:
    void setSymbol(char) override;

    void setIndex(int) override;

    int getIndex();

    char getSymbol() override;

    Variable(char);

    Variable(char, int);

    bool value;

};


#endif //DISCRETEMATH_VARIABLE_H