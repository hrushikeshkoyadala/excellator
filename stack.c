#include "stack.h"

stack *create_stack(int size)
{
    stack *new_stack = (stack *)malloc(sizeof(stack));

    new_stack->size = size;
    new_stack->top = -1;
    new_stack->stack_array = (element *)malloc(sizeof(element) * size);

    return new_stack;
}

int is_empty(stack *st)
{
    return (st->top < 0);
}

element create_char_element(char c)
{
    element new;

    new.class = '0';
    new.element_data.operator = c;

    return new;
}

element create_float_element(float num)
{
    element new;

    new.class = '1';
    new.element_data.operand = num;

    return new;
}

void push(stack *st, element to_add)
{
    if (st->top >= st->size - 1)
    {
        printf("stack overflow\n");
        return;
    }
    
    st->stack_array[++st->top] = to_add;
}

element pop(stack *st)
{
    if (is_empty(st))
    {
        printf("stack empty\n");
        return create_char_element('\0');
    }

    return st->stack_array[st->top--];
}

element peek(stack *st)
{
    if (is_empty(st))
    {
        printf("stack empty\n");
        return create_char_element('\0');
    }

    return st->stack_array[st->top];
}

/*
    from top to bottom
*/
void display_stack(stack *st)
{
    if (st == NULL)
        return;

    if (st->top < 0)
        return;

    for (int i = st->top; i >= 0; i--)
    {
        //display float
        if ((st->stack_array[i]).class == '1')
            printf("%f ", st->stack_array[i].element_data.operand);
        //display char
        else
            printf("%c ", st->stack_array[i].element_data.operator);
    }
}

void reverse_stack(stack *st)
{
    if (st->top < 2)
        return;

    element swap;
    for (int i = 0; i < st->top/2; i++)
    {
        swap = st->stack_array[i];
        st->stack_array[i] = st->stack_array[st->top - i];
        st->stack_array[st->top - i] = swap;
    }

    if (st->top % 2)
    {
        swap = st->stack_array[st->top/2];
        st->stack_array[st->top/2] = st->stack_array[st->top/2 + 1];
        st->stack_array[st->top/2 + 1] = swap;
    }
}