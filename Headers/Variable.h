//
// Created by ivanl on 21.02.2023.
//

#ifndef DISCRETEMATH_VARIABLE_H
#define DISCRETEMATH_VARIABLE_H

#include "ExpressionSymbol.h"
#include <compare>

class Variable final : public ExpressionSymbol{
    Type type = ExpressionSymbol::Type::Var;

public:
    void setSymbol(char) override;

    void setIndex(int) override;

    int getIndex();

    char getSymbol() const override;

    Variable(char);

    Variable(char, int);

    ExpressionSymbol::Type getType() override;

	auto operator<=>(const Variable &other){
		return this->getSymbol() <=> other.getSymbol();
	}
};


#endif //DISCRETEMATH_VARIABLE_H
