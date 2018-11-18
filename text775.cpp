#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
 
#define MAX_INPUT_SIZE        (1 << 12)
#define MAX_STACK_SIZE        (MAX_INPUT_SIZE / 2 + 64)
 
enum {
    kET_Atom,
    kET_High,
    kET_Mid,
    kET_Low
};
 
typedef struct
{
    int        kExprType, kExprLen;
    char    kExprValue[2*MAX_INPUT_SIZE - 2*sizeof(int)];
} ST_expr;
 
int solve(const char* input_expr, ST_expr* stack)
{
    int top = 0, c;
    ST_expr *curr, *left, *right;
 
    while(0 != (c = *input_expr++))
    {
        assert(top >= 0);
 
        if(top >= MAX_STACK_SIZE - 1)
            return -2;
 
        switch(c)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case '\f':
            /* ignore space */
            break;
 
        case '+':
        case '-':
            if(top < 2)    return -1;
            left  = curr = &stack[top - 2];
            right = &stack[top - 1];
            --top;
 
            curr->kExprValue[curr->kExprLen++] = c;
            if(right->kExprType == kET_Low)
            {
                curr->kExprValue[curr->kExprLen++] = '(';
                memcpy(curr->kExprValue + curr->kExprLen, right->kExprValue, right->kExprLen);
                curr->kExprLen += right->kExprLen;
                curr->kExprValue[curr->kExprLen++] = ')';
            }
            else
            {
                memcpy(curr->kExprValue + curr->kExprLen, right->kExprValue, right->kExprLen);
                curr->kExprLen += right->kExprLen;
            }
            curr->kExprValue[curr->kExprLen] = 0;
            curr->kExprType = kET_Low;
            break;
 
        case '*':
        case '/':
            if(top < 2)    return -1;
            left  = curr = &stack[top - 2];
            right = &stack[top - 1];
            --top;
 
            if(curr->kExprType == kET_Low)
            {
                memmove(curr->kExprValue + 1, curr->kExprValue, curr->kExprLen);
                curr->kExprValue[0] = '(';
                curr->kExprValue[1 + curr->kExprLen] = ')';
                curr->kExprLen += 2;
            }
 
            curr->kExprValue[curr->kExprLen++] = c;
            if(right->kExprType >= kET_Mid)
            {
                curr->kExprValue[curr->kExprLen++] = '(';
                memcpy(curr->kExprValue + curr->kExprLen, right->kExprValue, right->kExprLen);
                curr->kExprLen += right->kExprLen;
                curr->kExprValue[curr->kExprLen++] = ')';
            }
            else
            {
                memcpy(curr->kExprValue + curr->kExprLen, right->kExprValue, right->kExprLen);
                curr->kExprLen += right->kExprLen;
            }
            curr->kExprValue[curr->kExprLen] = 0;
            curr->kExprType = kET_Mid;
            break;
 
        case '!':
            if(top < 1)    return -1;
            curr = &stack[top - 1];
 
            if(curr->kExprType >= kET_Mid)
            {
                memmove(curr->kExprValue + 1, curr->kExprValue, curr->kExprLen);
                curr->kExprValue[0] = '(';
                curr->kExprValue[1 + curr->kExprLen] = ')';
                curr->kExprValue[2 + curr->kExprLen] = '!';
                curr->kExprLen += 3;
            }
            else
            {
                curr->kExprValue[curr->kExprLen++] = '!';
            }
 
            curr->kExprValue[curr->kExprLen]   = 0;
            curr->kExprType = kET_High;
            break;
 
        default:
            stack[top].kExprLen   = 1;
            stack[top].kExprType  = kET_Atom;
            stack[top].kExprValue[0] = c;
            stack[top].kExprValue[1] = 0;
            ++top;
            break;
        }
    }
 
    return top;
}
 
int main()
{
    int err = 0, r;
    char*        input_expr;
    ST_expr*    stack;
 
    input_expr = (char*)calloc(1, MAX_INPUT_SIZE + 16);
    stack       = (ST_expr*)malloc(sizeof(ST_expr)*MAX_STACK_SIZE);
 
    while(NULL != fgets(input_expr, MAX_INPUT_SIZE, stdin))
    {
        r = solve(input_expr, stack);
        switch(r)
        {
        case 0:
            /* empty line */
            break;
 
        case 1:
            printf("%s\n", stack[0].kExprValue);
            break;
 
        default:
            fprintf(stderr, "%d, ERROR(%d): %s\n", ++err, r, input_expr);
        }
    }
 
    free(input_expr);
    free(stack);
 
    return err;
}