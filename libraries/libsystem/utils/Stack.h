template <typename T>
struct Stack {
    T value;
    struct Stack *next;
};

typedef struct Stack<char> stack;

template <typename T>
void push(stack **top, T value){
   
   stack *node = new stack();
   node->next = *top;
   *top = node;
   node->value = value;
}

stack* pop(stack **top){
    stack *temp =  *top;
    *top = temp->next;
    temp->next = nullptr;
    delete temp;
    return *top;
}

template <typename T>
T peek (stack *top){
    return top->value;
}

bool isEmpty(stack *top){
    return top == nullptr;
}
