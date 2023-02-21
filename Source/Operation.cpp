//
// Created by Des Caldnd on 2/21/2023.
//

#include "../Headers/Operation.h"
#include <stdexcept>

const std::vector<Operation::proxy> Operation::ORDER_LIST =  {{'&', 4}, {'|', 3}, {'<', 2}, {'\\', 5}, {'+', 0}, {'~', 1}, {'-', 6}, {'(', -1}, {')', -1},};

void Operation::setIndex(int index = 0) {
    int ord;
    for(int i = 0; i < ORDER_LIST.size(); i++){
        if (data.symbol == ORDER_LIST[i].symbol){
            data.index = ORDER_LIST[i].index;
            break;
        }
    }
}

void Operation::setSymbol(char oper) {
    if (oper == '&' || oper == '|' || oper == '<' || oper == '\\' || oper == '+' || oper == '~' || oper == '-' || oper == '(' || oper == ')'){
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
