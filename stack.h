typedef union data
{
    float operand;
    char operator;
} data;

typedef struct element
{
    char class;
    data element_data;
} element;

typedef struct stack
{
    element *stack_array;
    int top;
    int size;
} stack;