#pragma once

#include <stdlib.h>
#include <stdint.h>

/*
 무한정수를 저장하는 타입
 부호없는 32비트 정수배열로 수를 저장
 한 칸에 2^32 - 1까지 저장(2진수 그대로 이어붙인거랑 같음)
 시프트연산을 이용해서 빠른 나눗셈 연산을 하기 위해 2진법(2^32진법) 사용.
 10진수 문자열로 입력받고 출력하기 위해 Double Dabble 알고리즘 사용
 기본적으로 불변(immutable, 피연산자의 값이 바뀌지 않음)임 연산결과로 새로운 BigInteger 반환
 덧셈, 뺼셈, 곱셈, 나눗셈(몫, 나머지), 최대공약수(GCD) 연산 가능
*/

typedef struct {
	int sign;       // 부호, 1이면 양수, -1이면 음수, 0이면 0
	uint32_t* nums; // 실제 수를 저장하는 배열
	size_t size;    // 배열 크기
} BigInteger;

// 덧셈을 수행하는 함수 (val1 + val2)
BigInteger* BigIntegerAdd(BigInteger* val1, BigInteger* val2);
// 뺄셈을 수행하는 함수 (val1 - val2)
BigInteger* BigIntegerSubtract(BigInteger* val1, BigInteger* val2);
// 곱셈을 수행하는 함수 (val1 * val2)
BigInteger* BigIntegerMultiply(BigInteger* val1, BigInteger* val2);
// 나눗셈을 수행하는 함수 (dividend / divisor) 정수로된 몫을 반환 나머지는 버림. 이진 나눗셈 알고리즘 사용
BigInteger* BigIntegerDivide(BigInteger* dividend, BigInteger* divisor);
// 나머지 연산을 수행하는 함수 (dividend mod divisor)
BigInteger* BigIntegerMod(BigInteger* dividend, BigInteger* divisor);
// 나머지 연산을 수행하는 함수 (dividend / divisor) 몫을 리턴하고 remainder로 나머지를 전달
BigInteger* BigIntegerDivideAndMod(BigInteger* dividend, BigInteger* divisor, BigInteger** remainder);
// 두 수의 최대공약수를 반환하는 함수 (gcd(val1, val2)) 유클리드 호제법 사용
BigInteger* BigIntegerGcd(BigInteger* val1, BigInteger* val2);

// 두 수를 비교하는 함수 val1이 더 크면 1, 작으면 -1, 같으면 0리턴
int BigIntegerCompare(BigInteger* val1, BigInteger* val2);
// 두 수의 절대값을 비교하는 함수 val1이 더 크면 1, 작으면 -1, 같으면 0리턴
int BigIntegerCompareAbs(BigInteger* val1, BigInteger* val2);

// BigInteger 객체를 만드는 함수
BigInteger* NewBigInteger(int sign, uint32_t* nums, size_t size);
// BigInteger의 메모리 할당을 해제하는 함수 free말고 이거 호출해서 해제해야됨
void ReleaseBigInteger(BigInteger*);
// BigInteger 객체를 복제하는 함수, 불변이지만 메모리 할당 해제시에 문제가 있을 수 있기에 복제를 쓰는 경우가 있음
BigInteger* CloneBigInteger(BigInteger*);

// 정수 문자열을 이용해 BigInteger 객체를 만드는 함수, 10진수를 2진수 형태로 바꾸어서 저장
BigInteger* NewBigIntegerFromString(char*);
// BigInteger를 10진수 정수 문자열로 바꾸는 함수, 2진수 형태를 10진수 문자열로 변환
size_t BigIntegerToString(BigInteger*, char*);
