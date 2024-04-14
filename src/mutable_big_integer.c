#include <stdio.h>
#include <string.h>

#include "mutable_big_integer.h"

void MutableBigIntegerAdd(MutableBigInteger* val1, MutableBigInteger* val2)
{
	if (val1->size <= val2->actual_size) printf("*** Mutableinfinite_integer_add : Overflow may occur.\n");

	uint32_t carry = 0; // 올림수
	for (size_t i = 0; i < val1->size; i++) {
		if (i >= val2->actual_size && carry == 0) break; // val2의 최대 크기를 넘어가고 올림수가 0인경우 추가 계산이 불필요함으로 멈춤
		uint32_t sum = val1->nums[i] + ((i < val2->actual_size) ? val2->nums[i] : 0) + carry;
		if (sum < val1->nums[i]) carry = 1; // 받아올림이 발생하는 경우 올림수를 1로 설정, 오버플로우가 나서 해당 자리의 수는 알아서 잘들어감
		else carry = 0; // 받아올림이 발생하지 않는 경우 올림수를 0으로 설정
		val1->nums[i] = sum;
	}

	if (val1->nums[val1->actual_size] > 0) val1->actual_size++; // 최상위 다음 데이터가 0 초과면 실제 크기 + 1
}

void MutableBigIntegerSubtract(MutableBigInteger* val1, MutableBigInteger* val2)
{
	if (MutableBigIntegerCompare(val1, val2) == -1) {
		printf("*** Mutableinfinite_integer_subtract : val1 must be larger than val2\n");
		exit(1);
	}

	uint32_t borrow = 0; // 빌림수
	for (size_t i = 0; i < val1->actual_size; i++) {
		uint32_t val2_num = ((i < val2->actual_size) ? val2->nums[i] : 0);
		if (i >= val2->actual_size && borrow == 0) break; // val2의 최대 크기를 넘어가고 빌림수가 0이면 추가 계산이 불필요함으로 멈춤
		if (val1->nums[i] < val2_num || (val1->nums[i] == val2_num && borrow > 0)) { // 받아내림이 발생하는 경우
			val1->nums[i] = 0 - (val2_num - val1->nums[i]) - borrow; // 부호없는 32비트 정수형 최대값 + 1 + val1 - val2 - 빌림수
			borrow = 1; // 받아내림이 발생 했으므로 빌림수를 1로 설정
		}
		else {
			val1->nums[i] = val1->nums[i] - val2_num - borrow;
			borrow = 0; // 받아내림이 발생하지 않았으므로 빌림수를 0으로 설정
		}
	}

	if (val1->nums[val1->actual_size - 1] < 1) val1->actual_size--; // 최상위 데이터가 0이면 실제 크기 - 1
}

void MutableBigIntegerShiftLeft(MutableBigInteger* val, size_t shift)
{
	size_t shift32 = shift / 32; // 32비트 단위 시프트
	shift %= 32; // 1비트 단위 시프트

	for (size_t i = val->actual_size + shift32 + 1; i > shift32; i--) {
		if (i > val->size) continue; // 최대 크기를 넘어갔을 경우 무시함
		val->nums[i - 1] = val->nums[i - shift32 - 1] << shift;
		if (shift > 0 && i - shift32 > 1) // 32비트 타입에 32비트 시프트는 Undefined Behavior이기 때문에 걸러줌
			val->nums[i - 1] |= val->nums[i - shift32 - 2] >> (32 - shift); 
	}

	memset(val->nums, 0, sizeof(uint32_t) * shift32); // 하위 비트 공간을 밀린만큼 0으로 채워줌
	val->actual_size += shift32; // 실제 크기 계산
	if (val->actual_size >= val->size) val->actual_size = val->size; // 최대 크기를 넘었을 경우 최대 크기를 실제 크기로 설정
	else val->actual_size += (val->nums[val->actual_size] > 0); // 최상위 데이터가 0이 아니면 1을 더해줌
}

void MutableBigIntegerShiftRight(MutableBigInteger* val, size_t shift)
{
	size_t shift32 = shift / 32; // 32비트 단위 시프트
	shift %= 32; // 1비트 단위 시프트

	for (size_t i = 0; i < val->actual_size - shift32; i++) {
		val->nums[i] = val->nums[i + shift32] >> shift;
		if (shift > 0 && i + shift32 + 1 < val->size) // 32비트 타입에 32비트 시프트는 Undefined Behavior이기 때문에 걸러줌
			val->nums[i] |= val->nums[i + shift32 + 1] << (32 - shift);
	}

	memset(val->nums + val->actual_size - shift32, 0, sizeof(uint32_t) * shift32); // 상위 비트 공간을 밀린만큼 0으로 채워줌
	val->actual_size -= shift32; // 실제 크기 계산
	if (val->actual_size <= 0) val->actual_size = 0; // 0 이하면 0으로 실제 크기 설정
	else val->actual_size -= (val->nums[val->actual_size - 1] == 0); // 최상위 데이터가 0인 경우 1을 빼줌
}

int MutableBigIntegerCompare(MutableBigInteger* val1, MutableBigInteger* val2)
{
	if (val1->actual_size < val2->actual_size) return -1;
	if (val1->actual_size > val2->actual_size) return 1;
	for (size_t i = 0; i < val1->actual_size; i++) {
		uint32_t num1 = val1->nums[val1->actual_size - i - 1];
		uint32_t num2 = val2->nums[val2->actual_size - i - 1];
		if (num1 < num2) return -1;
		if (num1 > num2) return 1;
	}
	return 0;
}

MutableBigInteger* NewMutableBigInteger(size_t size)
{
	MutableBigInteger* result = malloc(sizeof(MutableBigInteger));
	result->size = size;
	result->nums = calloc(size, sizeof(uint32_t));
	result->actual_size = 0;
	return result;
}

MutableBigInteger* NewMutableBigIntegerFromBigInteger(size_t size, BigInteger* value)
{
	MutableBigInteger* result = NewMutableBigInteger(size);
	memcpy(result->nums, value->nums, sizeof(uint32_t) * value->size);
	result->actual_size = value->size;
	return result;
}

void ReleaseMutableBigInteger(MutableBigInteger* val)
{
	if (!val) return;
	if (val->nums) free(val->nums);
	free(val);
}
