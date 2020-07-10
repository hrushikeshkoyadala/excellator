
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.c"
#include "structs.h"
#include "exp_eval.c"

/*
    moves file pointer to required cell position
    returns NULL on failure
*/
FILE* csv_seek(FILE *csv, cell_position target)
{
    fseek(csv, 0, SEEK_SET);

    cell_position current_position = {1, 'A'};
    char current_char;

    while (feof(csv) == 0)
    {
        if (current_position.column == target.column && target.row == current_position.row)
            return csv;
        
        current_char = (char)fgetc(csv);

        if (current_char == ',')
            current_position.column++;
        else if (current_char == '\n')
        {
            current_position.column = 'A';
            current_position.row++;
        }
    }

    return NULL;
}

/*
    retrieves cell value from results file
*/
float get_cell_value_from_file(cell_position required, FILE *result)
{
    cell_result current_cell;
    int no_of_cells;

    fseek(result, 0, SEEK_SET);

    fread(&no_of_cells, sizeof(int), 1, result);

    for (int i = 0; i < no_of_cells; i++)
    {

        fread(&current_cell, sizeof(cell_result), 1, result);
        if (current_cell.position.column == required.column && current_cell.position.row == required.row)
            return current_cell.value;
    }

    return 0.0;
}


char* read_element(FILE *csv)
{
    char *element = (char *)malloc(sizeof(char) * 20);
    char curr_element;

    for (int i = 0; i < 20 && feof(csv) == 0; i++)
    {
        curr_element = (char)fgetc(csv);
        if (curr_element == '\n' || curr_element == ',')
        {
            element[i] = '\0';
            break;
        }
        else
            element[i] = curr_element;
    }

    return element;
}
void display_csv(FILE *csv)
{
    fseek(csv, 0, SEEK_SET);

    char current_char;

    while (feof(csv) == 0)
    {
        current_char = (char)fgetc(csv);
        if (current_char == '\0')
            printf("null");
        else
        printf("%c", current_char);
    }
}

/*
    evaluates expression at position
    and other expressions it refers to,
    stores results in the FILE result

    returns expression result
*/
float evaluate_expression(FILE *csv, cell_position position, FILE *result)
{
    csv = csv_seek(csv, position);
    int file_pointer = ftell(csv);
    /*
        expression was already evaluated and 
        stored in result FILE
    */
    if ((char)fgetc(csv) == '!')
    {
        printf("Already there\n");
        return get_cell_value_from_file(position, result);
    }
    else
    {
        /*
            push number to stack if '1'
        */
        char add_number;
        float number;
        int multiplier;
        int flag = 0;
        fseek(csv, file_pointer, SEEK_SET);
        char *expression = read_element(csv);

        stack *operator_stack = create_stack(strlen(expression));
        stack *postfix = create_stack(strlen(expression));


        while (*expression)
        {
            add_number = '0';
            number = 0.0;

            if (is_alpha(*expression))
            {
                flag = 1;
                cell_position req_cell;
                req_cell.column = *expression;
                req_cell.row = 0;
                expression++;

                while (is_num(*expression))
                {
                    req_cell.row = (req_cell.row)*10 + (int)(*expression - 48);
                    expression++;
                }
                push(postfix, create_float_element(evaluate_expression(csv, req_cell, result)));
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
                    return 0.0;
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
                flag = 1;
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
                    return 0.0;
                }

                while (peek(operator_stack).element_data.operator != '(')
                {
                    push(postfix, pop(operator_stack));

                    if (operator_stack->top == -1) {
                        printf("Invalid");
                        return 0.0;
                    }
                }
                pop(operator_stack);
            }
            expression++;
        }

        while (operator_stack->top != -1 && peek(operator_stack).element_data.operator != '(')
            push(postfix, pop(operator_stack));

        reverse_stack(postfix);
        number = evaluate_postfix(postfix);

        //add result to result FILE
        if (flag)
        {
            fseek(csv, file_pointer, SEEK_SET);
            fprintf(csv, "!");

            int no_of_results;
            fseek(result, 0, SEEK_SET);
            fread(&no_of_results, sizeof(int), 1, result);

            cell_result to_write = {number, position};
            fseek(result, sizeof(int) + no_of_results*sizeof(cell_result), SEEK_SET);
            fwrite(&to_write, sizeof(cell_result), 1, result);

            fseek(result, sizeof(int) + no_of_results*sizeof(cell_result), SEEK_SET);
            fread(&to_write, sizeof(cell_result), 1, result);

            //update number of results in file
            no_of_results++;
            fseek(result, 0, SEEK_SET);
            fwrite(&no_of_results, sizeof(int), 1, result);
        }
        return number;
    }
    return 0.0;
}

/*
    returns a file containing results
    of expressions in csv file

    returns NULL on failure
*/
FILE* parse_csv(FILE *csv)
{
    if (csv == NULL)
        return NULL;

    fseek(csv, 0, SEEK_SET);
    //empty file
    if (feof(csv))
        return NULL;


    //file containing expression results
    FILE *result = fopen("result", "w+");
    int write_zero = 0;
    fseek(result, 0, SEEK_SET);
    fwrite(&write_zero, sizeof(int), 1, result);

    if (result == NULL)
        return NULL;
    
    char current_char;
    cell_position current_position = {1, 'A'};

    while (feof(csv) == 0)
    {
        current_char = (char)fgetc(csv);

        if (current_char == ',')
            current_position.column++;
        else if (current_char == '\n')
        {
            current_position.column = 'A';
            current_position.row++;
            printf("%lu\n", ftell(csv));
        }
        else if (current_char == ':')
        {
            while (current_char != '\n' && current_char != ',')
            {
                if (feof(csv))
                    return result;

                current_char = (char)fgetc(csv);
            }
        }
        //an expression
        else if (is_alpha(current_char))
        {
            int fp = ftell(csv);
            evaluate_expression(csv, current_position, result);
            fseek(csv, fp, SEEK_SET);
            while (current_char != '\n' && current_char != ',')
                if (feof(csv))
                    break;
                else
                    current_char = (char)fgetc(csv);
            
            fseek(csv, -1, SEEK_CUR);
        }
    }

    return result;
}

void display_results(FILE *result)
{
    int no_of_result;
    fseek(result, 0, SEEK_SET);
    fread(&no_of_result, sizeof(int), 1, result);

    cell_result current_result;
    for (int i = 0; i < no_of_result; i++)
    {
        fread(&current_result, sizeof(cell_result), 1, result);
        printf("row: %d column: %c value: %f\n", current_result.position.row, current_result.position.column, current_result.value);
    }
}

int main()
{
    FILE *csv = fopen("/Users/hrushi/Desktop/Programs/excellator/data/numbers.csv", "r+");
    //while (feof(csv) == 0)
      //  fgetc(csv);
    display_results(parse_csv(csv));
    return 0;
}