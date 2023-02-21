//
// Created by Des Caldnd on 2/21/2023.
//

#include "../Headers/Operation.h"
#include <stdexcept>

const std::vector<Operation::proxy> Operation::ORDER_LIST =  {{'&', 4}, {'|', 3}, {'<', 2}, {'/', 5}, {'+', 0}, {'~', 1}, {'-', 6}, {'(', -1}, {')', -1},};

void Operation::setIndex(int index = 0) {
    data.index = order(data.symbol);
}

void Operation::setSymbol(char oper) {
    if (oper == '&' || oper == '|' || oper == '<' || oper == '/' || oper == '+' || oper == '~' || oper == '-' || oper == '(' || oper == ')'){
        data.symbol = oper;
        setIndex();
    } else
        throw std::runtime_error(std::string("Not valid symbol"));

}

char Operation::getSymbol() {
    return data.symbol;
}

Operation::Operation(char oper) {
    setSymbol(oper);
}

int Operation::order(char oper) {
    for (int i = 0; i < ORDER_LIST.size(); i++){
        if(ORDER_LIST[i].symbol == oper)
            return ORDER_LIST[i].index;
    }
    return -2;
}

int Operation::getOrder() {
    return data.index;
}

Operation::Operation(char sym, int pos) {
    setSymbol(sym);
    positionOfStart = positionOfEnd = pos;
}

ExpressionSymbol::Type Operation::getType() {
    return type;
}
