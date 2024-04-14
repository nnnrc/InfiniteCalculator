#include <stdio.h>
#include <string.h>

#include "mutable_big_integer.h"
#include "big_integer_calc.h"

BigInteger* BigIntegerAddImpl(BigInteger* val1, BigInteger* val2, int sign)
{
	if (val1->size < val2->size) {
		BigInteger* temp = val1;
		val1 = val2;
		val2 = temp;
	}

	uint32_t* temp_array = malloc(sizeof(uint32_t) * val1->size); // 계산결과를 저장할 임시배열
	uint32_t carry = 0; // 올림수
	for (size_t i = 0; i < val1->size; i++) {
		uint32_t sum = val1->nums[i] + ((i < val2->size) ? val2->nums[i] : 0) + carry;
		if (sum < val1->nums[i]) carry = 1; // 받아올림이 발생하는 경우 올림수를 1로 설정, 오버플로우가 나서 해당 자리의 수는 알아서 잘들어감
		else carry = 0; // 받아올림이 발생하지 않는 경우 올림수를 0으로 설정
		temp_array[i] = sum;
	}

	if (carry > 0) { // 올림수가 0이상인경우, 연산결과값의 길이가 피연산자의 최대길이 + 1이됨
		uint32_t* new_array = malloc(sizeof(uint32_t) * (val1->size + 1));
		memcpy(new_array, temp_array, sizeof(uint32_t) * val1->size);
		new_array[val1->size] = carry;
		free(temp_array);
		return NewBigInteger(sign, new_array, val1->size + 1);
	}

	return NewBigInteger(sign, temp_array, val1->size);
}

BigInteger* BigIntegerSubtractImpl(BigInteger* val1, BigInteger* val2, int sign)
{
	if (BigIntegerCompareAbs(val1, val2) == -1) {
		printf("*** infinite_integer_subtract : val1 must be larger than val2\n");
		exit(1);
	}

	uint32_t* temp_array = malloc(sizeof(uint32_t) * val1->size); // 계산결과를 저장할 임시배열
	uint32_t borrow = 0; // 빌림수
	for (size_t i = 0; i < val1->size; i++) {
		uint32_t val2_num = ((i < val2->size) ? val2->nums[i] : 0);
		if (val1->nums[i] < val2_num || (val1->nums[i] == val2_num && borrow > 0)) { // 받아내림이 발생하는 경우
			temp_array[i] = 0 - (val2_num - val1->nums[i]) - borrow; // 부호없는 32비트 정수형 최대값 + 1 + val1 - val2 - 빌림수
			borrow = 1; // 받아내림이 발생 했으므로 빌림수를 1로 설정
		}
		else {
			temp_array[i] = val1->nums[i] - val2_num - borrow;
			borrow = 0; // 받아내림이 발생하지 않았으므로 빌림수를 0으로 설정
		}
	}

	// 계산결과 트리밍
	uint32_t* trimed_array;
	uint64_t trimed_size = bi_trim_array(&trimed_array, temp_array, val1->size);
	free(temp_array);
	if (trimed_size == 0) return NewBigIntegerFromString("0");
	return NewBigInteger(sign, trimed_array, trimed_size);
}

BigInteger* BigIntegerMultiplyImpl(BigInteger* val1, BigInteger* val2, int sign)
{
	if (val1->size < val2->size) {
		BigInteger* temp = val1;
		val1 = val2;
		val2 = temp;
	}

	MutableBigInteger* temp_integer = NewMutableBigInteger(val1->size + val2->size + 2); // 덧셈 결과를 저장하는 임시 MutableBigInteger
	MutableBigInteger* temp_integer2 = NewMutableBigInteger(val1->size + val2->size + 2); // 곱셈 결과를 저장하는 임시 MutableBigInteger
	for (size_t i = 0; i < val2->size; i++) {
		for (size_t j = 0; j < val1->size; j++) {
			uint64_t temp = (uint64_t)val1->nums[j] * (uint64_t)val2->nums[i];
			temp_integer2->nums[i + j] = (uint32_t) temp; // 2^32보다 작은 부분
			temp_integer2->nums[i + j + 1] = temp >> 32; // 2^32 이상인 부분
			temp_integer2->actual_size = i + j + 2; // 실제 크기 설정
			MutableBigIntegerAdd(temp_integer, temp_integer2); // 곱한값을 임시 MutableBigInteger에 더해줌
			temp_integer2->nums[i + j] = 0;
			temp_integer2->nums[i + j + 1] = 0;
		}
	}

	// 계산결과 트리밍
	uint32_t* trimed_array;
	size_t trimed_size = bi_trim_array(&trimed_array, temp_integer->nums, temp_integer->size);

	ReleaseMutableBigInteger(temp_integer);
	ReleaseMutableBigInteger(temp_integer2);

	return NewBigInteger(sign, trimed_array, trimed_size);
}

