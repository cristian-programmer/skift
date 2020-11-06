#pragma once

template <class T>
class Stack {
    
    T value;
    Stack *next;
    
    public:
        bool isEmpty(Stack *top){
            return top == nullptr;
        }
        
        void push(Stack **top, T value){
            Stack *node = new Stack();
            node->next = *top;
            *top = node;
            node->value = value;
        }
        
        T peek(Stack *top){
             return !isEmpty(top) ? top->value : 0;
        }
        
        Stack *pop(Stack **top){
            Stack *temp =  *top;
            *top = temp->next;
            temp->next = nullptr;
            delete temp;
            return *top;
        }
};
