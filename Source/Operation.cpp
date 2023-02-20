//
// Created by Des Caldnd on 2/21/2023.
//

#include "../Headers/Operation.h"

const std::vector<Operation::proxy> Operation::ORDER_LIST =  {{'&', 4}, {'|', 3}, {'<', 2}, {'\\', 5}, {'+', 0}, {'~', 1}, {'-', 6}};

void Operation::setOrder() {
    int ord;
    for(int i = 0; i < ORDER_LIST.size(); i++){
        if (data.operation == ORDER_LIST[i].operation){
            data.order = ORDER_LIST[i].order;
            break;
        }
    }
}

void Operation::setOperation(char oper) {
    data.operation = oper;
    setOrder();
}

char Operation::getOperation() {
    return data.operation;
}

Operation::Operation(char oper) {
    setOperation(oper);
}
