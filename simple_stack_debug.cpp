#include "simple_stack.h"

#define SET_ERR_CODE(condition, code) if(condition) err_code |= (code)

void stk_dump(FILE *log, my_stk *stk, int line, const char *file, const char *func, int err_code)
{
    assert(log != NULL);

    fprintf(log, "in file %s in function %s on line %d:\n", file, func, line);

    if((err_code & NULL_STK_PTR) == NULL_STK_PTR)
    {
        fprintf(log, "ERROR: NULL pointer to stack\n");
        return;
    }

    fprintf(log, "variable %s created in function %s in file %s in line %d\n",
       (stk->info).var_name, (stk->info).func_name, (stk->info).file_name, (stk->info).line);

    fprintf(log, "size = %d\ncapacity = %d\n", stk->size, stk->capacity);

    if((err_code & NULL_DATA_PTR) == NULL_DATA_PTR)
    {
        fprintf(log, "ERROR: NULL pointer to data\n");
        return;
    }

    if((err_code & BAD_CAPACITY) == BAD_CAPACITY) fprintf(log, "ERROR: invalid capacity\n");
    if((err_code & BAD_SIZE) == BAD_SIZE) fprintf(log, "ERROR: invalid size\n");
    if((err_code & CAPACITY_LESS_SIZE) == CAPACITY_LESS_SIZE) fprintf(log, "ERROR: size > capacity\n");

    #ifdef CANARY
    fprintf(log, "stack canaries:    %10llu %10llu\n", stk->stk_cnry_l, stk->stk_cnry_r);
    fprintf(log, "data canaries:     %10llu %10llu\n", *(stk->data_cnry_l), *(stk->data_cnry_r));
    fprintf(log, "expected canaries:\n");
    fprintf(log, "for stack:         %10llu\n", (cnry)stk);
    fprintf(log, "for data:          %10llu\n", (cnry)(stk->data));
    if((err_code & CANARY_DEAD) == CANARY_DEAD)
    {
        fprintf(log, "ERROR: canary dead\n");
    }
    #endif

    #ifdef HASH
    if((err_code & WRONG_HASH) == WRONG_HASH) fprintf(log, "ERROR: hash discrepancy\n");
    #endif

    ssize_t i = 0;
    for(; i < stk->size; i++)
    {
        fprintf(log, "*[%3d] = %.3lf\n", i, stk->data[i]);
    }
    for(; i < stk->capacity; i++)
    {
        fprintf(log, " [%3d] = %.3lf\n", i, stk->data[i]);
    }

    putc('\n', log);

    return;
}

char stk_assert(my_stk *stk, int line, const char *file, const char *func)
{
    int err_code = 0;

    if(stk == NULL)
    {
        err_code |= NULL_STK_PTR;

        //stk_dump(stk->log, stk, line, file, func, err_code);

        return 0;
    }

    SET_ERR_CODE(stk->data == NULL, NULL_DATA_PTR);
    SET_ERR_CODE(stk->capacity <= 0, BAD_CAPACITY);
    SET_ERR_CODE(stk->size < 0, BAD_SIZE);
    SET_ERR_CODE(stk->capacity < stk->size, CAPACITY_LESS_SIZE);

    #ifdef CANARY
    SET_ERR_CODE(stk->stk_cnry_l != (cnry)stk, CANARY_DEAD);
    SET_ERR_CODE(stk->stk_cnry_r != (cnry)stk, CANARY_DEAD);
    SET_ERR_CODE(*(stk->data_cnry_l) != (cnry)(stk->data), CANARY_DEAD);
    SET_ERR_CODE(*(stk->data_cnry_r) != (cnry)(stk->data), CANARY_DEAD);
    #endif

    #ifdef HASH
    SET_ERR_CODE(stk->hash_num != my_hash(stk, sizeof(*stk) - sizeof(unsigned long long int)), WRONG_HASH);
    #endif

    if(err_code != 0 && stk->log)
    {
        stk_dump(stk->log, stk, line, file, func, err_code);

        return 0;
    }

    return 1;
}
