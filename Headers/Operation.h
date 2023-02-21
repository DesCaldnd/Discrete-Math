//
// Created by Des Caldnd on 2/21/2023.
//

#ifndef DISCRETEMATH_OPERATION_H
#define DISCRETEMATH_OPERATION_H

#include <vector>
#include "ExpressionSymbol.h"

class Operation : public ExpressionSymbol{
    proxy data;

    void setIndex(int) override;

    static const std::vector<proxy> ORDER_LIST;

public:
    void setSymbol(char) override;

    char getSymbol() override;

    static int order(char);

    int getOrder();

    Operation(char);

    Operation(char, int);
};


#endif //DISCRETEMATH_OPERATION_H
