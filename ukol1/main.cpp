#include <iostream>
#include <fstream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <exception>
#include <cstdint>
#include <cctype>
#include <ios>
#include <tuple>

class AbstractOperation {
public:
    virtual ~AbstractOperation() {};
    virtual int_least32_t compute() const = 0;
    virtual bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) = 0;
};

class BinaryOperation: public AbstractOperation {
public:
    //this constructor is unused but it could be useful in another implementation
    BinaryOperation(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): left{std::move(l)}, right{std::move(l)}{};
    BinaryOperation() {
        reset();
    };
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
            return true;
        }
        else{
            goto end;
        }
    end:
        return false;
    };
protected:
    void reset() {
        left = nullptr;
        right = nullptr;
    }
    std::unique_ptr<AbstractOperation> left;
    std::unique_ptr<AbstractOperation> right;
};

class Add : public BinaryOperation {
public:
    Add(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): BinaryOperation(std::move(l), std::move(r)){};
    Add() {};
    int_least32_t compute() const override {
        return left->compute() + right->compute();
    };
};

class Subtract : public BinaryOperation {
public:
    Subtract(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): BinaryOperation(std::move(l), std::move(r)){};
    Subtract() {};
    int_least32_t compute() const override {
        return left->compute() - right->compute();
    };
};

class Multiply : public BinaryOperation {
public:
    Multiply(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): BinaryOperation(std::move(l), std::move(r)){};
    Multiply(){};
    int_least32_t compute() const override {
        return left->compute() * right->compute();
    };
};

class Divide : public BinaryOperation {
public:
    Divide(std::unique_ptr<AbstractOperation> l, std::unique_ptr<AbstractOperation> r): BinaryOperation(std::move(l), std::move(r)){};
    Divide(){};
    int_least32_t compute() const override {
        int_least32_t r = right->compute();
        if(right->compute() != 0)
            return left->compute() / r;
        else
            throw std::domain_error("Division by zero.");
    };
};

class Number : public AbstractOperation {
public:
    Number(int_least32_t v): value{v} {};
    bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) override {
        return true;
    };

    int_least32_t compute() const override{
        return value;
    };
private:
    int_least32_t value;
};

class X_value: public AbstractOperation {
public:
    X_value(const int_least32_t * const x_pointer): x_ptr {x_pointer}{};
    bool initiate(std::stack<std::unique_ptr<AbstractOperation>> & stack_ptr) override {
        return true;
    };
    
    int_least32_t compute() const override {
        return *x_ptr;
    };
private:
    const int_least32_t * const x_ptr;
};

class PostFix {
public:
    PostFix(){};
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
    
    const int_least32_t * const get_x_ptr() const{
        return &x;
    }
private:
    int_least32_t x;
    std::unique_ptr<AbstractOperation> head;
};

//splitting the expression into logical pieces
void analyzeExpression(PostFix & postfix, std::stack<std::unique_ptr<AbstractOperation>> & s, const std::string & input){
    std::string nr = "";
    for (auto && c : input) {
        if(isdigit(c)){
            nr += c;
        }
        else {
            if(nr.length() > 0){ //constructing integer from string - char by char.
                int_least32_t k = std::stoi(nr);
                auto number = std::make_unique<Number>(k);
                s.push(std::move(number));
                nr = "";
            }
            if (c == '+'){
                s.push(std::make_unique<Add>());
            }
            else if (c == '-'){
                s.push(std::make_unique<Subtract>());
            }
            else if (c == '*'){
                s.push(std::make_unique<Multiply>());
            }
            else if (c == '/'){
                s.push(std::make_unique<Divide>());
            }
            else if(c == 'x'){
                s.push(std::make_unique<X_value>(postfix.get_x_ptr()));
            }
            
        }
    }
    postfix.set_tree(s);
};

std::tuple<std::string, int_least32_t, int_least32_t> getInput(std::ofstream & err, const char * * argv,  int argc){
    //parsing arguments
    std::tuple<std::string, int_least32_t, int_least32_t> r;
    if(argc >= 4){
        std::ifstream in;
        in.open(argv[1]);
        if(!in.good()){
            throw std::invalid_argument("Could not open input file.");
        }
        std::getline(in,std::get<0>(r));
        in.close();
        
        int_least32_t helper;
        try {
            helper = std::stoi(argv[2]);
            std::get<2>(r) = std::stoi(argv[3]);
        } catch (const std::invalid_argument&) {
            throw std::invalid_argument("Invalid arguments.");
        }
        
        //sorting the arguments
        std::get<1>(r) = std::min(helper,std::get<2>(r));
        std::get<2>(r) = std::max(helper,std::get<2>(r));
    }
    else {
        throw std::invalid_argument("Not enough arguments.");
    }
    return r;
};

int main(int argc, const char * * argv) {

    std::stack<std::unique_ptr<AbstractOperation>> s;
    std::string op;
    std::string input = "DEADBEEF"; //string with the expression to evaluate
    int_least32_t start;
    int_least32_t end;
    
    auto processedInput = getInput(std::cerr, argv, argc);

    PostFix postfix;
    
    //construction of the tree
    try {
        analyzeExpression(postfix, s, input);
    }
    catch(std::exception & e){
        std::cerr << e.what();
        return 1;
    }
    
    
    int_least32_t min = INT_LEAST32_MAX;
    int_least32_t max = INT_LEAST32_MIN;
    
    int_least32_t current;
    bool computed = false;
    
    //evaluate expression for all x in the given interval
    for (int_least32_t x = start; x <= end; ++x) {
        try {
            current = postfix.compute(int_least32_t(x));
        } catch (std::domain_error & e) {
            std::cerr << e.what() << std::endl;
            continue;
        }
        min = min < current ? min : current;
        max = max > current ? max : current;
        
        computed = true;
    }
    if(computed){
        std::cout << "min=" << min << " max=" << max << std::endl;
    }
    
    return 0;
}
