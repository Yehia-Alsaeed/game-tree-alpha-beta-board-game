#ifndef STACK_H
#define STACK_H
#include <vector>
#include <stdexcept>
using namespace std;
template <typename T>
class Stack {
private:
vector<T> elements; 

public:
     
    void push(const T& element) {
        elements.push_back(element);
    }
    
    
    T pop() {
        if (isEmpty()) {
            throw std::runtime_error("Stack underflow");
        }
        T top = elements.back();
        elements.pop_back();
        return top;
    }
    
   
    T peek() const {
        if (isEmpty()) {
            throw std::runtime_error("Stack is empty");
        }
        return elements.back();
    }
    
    
    bool isEmpty() const {
        return elements.empty();
    }
    
   
    size_t size() const {
        return elements.size();
    }
    
    
    void clear() {
        elements.clear();
    }
};

#endif 
