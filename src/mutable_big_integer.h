#pragma once

#include "big_integer.h"

/*
 변할 수 있는(mutable) 무한정수를 저장하는 타입
 부호가 없음
 덧셈, 뺄셈, 비트시프트 연산을 지원
 나눗셈, 곱셈등에서 임시로 값을 저장하거나 비트시프트를 위해 사용
*/

typedef struct {
	uint32_t* nums;     // 실제 수를 저장하는 배열
	size_t size;        // 배열 크기
	size_t actual_size; // 실제 사용중인 크기
} MutableBigInteger;

// 덧셈을 수행하는 함수, 결과 값은 val1에 저장됨 (val1 = val1 + val2)
void MutableBigIntegerAdd(MutableBigInteger* val1, MutableBigInteger* val2);
// 뺄셈을 수행하는 함수, 결과 값은 val1에 저장됨 (val1 = val1 - val2)
void MutableBigIntegerSubtract(MutableBigInteger* val1, MutableBigInteger* val2);

// 왼쪽으로 shift 비트만큼 시프트
void MutableBigIntegerShiftLeft(MutableBigInteger* val, size_t shift);
// 오른쪽으로 shift 비트만큼 시프트
void MutableBigIntegerShiftRight(MutableBigInteger* val, size_t shift);
// 두 수를 비교하는 함수 val1이 더 크면 1, 작으면 -1, 같으면 0리턴
int MutableBigIntegerCompare(MutableBigInteger* val1, MutableBigInteger* val2);

// size 만큼의 크기를 가지는 MutableBigInteger 객체를 생성
MutableBigInteger* NewMutableBigInteger(size_t size);
// size 만큼의 크기를 가지는 MutableBigInteger 객체를 생성한뒤 BigInteger 객체에 저장돤 값을 담음
MutableBigInteger* NewMutableBigIntegerFromBigInteger(size_t size, BigInteger* value);
// MutableBigInteger 객체의 메모리 할당 해제
void ReleaseMutableBigInteger(MutableBigInteger* val);
