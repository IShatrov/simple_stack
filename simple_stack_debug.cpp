#include "simple_stack.h"

#define SET_ERR_CODE(condition, code) if(condition) err_code |= (code)
#define PRINT_ERR(err, msg) if((err_code & err) == err) fprintf(log, "ERROR: %s\n", #msg);

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

    PRINT_ERR(BAD_CAPACITY, invalid capacity);
    PRINT_ERR(BAD_SIZE, invalid size);
    PRINT_ERR(CAPACITY_LESS_SIZE, size>capacity);

    #ifdef CANARY
    PRINT_ERR(CANARY_DEAD_LS, left stack canary dead);
    PRINT_ERR(CANARY_DEAD_RS, right stack canary dead);
    PRINT_ERR(CANARY_DEAD_LD, left data canary dead);
    PRINT_ERR(CANARY_DEAD_RD, right data canary dead);
    #endif

    #ifdef HASH
    PRINT_ERR(WRONG_HASH, hash discrepancy);
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
    SET_ERR_CODE(check_cnry(&stk->stk_cnry_l), CANARY_DEAD_LS);
    SET_ERR_CODE(check_cnry(&stk->stk_cnry_r), CANARY_DEAD_RS);
    SET_ERR_CODE(check_cnry(stk->data_cnry_r), CANARY_DEAD_LD);
    SET_ERR_CODE(check_cnry(stk->data_cnry_r), CANARY_DEAD_RD);
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