void BigIntegerDivideImpl(BigInteger* val1, BigInteger* val2, BigInteger** quotient_p, BigInteger** remainder_p)
{
	// 이진 나눗셈 사용
	MutableBigInteger* divisor = NewMutableBigIntegerFromBigInteger(val2->size, val2); // 제수를 담는 MutableBigInteger, 얘는 불변이여도 되는데 MutableBigInteger끼리만 뺄셈이 되게 구현해놔서 이렇게 만듦
	MutableBigInteger* quotient = NewMutableBigInteger(val1->size); // 몫을 담는 MutableBigInteger
	MutableBigInteger* remainder = NewMutableBigInteger(val2->size * 2); // 나머지를 담는 MutableBigInteger
	for (size_t i = 0; i < val1->size; i++) {
		for (size_t j = 1; j <= 32; j++) {
			MutableBigIntegerShiftLeft(remainder, 1); // 나머지 왼쪽으로 1비트 시프트
			remainder->nums[0] |= (val1->nums[val1->size - i - 1] >> ((size_t)32 - j)) & 1; // 피제수 한비트씩 가져옴
			MutableBigIntegerShiftLeft(remainder, 0); // 실제 크기 재계산용 0비트 시프트(반복문 비교보다 이게 빠름)
			MutableBigIntegerShiftLeft(quotient, 1); // 몫 왼쪽으로 1비트 시프트
			if (MutableBigIntegerCompare(remainder, divisor) >= 0) { // 나머지가 제수보다 큰 경우
				MutableBigIntegerSubtract(remainder, divisor); // 나머지에서 제수 빼줌
				quotient->nums[0] |= 1; // 몫에 1 더해줌
				MutableBigIntegerShiftLeft(quotient, 0); // 실제 크기 재계산
			}
		}
	}
	if (quotient_p) {
		// 몫 전달
		uint32_t* trimed_array;
		size_t trimed_size = bi_trim_array(&trimed_array, quotient->nums, quotient->actual_size);
		if (trimed_size == 0) {
			*quotient_p = NewBigIntegerFromString("0");
		}
		else {
			*quotient_p = NewBigInteger(1, trimed_array, trimed_size);
		}
	}

	if (remainder_p) {
		// 나머지 전달
		uint32_t* trimed_array;
		size_t trimed_size = bi_trim_array(&trimed_array, remainder->nums, remainder->actual_size);
		if (trimed_size == 0) {
			*remainder_p = NewBigIntegerFromString("0");
		}
		else {
			*remainder_p = NewBigInteger(1, trimed_array, trimed_size);
		}
	}

	ReleaseMutableBigInteger(divisor);
	ReleaseMutableBigInteger(quotient);
	ReleaseMutableBigInteger(remainder);
}

size_t bi_trim_array(uint32_t** dest, uint32_t* src, size_t size)
{
	size_t new_size = size;
	for (size_t i = 0; i < size; i++) {
		if (src[size - i - 1] != 0) break;
		new_size--;
	}

	if (new_size == 0) {
		*dest = 0;
		return 0;
	}

	uint32_t* new_array = malloc(sizeof(uint32_t) * new_size);
	memcpy(new_array, src, sizeof(uint32_t) * new_size);
	*dest = new_array;
	return new_size;
}
