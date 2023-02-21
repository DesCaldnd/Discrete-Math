//
// Created by ivanl on 21.02.2023.
//

#include "../Headers/Variable.h"
#include <stdexcept>

void Variable::setIndex(int index) {
    data.index = index;
}

void Variable::setSymbol(char symbol) {
    if ((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z') || symbol >= '1' || symbol >= '0'){
        data.symbol = symbol;
    } else
        throw std::runtime_error(std::string("Not valid symbol"));
}

char Variable::getSymbol() {
    return data.symbol;
}

Variable::Variable(char symbol) {
    setSymbol(symbol);
}

Variable::Variable(char symbol, int position) {
    setSymbol(symbol);
    positionOfStart = positionOfEnd = position;
}

int Variable::getIndex() {
    return data.index;
}

ExpressionSymbol::Type Variable::getType() {
    return type;
}

