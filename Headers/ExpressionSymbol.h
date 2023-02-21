//
// Created by ivanl on 21.02.2023.
//

#ifndef DISCRETEMATH_EXPRESSIONSYMBOL_H
#define DISCRETEMATH_EXPRESSIONSYMBOL_H


class ExpressionSymbol {

protected:
    struct proxy{
        char symbol;
        int index = -1;
    };

    virtual void setIndex(int) =0;

public:

    int position = 0;

    virtual void setSymbol(char) =0;

    virtual char getSymbol() = 0;
};


#endif //DISCRETEMATH_EXPRESSIONSYMBOL_H
