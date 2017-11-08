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


int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
