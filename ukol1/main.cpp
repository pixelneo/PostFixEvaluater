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
    void setLeft(std::unique_ptr<AbstractOperation> v){
        left = std::move(v);
    }
    void setRight(std::unique_ptr<AbstractOperation> v){
        right = std::move(v);
    }
    
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
    AbstractOperation(int_least32_t v): value {v} {};
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
private:

};

class X_value: public AbstractOperation {
public:
    X_value(int_least32_t * x_pointer): x_ptr {x_pointer}, AbstractOperation() {};
    int_least32_t compute() override {
        return * x_ptr;
    };
private:
    int_least32_t * x_ptr;

};

class PostFix {
public:
    PostFix(){
        
    };
    PostFix(std::unique_ptr<std::stack<std::unique_ptr<AbstractOperation>>> s){
        set_tree(std::move(s));
    };
    //TODO nefunguje. kazde operaci dat ptr na stack, at si z toho vytahne co chce.
    void set_tree(std::unique_ptr<std::stack<std::unique_ptr<AbstractOperation>>> s){
        head = std::move(s->top());
        s->pop();
        AbstractOperation * last = head.get();
        while(!s->empty()){
            last->setRight(std::move(s->top()));
            s->pop(); //je to potreba? nesmaze se to unique_ptrem
            if(!s->empty()){
                last->setLeft(std::move(s->top()));
                s->pop();
            }
            
        }
    }
    
    int_least32_t compute(const int_least32_t value){
        x = value;
        return head->compute();
    };
    int_least32_t * get_x_ptr(){
        return &x;
    }
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
    int_least32_t start;
    int_least32_t end;
    
    //TODO osetrit to, kdyz vstup neni ints a to jestli jsou serazeny
    if(argc >= 4 || 0){
        int_least32_t h = atoi(argv[2]);
        end = atoi(argv[3]);
        
        //sorting the arguments
        start = std::min(h, end);
        end = std::max(h,end);
    }
  
    
    PostFix postfix;
    
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
            else if(c == 'x'){
                s->push(std::make_unique<X_value>(postfix.get_x_ptr()));
            }
            
        }
    }
    int_least32_t min = INT_LEAST32_MAX;
    int_least32_t max = INT_LEAST32_MIN;
    
    int_least32_t current;
    bool computed = false;
    for (size_t x = start; x <= end; x++) {
        current = postfix.compute(int_least32_t(x));
        min = min < current ? min : current;
        max = max > current ? max : current;
        computed = true;
    }
    if(computed){
        std::cout << "min=" << min << " max=" << max;
    }
    
    postfix.set_tree(std::move(s));
    
    
    return 0;
}
