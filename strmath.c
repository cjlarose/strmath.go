/*
 * File: strmath.c
 * Author: Chris La Rose
 * Purpose: Adds and subtracts arbitrarily long positive integers
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/*
 * MAGNITUDE is a configuration setting that represents how numbers should be 
 * represented during calcuations. Numbers will be represented in base 
 * 10^MAGNITUDE, so each digit will be at most 10^MAGNITUDE - 1.  In testing, 
 * this number should be low, like 3. In production, we want to store numbers 
 * in the highest base possible while keeping in the constraints of a 8-byte 
 * unsigned int. It turns out to be 18, because if we add (10^18 -1) + 
 * (10^18 - 1), we can still store the result in an unsigned long long int. 
 * The same statement is untrue for 19.
 */
const int MAGNITUDE = 18;

/*
 * is_digit_sequence(char *str) -- returns true iff the null-terminated string 
 * str contains only digit characters
 */
bool is_digit_sequence(char *str) {
    while (*str != '\0') {
        if (!isdigit(*str))
            return false;
        str++;
    }
    return true;
}

/*
 * get_digit_sequence() -- reads a decimal number string from a line of input 
 * from stdin. Returns a pointer to the string with leading zeros removed.  
 * Program exits if any non-digit characters are encountered or if the input 
 * stream is terminated
 */
char *get_digit_sequence() {
    size_t n = 0;
    char *line = NULL;
    ssize_t len;

    if ((len = getline(&line, &n, stdin)) <= 1) {
        fprintf(stderr, "%s\n", "Error: Not enough inputs");
        exit(1);
    }

    line[len-1] = '\0';
    if (!is_digit_sequence(line)) {
        fprintf(stderr, "%s\n", "Error: Not an integer");
        exit(1);
    }

    while (*line == '0')
        line++; 
    if (*line == '\0')
        line--;

    return line;
}

/*
 * power_of_ten(n) -- returns 10^n, for n >= 0
 */
unsigned long long power_of_ten(int n) {
    unsigned long long r = 1;
    int i;
    for (i = 0; i < n; i++)
        r *= 10;
    return r;
}

/*
 * to_big_integer(str, int *n) -- takes a seqeuence of digits as a string and 
 * returns an array of ints that represents the digits of the number in base 
 * 10^MAGNITUDE. Sets *n to the length of the array. The last element of the
 * array is the sign bit, where 0 indicates a positive number, and 10^MAGNITUDE
 * -1 is a negative number.  Index n-2 is the most significant digit and index
 * 0 is the least significant. This function assumes str represents a positive
 * integer and contains only digits.
 */
unsigned long long *to_big_integer(char *str, int *n) {
    int len = strlen(str);
    char *digit_seq = strcpy(malloc(len+1), str);

    int num_bigits = len % MAGNITUDE == 0 ? len / MAGNITUDE : len / MAGNITUDE + 1;
    // num_bigits = ceil(len/MAGNITUDE)
    unsigned long long *arr = malloc((num_bigits+1)*sizeof(long long));

    int i, j;
    for (i = len, j = 0; j < num_bigits; j++) {
        digit_seq[i] = '\0';
        i -= MAGNITUDE;
        if (i < 0)
            i = 0;
        sscanf(&digit_seq[i], "%lld", &arr[j]);
    }
    arr[j] = 0;
    *n = num_bigits + 1;
    return arr; 
}

/*
 * add(long long *op1, int op1_len, long long *op2, int op2_len, int *sum_len) 
 * -- takes two big ints, adds them, and returns the result as a bit int. Sets
 * *sum_len to the length of the resulting sum.
 */
unsigned long long *add_big_ints(unsigned long long *op1, int op1_len, unsigned long long *op2, int op2_len, int *sum_len) {
    int result_length = op1_len > op2_len ? op1_len : op2_len;
    unsigned long long *sum = malloc(result_length * sizeof(long long));
    unsigned long long max_digit = power_of_ten(MAGNITUDE);
    int i;
    int carry = 0;
    for (i = 0; i < result_length; i++) {
        unsigned long long a = i >= op1_len ? 0 : op1[i];
        unsigned long long b = i >= op2_len ? 0 : op2[i];
        unsigned long long result = a + b + carry;
        if (result >= max_digit) {
            carry = 1;
            result -= max_digit;
        } else
            carry = 0;
        sum[i] = result;
    }

    // if the sign bit is 1, overflow occured. Make a new digit.
    if (sum[result_length-1] == 1) {
        result_length++;
        sum = realloc(sum, result_length);
        sum[result_length-1] = 0;
    }
    *sum_len = result_length;
    return sum;
}

/*
 * complement_big_int(b[], n) -- takes a big int b of size n, and returns its 
 * negation.
 */
unsigned long long *complement_big_int(unsigned long long *b, int n, int *result_len) {
    unsigned long long max_digit = power_of_ten(MAGNITUDE) - 1;
    unsigned long long *complement = malloc(n * sizeof(long long));
    int i;
    for (i = 0; i < n; i++)
        complement[i] = max_digit - b[i];

    unsigned long long *one = malloc(2 * sizeof(long long));
    *one = 1;
    *(one+1) = 0;
    return add_big_ints(complement, n, one, 2, result_len);
}

/*
 * subtract_big_ints(minuend, x, subtrahend, y, diff_len) -- subtracts big int
 * subtrahend of size y from bit int minuend of size x. Sets *diff to the 
 * length of the result.
 */
unsigned long long *subtract_big_ints(unsigned long long *minuend, int min_len, unsigned long long *subtrahend, int sub_len, int *difference_len) {
    int comp_len;
    unsigned long long *comp = complement_big_int(subtrahend, sub_len, &comp_len);
    return add_big_ints(minuend, min_len, comp, comp_len, difference_len);
}

/*
 * print_big_int(long long *b, n) -- prints a big int as a base-10 number
 */
void print_big_int(unsigned long long *b, int n) {
    if (b[n-1] != 0) {
        b = complement_big_int(b, n, &n);
        printf("-");
    }
    n--;
    printf("%llu", b[--n]);
    // create an appropriate format string based on magnitude
    char format_str[7];
    sprintf(format_str, "%%0%dllu", MAGNITUDE);
    for (n--; n >= 0; n--)
        printf(format_str, b[n]);
    printf("\n");
}


/*
int main() {
    size_t n = -1;
    char *line = NULL;
    if (getline(&line, &n, stdin) == EOF) {
        fprintf(stderr, "%s\n", "Error: unsupported operation");
        return 1;
    }

    bool add;
    if (strcmp(line, "add\n") == 0)
        add = true;
    else if (strcmp(line, "sub\n") == 0)
        add = false;
    else {
        fprintf(stderr, "%s\n", "Error: unsupported operation");
        return 1;
    }

    char *op1 = get_digit_sequence();
    char *op2 = get_digit_sequence();

    int op1_len, op2_len;
    unsigned long long *op1_arr = to_big_integer(op1, &op1_len);
    unsigned long long *op2_arr = to_big_integer(op2, &op2_len);

    //printf("%s %s %s\n", op1, add ? "+" : "-", op2);

    int sum_len = 0;
    unsigned long long *sum;
    if (add)
        sum = add_big_ints(op1_arr, op1_len, op2_arr, op2_len, &sum_len);
    else
        sum = subtract_big_ints(op1_arr, op1_len, op2_arr, op2_len, &sum_len);

    print_big_int(sum, sum_len);

    return 0;
}
*/
