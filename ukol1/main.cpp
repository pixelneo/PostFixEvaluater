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
    //takes reference
    virtual bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) = 0;
    virtual void setLeft(std::unique_ptr<AbstractOperation> v) = 0;
    virtual void setRight(std::unique_ptr<AbstractOperation> v) = 0;
    virtual int_least32_t getValue() = 0;
    virtual void setValue(int_least32_t v) = 0;
    
protected:
    virtual void reset() = 0;
    virtual int_least32_t compute_helper() = 0;
    AbstractOperation() {
    };
};
class Operation: public AbstractOperation {
public:
    void setLeft(std::unique_ptr<AbstractOperation> v) override {
        left = std::move(v);
    }
    void setRight(std::unique_ptr<AbstractOperation> v) override{
        right = std::move(v);
    }
    int_least32_t getValue() override{
        return value;
    }
    void setValue(int_least32_t v) override{
        value = v;
    }
protected:
    Operation(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): left{std::move(l)}, right{std::move(r)} {};
    Operation(int_least32_t v): value{v} {
        reset();
    };
    Operation(){
        reset();
        
    };
    int_least32_t compute_helper() override{
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
private:
    void reset() override{
        left = nullptr;
        right = nullptr;
    }

};



class Plus : public Operation {
public:
    Plus(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): Operation(std::move(l), std::move(r)){};
    bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) override {
        if(stack_ptr.size() >= 2){
            right = std::move(stack_ptr.top());
            stack_ptr.pop();
            if(!right->initiate(stack_ptr))
                goto end;
            left = std::move(stack_ptr.top());
            stack_ptr.pop();
            if(!left->initiate(stack_ptr))
                goto end;
            
        }
        return true;
    end:
        return false;
    };

    Plus(){};
    int_least32_t compute() override {
        int_least32_t r = compute_helper();
        if(r != 0)
            return r;
        else
            return left->compute() + right->compute();
    };
};

class Minus : public Operation {
public:
    Minus(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): Operation(std::move(l), std::move(r)){};
    bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) override {
        if(stack_ptr.size() >= 2){
            right = std::move(stack_ptr.top());
            stack_ptr.pop();
            if(!right->initiate(stack_ptr))
                goto end;
            left = std::move(stack_ptr.top());
            stack_ptr.pop();
            if(!left->initiate(stack_ptr))
                goto end;
        }
        return true;
    end:
        return false;
    };
    Minus (){};

    int_least32_t compute() override {
        int_least32_t r = compute_helper();
        if(r != 0)
            return r;
        else
            return left->compute() - right->compute();
    };
};

class Multiply : public Operation {
public:
    Multiply(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): Operation(std::move(l), std::move(r)){};
    bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) override {
        if(stack_ptr.size() >= 2){
            right = std::move(stack_ptr.top());
            stack_ptr.pop();
            if(!right->initiate(stack_ptr))
                goto end;
            left = std::move(stack_ptr.top());
            stack_ptr.pop();
            if(!left->initiate(stack_ptr))
                goto end;
        }
        return true;
    end:
        return false;
    };
    Multiply(){};
    int_least32_t compute() override {
        int_least32_t r = compute_helper();
        if(r != 0)
            return r;
        else
            return left->compute() * right->compute();
    };
};

class Divide : public Operation {
public:
    Divide(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): Operation(std::move(l), std::move(r)){};
    bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) override {
        if(stack_ptr.size() >= 2){
            right = std::move(stack_ptr.top());
            stack_ptr.pop();
            if(!right->initiate(stack_ptr))
                goto end;
            left = std::move(stack_ptr.top());
            stack_ptr.pop();
            if(!left->initiate(stack_ptr))
                goto end;
        }
        return true;
    end:
        return false;
    };
    Divide(){};
    int_least32_t compute() override {
        int_least32_t r = compute_helper();
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
class Number: public Operation {
public:
    Number(int_least32_t v){
        setValue(v);
    };
    
    //use T & - reference (object will disappear after end of function
    bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) override {
        return true;
    };

    int_least32_t compute() override{
        return value;
    };
    ~Number() {};
private:

};

class X_value: public Operation {
public:
    X_value(const int_least32_t * const x_pointer): x_ptr {x_pointer}, Operation() {};
    bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) override {
        return true;
    };
    int_least32_t compute() override {
        return *x_ptr;
    };
private:
    const int_least32_t * const x_ptr;

};

class PostFix {
public:
    PostFix(){
        
    };
    PostFix(std::stack<std::unique_ptr<AbstractOperation>> & s){
        set_tree(s);
    };
    void set_tree(std::stack<std::unique_ptr<AbstractOperation>> & s){
        head = std::move(s.top());
        s.pop();
        head->initiate(s);
    }

    int_least32_t compute(int_least32_t value){
        x = value;
        return head->compute();
    };
    
    //todo: return reference instead of pointer
    const int_least32_t * const get_x_ptr(){
        return &x;
    }
private:
    int_least32_t x;
    std::unique_ptr<AbstractOperation> head;
};
void cou(const char & c){
    std::cout << c << std::endl;
};
void cou(const std::string & c){
    std::cout << c << std::endl;
};

int main(int argc, const char * argv[]) {
    //unique_ptr because later it will be passed in function
    std::stack<std::unique_ptr<AbstractOperation>> s;
    std::string op;
    std::string input = "14 x 71 /+3000 x-75/*";
    std::string nr = "";
    int_least32_t start = -3;
    int_least32_t end = 10;
    
    //TODO osetrit to, kdyz vstup neni ints a to jestli jsou serazeny
    if(argc >= 4 || 1){
        int_least32_t h = atoi(argv[2]);
        end = atoi(argv[3]);
        
        //sorting the arguments
        start = std::min(h, end);
        end = std::max(h,end);
    }
    else {
        return 1;
    }
    start = 1;
    end = 2;

    
    PostFix postfix;
    
    //first the input is splitted into logical piecies without any relation.
    for (auto && c : input) {
        if(isdigit(c)){
            nr += c;
        }
        else {
            if(nr.length() > 0){
                int_least32_t k = stoi(nr);
                auto number = std::make_unique<Number>(k);
                s.push(std::move(number));
                cou(nr);
                nr = "";
            }
            if (c == '+'){
                s.push(std::make_unique<Plus>());cou(c);
            }
            else if (c == '-'){
                s.push(std::make_unique<Minus>());cou(c);
            }
            else if (c == '*'){
                s.push(std::make_unique<Multiply>());cou(c);
            }
            else if (c == '/'){
                s.push(std::make_unique<Divide>());cou(c);
            }
            else if(c == 'x'){
                s.push(std::make_unique<X_value>(postfix.get_x_ptr()));cou(c);
            }
            
        }
    }
    postfix.set_tree(s);
    
    int_least32_t min = INT_LEAST32_MAX;
    int_least32_t max = INT_LEAST32_MIN;
    
    int_least32_t current;
    bool computed = false;
    for (int_least32_t x = start; x <= end; ++x) {
        current = postfix.compute(int_least32_t(x));
        //std::cout << current << ", ";
        min = min < current ? min : current;
        max = max > current ? max : current;
        computed = true;
    }
    if(computed){
        std::cout << "min=" << min << " max=" << max;
    }
    
    
    
    return 0;
}
