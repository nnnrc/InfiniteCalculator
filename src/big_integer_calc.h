#pragma once

#include "big_integer.h"

/*
 실제 BigInteger 연산파트
*/

// 실제로 BigInteger의 덧셈을 처리하는 함수
BigInteger* BigIntegerAddImpl(BigInteger*, BigInteger*, int sign);
// 실제로 BigInteger의 뺄셈을 처리하는 함수
BigInteger* BigIntegerSubtractImpl(BigInteger*, BigInteger*, int sign);
// 실제로 BigInteger의 곱셈을 처리하는 함수
BigInteger* BigIntegerMultiplyImpl(BigInteger*, BigInteger*, int sign);
// 실제로 BigInteger의 나눗셈을 처리하는 함수
void BigIntegerDivideImpl(BigInteger*, BigInteger*, BigInteger** quotient_p, BigInteger** remainder_p);

// 부호없는 32비트 정수 배열을 받아서 필요없는 부분을 제거한뒤 새로운 배열과 그 길이를 리턴
size_t bi_trim_array(uint32_t** dest, uint32_t* src, size_t size);
