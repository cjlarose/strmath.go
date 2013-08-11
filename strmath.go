package main
/*
#include "strmath.h"
*/
import "C"

import (
    "fmt"
)

func toBigInteger(str string) (*C.ulonglong, C.int) {
    len := C.int(0)
    result := C.to_big_integer(C.CString(str), &len)
    return result, len
}

func GetDigitSequence() string {
    return C.GoString(C.get_digit_sequence())
}

func AddStrings(a, b string) (*C.ulonglong, *C.ulonglong, int, int) {
    //sum_len := C.int(0)
    x, x_len := toBigInteger(a)
    y, y_len := toBigInteger(b)
    //c := C.add_big_ints(x, x_len, y, y_len, &sum_len)
    //C.print_big_int(c, sum_len)
    return x, y, int(x_len), int(y_len)
}

func main() {
    a, b := GetDigitSequence(), GetDigitSequence()
    fmt.Printf("%s + %s = ", a, b)
    AddStrings(a,b)
}
