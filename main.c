/* *** COPYRIGHT SLYAPA26 INC. 2024 ***
 * *** NOT FOR COPYING
 * *** NOT FOR DISTRIBUTION
 * *** ONLY FOR SLYAPA USE
 * */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
//#include "variable_table.h"

#define DELIM 1
#define VAR 2
#define NUM 3
#define COM 4
#define STR 5
#define QUOT 6
#define FINISH 10
#define EOL 9
#define INCR 11
#define LABL 12

 char token[80];
 int tok, token_type;
 char *prog;
enum {PRINT = 1, INPUT, IF, THEN, FOR, NEXT, TO, GOTO, GOSUB = 11, RETURN, END};
struct {
    char command[21];
    int tok;
} table[] = {
        "print", PRINT,
        "input", INPUT,
        "if", IF,
        "then", THEN,
        "goto", GOTO,
        "for", FOR,
        "next", NEXT,
        "to", TO,
        "gosub", GOSUB,
        "return", RETURN,
        "end", END,
        0, END
};
//hash_t *vartable;
HT(char *, int) vartable;

int internal_format(char *s);
int get_token(void);
int find_var(char *name);
char *load_program(char *filename);
long int filesize( FILE *fp);
void arith(char op, int *res, int *hold);
void calc_expression(int *result);
void term_sum(int *result);
void factor_mul(int *result);
void power(int *result);
void unary(int *result);
void braces_expr(int *result);
void inc_dec(int *result);
void get_primitive(int *result);
void putback(void);
void assign();


void putback()
{
    for (char *t = token; *t; t++)
        prog--;
}

int internal_format(char *s)
{
    char *p = s;
    while (*p) {
        *p = tolower(*p); p++;
    }
    for (int i = 0; *table[i].command; ++i) {
        if (strcmp(table[i].command, s) == 0)
              table[i].tok;
    }
    return 0;
}

int get_token(void)
{
    register char *tmp;
    token_type = 0;
    tok = 0;
    tmp = token;
    if (*prog == '\0') {
        *token = 0;
        tok = FINISH;
        return (token_type = DELIM);
    }
    while (isspace(*prog))
        ++prog;
    if (*prog == '\r'){
        prog += 2;
        tok = EOL;
        token[0] = '\r'; token[1] = '\n'; token[2] = '\0';
        return (token_type = DELIM);
    }
    if (strchr("+-*/%^=,;<>()", *prog)) {
        *tmp = *prog;
        prog++; tmp++;
        if (((*(tmp-1) == '-' || *(tmp-1) == '+') && strchr("+-", *prog)) || (*prog == '=')) {
            *tmp = *prog;
            prog++; tmp++;
        }
        *tmp = 0;
        if (strcmp("--", token) == 0 || strcmp("++", token) == 0) {
            tok = INCR;
        }
        return (token_type = DELIM);
    }
    if (*prog == '"') {
        prog++;
        while (*prog != '"' && *prog != '\r')
            *tmp++ = * prog++;
        if (*prog == '\r')
            perror("syntax error");
        prog++; *tmp = 0;
        return (token_type = QUOT);
    }
    if (isdigit(*prog)) {
        while (isdigit(*prog))
            *tmp++ = *prog++;
        *tmp = '\0';
        return (token_type = NUM);
    }
    if (isalpha(*prog)) {
        while(isalnum(*prog) || *prog == ':')
            *tmp++ = *prog++;

        token_type = STR;
    }
    *tmp = '\0';
    if (token_type == STR) {
        tok = internal_format(token);
        if (!tok) {
            if (*(tmp-1) == ':')
                token_type = LABL;
            else
                token_type = VAR;
        }

        else
            token_type = COM;
    }
    return token_type;
}

int find_var(char *name)
{
    if(!isalpha(*name)) {
        perror("wrong var name");
        exit(1);
    }
    int res_ind;
    // TOD0 DONE
    /*if (ht_get(vartable, name, &tmp) == 0)
        return tmp;*/
    ht_get(vartable, name, res_ind, hash_f, str_eq);
    if (res_ind != 0)
        return ht_val(vartable, res_ind);
    else {
        perror("No such variable");
        exit(1);
    }

}

void arith(char op, int *res, int *hold)
{
    int tmp;
    switch (op) {
        case '+':
            *res += *hold;
            break;
        case '-':
            *res -= *hold;
            break;
        case '*':
            *res *= *hold;
            break;
        case '/':
            *res /= *hold;
            break;
        case '%':
            *res %= * hold;
            break;
        case '^':
            if (*hold == 0) {
                *res = 1;
                break;
            }
            tmp = *res;
            for (int i = 1; i < *hold; ++i) {
                *res *= tmp;
            }
            break;
        default:
            perror("Unknown arith operation");
    }
}

void calc_expression(int *result)
{
    get_token();
    if (!*token) {
        perror("Problems with Expression");
        return;
    }
    term_sum(result);
//    putback();
}

