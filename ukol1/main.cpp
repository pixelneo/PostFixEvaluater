//
//  main.cpp
//  ukol1
//
//  Created by Ondřej Měkota on 03/11/2017.
//  Copyright © 2017 Ondřej Měkota. All rights reserved.
//

#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <exception>
#include <cstdint>


class AbstractOperation {
public:
    virtual ~AbstractOperation() {};
    virtual int_least32_t compute() = 0;
protected:
    int_least32_t compute_helper(){
        if(!left ^ !right){
            throw std::domain_error("Invalid input");
        }
        if(!left && !right){
            return value;
        }
        return 0;
    };
    std::unique_ptr<AbstractOperation> left;
    std::unique_ptr<AbstractOperation> right;
    int_least32_t value;
    AbstractOperation( std::unique_ptr<AbstractOperation> l,  std::unique_ptr<AbstractOperation> r): left {std::move(l)}, right {std::move(r)} {};
    
    AbstractOperation() {};
};

class Plus : public AbstractOperation {
public:
    Plus(std::unique_ptr<AbstractOperation> left, std::unique_ptr<AbstractOperation> right): AbstractOperation(std::move(left), std::move(right)) {};
    Plus(){};
    int_least32_t compute() override {
        int_least32_t r = AbstractOperation::compute_helper();
        if(r != 0)
            return r;
        else
            return left->compute() + right->compute();
    };
};

class Minus : public AbstractOperation {
public:
    Minus(std::unique_ptr<AbstractOperation> left, std::unique_ptr<AbstractOperation> right): AbstractOperation(std::move(left), std::move(right)) {};
    int_least32_t compute() override {
        int_least32_t r = AbstractOperation::compute_helper();
        if(r != 0)
            return r;
        else
            return left->compute() - right->compute();
    };
};

class Multiply : public AbstractOperation {
public:
    Multiply(std::unique_ptr<AbstractOperation> left, std::unique_ptr<AbstractOperation> right): AbstractOperation(std::move(left), std::move(right)) {};
    int_least32_t compute() override {
        int_least32_t r = AbstractOperation::compute_helper();
        if(r != 0)
            return r;
        else
            return left->compute() * right->compute();
    };
};

class Divide : public AbstractOperation {
public:
    Divide(std::unique_ptr<AbstractOperation> left, std::unique_ptr<AbstractOperation> right): AbstractOperation(std::move(left), std::move(right)) {};
    
    int_least32_t compute() override {
        int_least32_t r = AbstractOperation::compute_helper();
        if(r != 0)
            return r;
        else if(right->compute() != 0){
            return left->compute() / right->compute();
        }
        else {
            return INT_LEAST32_MAX;
        }
    };
};
class Number: public AbstractOperation {
public:
    Number(int_least32_t v): value{v} {};
    int_least32_t compute() override{
        return value;
    };
};

static bool isInt(const std::string& s){
    if(s.empty())
        return false;
    for (auto && c: s) {
        if(!isdigit(c))
            return false;
    }
    return true;
}

struct InputElement {
    int_least32_t value;
    char type;
};

int main(int argc, const char * argv[]) {
    std::vector<std::unique_ptr<AbstractOperation>> s;
    std::string op;
    while(std::cin >> op){
        if (isInt(op)) {
            s.push_back(std::make_unique<Number>(stoi(op)));
        }
        else if (op == "+"){
            s.push_back(std::make_unique<Plus>());
        }
        else if (op == "-"){
            s.push_back(std::make_unique<Minus>());
        }
        else if (op == "*"){
            s.push_back(std::make_unique<Multiply>());
        }
        else if (op == "/"){
            s.push_back(std::make_unique<Divide>());
        }
        else{
            std::cout << "Error";
            return 0;
        }
    }
    
    for(auto && i:s){
        
    }
    
    return 0;
}
