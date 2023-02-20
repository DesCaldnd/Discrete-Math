//
// Created by Des Caldnd on 2/21/2023.
//

#ifndef DISCRETEMATH_OPERATION_H
#define DISCRETEMATH_OPERATION_H

#include <vector>

class Operation {

    struct proxy{
        char operation;
        int order;
    };
    proxy data;

    void setOrder();

    static const std::vector<proxy> ORDER_LIST;

public:
    void setOperation(char);

    char getOperation();

    Operation(char);
};


#endif //DISCRETEMATH_OPERATION_H
