//
// Created by ivanl on 21.02.2023.
//

#include "../Headers/Variable.h"
#include <stdexcept>

void Variable::setIndex(int index) {
    data.index = index;
}

void Variable::setSymbol(char symbol) {
    if ((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z')){
        data.symbol = symbol;
    } else
        throw std::runtime_error(std::string("Not valid symbol"));
}

char Variable::getSymbol() {
    return 0;
}

Variable::Variable(char symbol) {
    setSymbol(symbol);
}

Variable::Variable(char symbol, int index) {
    setSymbol(symbol);
    setIndex(index);
}

int Variable::getIndex() {
    return data.index;
}