void term_sum(int *result)
{
    char op;
    int hold;
    factor_mul(result);
    while ((op = *token) == '+' || op == '-' && tok != INCR) {
        get_token();
        factor_mul(&hold);
        arith(op, result, &hold);
    }
}

void factor_mul(int *result)
{
    char op;
    int hold;
    power(result);
    while ((op = *token) == '*' || op == '/' || op == '%') {
        get_token();
        power(&hold);
        arith(op, result, &hold);
    }
}

void power(int *result)
{
    int hold;
    unary(result);
    while (*token == '^') {
        get_token();
        unary(&hold);
        arith('^', result, &hold);
    }
}

void unary(int *result)
{
    char op = 0;
    if ((token_type == DELIM && tok != INCR) && (*token == '-' || *token == '+')) {
        op = *token;
        get_token();
    }
    braces_expr(result);

    if (op == '-') {
        *result = - (*result);
    }
}

void braces_expr(int *result)
{
    if ((token_type == DELIM) && (*token == '(')) {
        get_token();
        term_sum(result);
        if (*token != ')') {
            perror("Wrong Brace Expression");
        }
        get_token();
    } else {
        inc_dec(result);
        //get_primitive(result);
    }

}

void inc_dec(int *result)
{
    if (token_type == DELIM && tok == INCR) {

        int op = 0;
        if (strcmp(token, "++") == 0)
            op = 1;
        if (strcmp(token, "--") == 0)
            op = -1;
        get_token();
        if (token_type != VAR) {
            perror("Only VARs incrementable");
            return;
        }
        if (op == 0) {
            perror("Only ++ or --");
            return;
        }
        printf("Increment operation\n");
        char *name = strdup(token);

        get_primitive(result);
        *result += op;
        //TOD0:macro DONE
        int elem_ind;
        ht_get(vartable, name, elem_ind, hash_f, str_eq);
        ht_val(vartable, elem_ind) = *result;
//        ht_rewrite(vartable, name, *result);
        free(name);


    } else {
        get_primitive(result);
    }

}

void get_primitive(int *result)
{
    switch (token_type) {
        case VAR:
            *result = find_var(token);
            get_token();
            return;
        case NUM:
            *result = atoi(token);
            get_token();
            return;
        default:
            perror("Syntax Error");
    }
}

void assign(void)
{
    get_token();
    printf("'%s', %d, %d\n", token, token_type, tok);
    if (token_type != VAR) {
        perror("Wrong Var name!");
        return;
    }
    char *name = strdup(token);
    get_token();
    char *operator = strdup(token);
    int value;
    calc_expression(&value);
    if (*operator == '=') {
        //TOD0:macro DONE
        int ind, absnt;
        ht_append(vartable, char *, int, name, ind, absnt, hash_f, str_eq);
        printf("{absnt - %s %d}\n", name, absnt);
        if(absnt == 1 || absnt == 0)
            ht_val(vartable, ind) = value;
        //        ht_append(vartable, name, value);
//        free(name); не надо, т.к. ета именно строка записывается в хеш-таблицу
    } else if (operator[1] == '=') {
        printf("%s\n", name);
        int var = find_var(name);
        switch (*operator) {
            case '+':
                var += value;
                break;
            case '-':
                var -= value;
                break;
            case '*':
                var *= value;
                break;
            case '/':
                var /= value;
                break;
            case '%':
                var %= value;
                break;
        }
        //TOD0:macro DONE
        int ind;
        ht_get(vartable, name, ind, hash_f, str_eq);
        if (ind != 0) {
            ht_val(vartable, ind) = var;
        } else {
            return;
        }
        /*if (ht_rewrite(vartable, name, var) == -1)
            return;*/

        free(name);
        free(operator);
    } else
        perror("Assignment symbol expected");

}

long int filesize( FILE *fp )
{
    long int save_pos, size_of_file;

    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );
    return(size_of_file);
}

char *load_program(char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return 0;
    char *program = (char *) malloc(filesize(fp));
    if (!program)
        return 0;
    int i = 0;
    do {
        program[i++] = getc(fp);
    } while (!feof(fp));
    program[i-2] = '\0';
    fclose(fp);
    return program;

}

int main(int argc, char *argv[])
{
    //TOD0:macro DONE
    ht_init(vartable);
//    vartable = create_ht(200);

    printf("started\n");
    char *program = load_program("E:/Documents/main.slyap--");
    printf("loaded!\n");

    if (!program) {
        perror("Error Loading file!");
        exit(1);
    }

    prog = program;
    printf("program ::\n %s\n", prog);
    get_token();
    while (*token) {
        printf("'%s', %d, %d\n", token, token_type, tok);
        get_token();
    }
    prog = program;
    assign();
    printf(">>> %d\n", find_var("a"));
    assign();
    printf(">>> a = %d", find_var("a"));
    assign();
    assign();
    assign();
    printf(">>> slyapa = %d", find_var("slyapa"));
    free(program);
    return 0;
}