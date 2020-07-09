int is_operand(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int is_alpha(char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    
    return (c >= 'A' && c <= 'Z');
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

float evaluate_postfix(stack *postfix)
{
    stack *operand_stack = create_stack(postfix->size);
    char operator;

    while (is_empty(postfix) == 0)
    {
        if ((postfix->stack_array[postfix->top]).class == '1')
        {
            push(operand_stack, pop(postfix));
        }
        else
        {
            operator = pop(postfix).element_data.operator;

            if (operator == '+')
            {
                push(operand_stack, create_float_element(pop(operand_stack).element_data.operand + pop(operand_stack).element_data.operand));
            }
            else if (operator == '-')
            {
                float operand1, operand2;
                operand1 = pop(operand_stack).element_data.operand;
                operand2 = pop(operand_stack).element_data.operand;
                push(operand_stack, create_float_element(operand2 - operand1));
            }
            else if (operator == '*')
                push(operand_stack, create_float_element(pop(operand_stack).element_data.operand * pop(operand_stack).element_data.operand));
            else if (operator == '/')
            {
                float operand1, operand2;
                operand1 = pop(operand_stack).element_data.operand;
                operand2 = pop(operand_stack).element_data.operand;
                push(operand_stack, create_float_element(operand2 / operand1));
            }
        }
    }

    if (operand_stack->top >= 0)
        return pop(operand_stack).element_data.operand;

    return 0.0;
}
