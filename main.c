
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.c"
#include "structs.h"

int is_alpha(char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    
    return (c >= 'A' && c <= 'Z');
}

int is_operand(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int is_num(char c)
{
    return (c >= '0' && c <= '9');
}

float to_float(char c)
{
    return (float)c - 48;
}

int ge_precedence(char op1, char op2)
{
    if (op2 == '+' || op2 == '-')
        return 1;

    if (op1 == '*' || op1 == '/')
        return 1;
    
    return 0;
}

/*
    converts expression and
    returns postfix in a stack
*/
stack* to_postfix(char *expression)
{
    /*
        push number to stack if '1'
    */
    char add_number;
    float number;
    int multiplier;

    stack *operator_stack = create_stack(strlen(expression));
    stack *postfix = create_stack(strlen(expression));


    while (*expression)
    {
        add_number = '0';
        number = 0.0;

        if (is_alpha(*expression))
        {
            cell_position position;
            position.column = *expression;
            expression++;

            if (is_num(*expression) == 0)
            {
                printf("invalid\n");
                return NULL;
            }

            while (is_num(*expression))
            {
                number += number*10 + to_float(*expression);
                expression++;
            }

            position.row = number;
            //push(postfix, create_float_element(get_cell_value(position)));
            push(postfix, create_char_element('.'));
            continue;
        }
        if (is_num(*expression))
        {
            while (is_num(*expression))
            {
                number = number*10 + to_float(*expression);
                expression++;
            }
            add_number = '1';
        }
        if (*expression == '.')
        {
            multiplier = 10;
            expression++;

            if (is_num(*expression) == 0)
            {
                printf("Invalid");
                return NULL;
            }

            while (is_num(*expression))
            {
                number += to_float(*expression)/multiplier;
                multiplier *= 10;
                expression++;
            }

            add_number = '1';
        }

        if (add_number == '1')
        {
            push(postfix, create_float_element(number));
            continue;
        }

        if (*expression == '(')
        {
            push(operator_stack, create_char_element('('));
        }
        else if (is_operand(*expression))
        {
            if (operator_stack->top == -1) {
                push(operator_stack, create_char_element(*expression));
                expression++;
                continue;
            }

            char top = peek(operator_stack).element_data.operator;

            while (is_empty(operator_stack) == 0 && is_operand(top) && ge_precedence(top, *expression))
            {
                push(postfix, pop(operator_stack));

                if (is_empty(operator_stack))
                    break;

                top = peek(operator_stack).element_data.operator;
            }
            push(operator_stack, create_char_element(*expression));
        }
        else if (*expression == ')')
        {
            if (operator_stack->top == -1)
            {
                printf("Invalid");
                return NULL;
            }

            while (peek(operator_stack).element_data.operator != '(')
            {
                push(postfix, pop(operator_stack));

                if (operator_stack->top == -1) {
                    printf("Invalid");
                    return NULL;
                }
            }
            pop(operator_stack);
        }
        expression++;
    }

    while (operator_stack->top != -1 && peek(operator_stack).element_data.operator != '(')
        push(postfix, pop(operator_stack));

    reverse_stack(postfix);

    return postfix;
}


int main()
{
    display_stack(to_postfix("A23+(a4*c9) + C1*4"));
    return 0;
}