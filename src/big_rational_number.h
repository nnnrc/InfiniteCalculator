#pragma once

#include "big_integer.h"

/*
 무한 유리수를 저장하는 타입
 계산 오차를 없애기 위해 분수꼴로 저장함
 출력할때 잘리는거를 제외하고 계산시에 오차는 존재하지 않음
 BigInteger와 마찬가지로 불변(immutable)임
*/

typedef struct {
	int sign;                // 부호, 1이면 양수, -1이면 음수, 0이면 0, 분자와 분모의 BigInteger는 무조건 양수이고 이 값으로만 부호를 판단함
	BigInteger* numerator;   // 분모
	BigInteger* denominator; // 분자
} BigRationalNumber;

// 덧셈을 수행하는 함수 (val1 + val2)
BigRationalNumber* BigRationalNumberAdd(BigRationalNumber* val1, BigRationalNumber* val2);
// 뺄셈을 수행하는 함수 (val1 - val2)
BigRationalNumber* BigRationalNumberSubtract(BigRationalNumber* val1, BigRationalNumber* val2);
// 곱셈을 수행하는 함수 (val1 * val2)
BigRationalNumber* BigRationalNumberMultiply(BigRationalNumber* val1, BigRationalNumber* val2);
// 나눗셈을 수행하는 함수 (dividend / divisor)
BigRationalNumber* BigRationalNumberDivide(BigRationalNumber* dividend, BigRationalNumber* divisor);

// 문자열을 받아와 BigRationalNumber 객체를 만드는 함수
BigRationalNumber* NewBigRationalNumber(char*);
// BigInteger를 받아와 BigRationalNumber 객체를 만드는 함수 (numerator / denominator)
BigRationalNumber* NewBigRationalNumberFromBigIntegers(BigInteger* numerator, BigInteger* denominator);
// BigRationalNumber의 메모리 할당을 해제하는 함수 free말고 이거 호출해서 해제해야됨
void ReleaseBigRationalNumber(BigRationalNumber*);

// BigRationalNumber를 유리수(소수) 문자열로 바꾸는 함수, decimal_length만큼 소수점 밑자리 표기
void BigRationalNumberToString(BigRationalNumber*, char*, unsigned int decimal_length);
