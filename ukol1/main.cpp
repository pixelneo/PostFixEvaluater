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
#include <cctype>


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
    int_least32_t * x_ptr;
    AbstractOperation( std::unique_ptr<AbstractOperation> l,  std::unique_ptr<AbstractOperation> r): left {std::move(l)}, right {std::move(r)} {};
    AbstractOperation( int_least32_t val): value {val} {};
    AbstractOperation( int_least32_t * val_ptr): x_ptr {val_ptr} {};

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
    Number(int_least32_t v): AbstractOperation(v) {};
    int_least32_t compute() override{
        return value;
    };
};

class X_value: public AbstractOperation {
public:
    X_value(int_least32_t * x_ptr): AbstractOperation(x_ptr) {};
    int_least32_t compute() override {
        return * x_ptr;
    };
};

class PostFix {
public:
    PostFix(std::unique_ptr<std::stack<std::unique_ptr<AbstractOperation>>> s){
        head = std::move(s->top());
        s->pop();
        AbstractOperation * last = head.get();
        while(!s->empty()){
            last
        }
    };
    int_least32_t compute(int_least32_t value){
        
        return 0;
    };
private:
    int_least32_t x;
    std::unique_ptr<AbstractOperation> head;
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
    //unique_ptr because later it will be passed in function
    std::unique_ptr<std::stack<std::unique_ptr<AbstractOperation>>> s;
    std::string op;
    std::string input = "14 x 71 /+3000 x-75/*";
    std::string nr = "";
    
    //first the input is splitted into logical piecies without any relation.
    for (auto && c : input) {
        if(isdigit(c)){
            nr += c;
        }
        else {
            if(nr.length() > 0){
                s->push(std::make_unique<Number>(stoi(nr)));
                nr = "";
            }
            
            if (c == '+'){
                s->push(std::make_unique<Plus>());
            }
            else if (c == '-'){
                s->push(std::make_unique<Minus>());
            }
            else if (c == '*'){
                s->push(std::make_unique<Multiply>());
            }
            else if (c == '/'){
                s->push(std::make_unique<Divide>());
            }
            
        }
    }
    while(std::cin >> op){
        if (isInt(op)) {
            s.push(std::make_unique<Number>(stoi(op)));
        }
        else{
            std::cout << "Error";
            return 0;
        }
    }
    auto top = std::move(s->top());
    s.pop();
    auto * parent = top;
    
    while(!s.empty()){
        
    }
    
    return 0;
}
