#include <stdbool.h>

bool is_digit_sequence(char *str);

char *get_digit_sequence();

unsigned long long power_of_ten(int n);

unsigned long long *to_big_integer(char *str, int *n);

unsigned long long *add_big_ints(unsigned long long *op1, int op1_len, unsigned long long *op2, int op2_len, int *sum_len);

unsigned long long *subtract_big_ints(unsigned long long *minuend, int min_len, unsigned long long *subtrahend, int sub_len, int *difference_len);

void print_big_int(unsigned long long *b, int n);
