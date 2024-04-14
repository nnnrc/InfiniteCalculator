#include <stdio.h>
#include <string.h>

#include "big_integer.h"
#include "big_integer_calc.h"
#include "mutable_big_integer.h"

BigInteger* BigIntegerAdd(BigInteger* val1, BigInteger* val2)
{
	if (val1->sign == 0) return CloneBigInteger(val2); // 피연산자 하나가 0인경우 값 그대로 반환
	if (val2->sign == 0) return CloneBigInteger(val1);
	if (val1->sign == val2->sign) return BigIntegerAddImpl(val1, val2, val1->sign); // 부호가 같은 경우 그냥 더함

	int cmp = BigIntegerCompareAbs(val1, val2);
	if (cmp == 0) return NewBigIntegerFromString("0"); // 부호가 다르고 절대값은 같은 경우 0

	// 부호가 다르고 절대값도 다른 경우
	return (cmp > 0) ? BigIntegerSubtractImpl(val1, val2, (cmp == val1->sign) ? 1 : -1) : BigIntegerSubtractImpl(val2, val1, (cmp == val1->sign) ? 1 : -1);
}

BigInteger* BigIntegerSubtract(BigInteger* val1, BigInteger* val2)
{
	if (val1->sign == 0) { // val1이 0인경우 -val2 반환
        BigInteger* temp = CloneBigInteger(val2);
        temp->sign *= -1;
        return temp;
    }
	if (val2->sign == 0) return CloneBigInteger(val1); // val2가 0인경우 val1 반환
	if (val1->sign != val2->sign) return BigIntegerAddImpl(val1, val2, val1->sign); // 부호가 다른경우 더함

	int cmp = BigIntegerCompareAbs(val1, val2);
	if (cmp == 0) return NewBigIntegerFromString("0"); // 부호가 같고 절대밧은 같은 경우 0

	// 부호가 같고 절대값이 다른 경우
	return (cmp > 0) ? BigIntegerSubtractImpl(val1, val2, (cmp == val1->sign) ? 1 : -1) : BigIntegerSubtractImpl(val2, val1, (cmp == val1->sign) ? 1 : -1);
}

BigInteger* BigIntegerMultiply(BigInteger* val1, BigInteger* val2)
{
	if (val1->sign == 0 || val2->sign == 0) return NewBigIntegerFromString("0"); // 피연산자중 하나라도 0이면 0 반환
	return BigIntegerMultiplyImpl(val1, val2, (val1->sign == val2->sign) ? 1 : -1);
}

BigInteger* BigIntegerDivide(BigInteger* val1, BigInteger* val2)
{
	if (val2->sign == 0) {
		printf("*** BigIntegerDivide : Division by 0.\n");
		return NULL;
	}
	if (val1->sign == 0) return NewBigIntegerFromString("0");

	BigInteger* result;
	BigIntegerDivideImpl(val1, val2, &result, NULL);
	return result;
}

BigInteger* BigIntegerMod(BigInteger* val1, BigInteger* val2)
{
	if (val2->sign == 0) {
		printf("*** BigIntegerMod : Division by 0.\n");
		return NULL;
	}
	if (val1->sign == 0) return NewBigIntegerFromString("0");

	BigInteger* result;
	BigIntegerDivideImpl(val1, val2, NULL, &result);
	return result;
}

BigInteger* BigIntegerDivideAndMod(BigInteger* val1, BigInteger* val2, BigInteger** remainder)
{
	if (val2->sign == 0) {
		printf("*** BigIntegerDivideAndMod : Division by 0.\n");
		return NULL;
	}
	if (val1->sign == 0) {
		*remainder = NewBigIntegerFromString("0");
		return NewBigIntegerFromString("0");
	}

	BigInteger* result;
	BigIntegerDivideImpl(val1, val2, &result, remainder);
	return result;
}

BigInteger* BigIntegerGcd(BigInteger* val1, BigInteger* val2)
{
	BigInteger* one_temp = NewBigIntegerFromString("1");
	// 피연산자 하나라도 1인경우 최대 공약수는 1
	if (BigIntegerCompare(val1, one_temp) == 0 || BigIntegerCompare(val2, one_temp)) {
		return one_temp;
	}
	ReleaseBigInteger(one_temp);

	// 유클리드 호제법 구현
	BigInteger* a = CloneBigInteger(val1), * b = CloneBigInteger(val2), * c;
	int u = 0;
	while (b->sign != 0) {
		c = a;
		a = b;
		b = BigIntegerMod(c, b);
		ReleaseBigInteger(c);
	}
	ReleaseBigInteger(b);
	return a;
}

int BigIntegerCompare(BigInteger* val1, BigInteger* val2)
{
	if (val1->sign == val2->sign) {
		if (val1->sign > 0) return BigIntegerCompareAbs(val1, val2);
		if (val1->sign < 0) return BigIntegerCompareAbs(val2, val1);
		return 0;
	}
	return (val1->sign > val2->sign) ? 1 : -1;
}

int BigIntegerCompareAbs(BigInteger* val1, BigInteger* val2)
{
	if (val1->size < val2->size) return -1;
	if (val1->size > val2->size) return 1;
	for (size_t i = 0; i < val1->size; i++) {
		uint32_t num1 = val1->nums[val1->size - i - 1];
		uint32_t num2 = val2->nums[val2->size - i - 1];
		if (num1 < num2) return -1;
		if (num1 > num2) return 1;
	}
	return 0;
}

BigInteger* NewBigInteger(int sign, uint32_t* nums, size_t size)
{
	BigInteger* result = malloc(sizeof(BigInteger));
	result->sign = sign;
	result->nums = nums;
	result->size = size;

	return result;
}

void ReleaseBigInteger(BigInteger* val)
{
	if (!val) return;
	if (val->nums) free(val->nums);
	free(val);
}

BigInteger* CloneBigInteger(BigInteger* val)
{
	if (val->sign != 0) {
		uint32_t* new_array = malloc(sizeof(uint32_t) * val->size);
		memcpy(new_array, val->nums, sizeof(uint32_t) * val->size);
		return NewBigInteger(val->sign, new_array, val->size);
	}
	return NewBigIntegerFromString("0");
}

BigInteger* NewBigIntegerFromString(char* str)
{
	int sign = 1;
	if (str[0] == '-') {
		sign = -1;
		str++;
	}

	size_t str_length = strlen(str);
	if (str_length == 0) {
		printf("*** NewBigIntegerFromString : Not number string\n");
		return NULL;
	}

	// 9의 배수길이가 아닌경우 부족한부분을 0으로 패딩
	char* temp_str = malloc(sizeof(char) * (str_length + 9 - str_length % 9) + 1);
	memset(temp_str, '0', sizeof(char) * (9 - str_length % 9));
	memcpy(temp_str + (9 - str_length % 9), str, str_length + 1);
	str_length += 9 - str_length % 9;

	// 문자열을 9자리씩 끊어서 MutableBigInteger에 저장
	MutableBigInteger* temp_integer = NewMutableBigInteger(str_length / 9);
	temp_integer->actual_size = temp_integer->size;
	for (size_t i = 0; i < temp_integer->size; i++) {
		uint32_t mul = 100000000;
		for (size_t j = 0; j < 9; j++) {
			if (temp_str[i * 9 + j] < 48 || temp_str[i * 9 + j] > 57) {
				printf("*** NewBigIntegerFromString : Not number character\n");
				printf(" : %c\n", temp_str[i * 9 + j]);
				goto END;
			}
			temp_integer->nums[temp_integer->size - i - 1] += (temp_str[i * 9 + j] - 48) * mul;
			mul /= 10;
		}
	}

	// 32비트단위 Double-Dabble 알고리즘 수행(역과정)
	uint32_t* temp_array = calloc(temp_integer->size, sizeof(uint32_t));
	for (size_t i = 0; i < temp_integer->size; i++) {
		for (size_t j = 0; j < 32; j++) {
			temp_array[i] |= (temp_integer->nums[0] & 1) << j; // 10^9 진법으로 저장된 값을 1비트씩 오른쪽으로 시피트해서 가져옴
			MutableBigIntegerShiftRight(temp_integer, 1);
			for (size_t k = 0; k < temp_integer->actual_size; k++) {
				if (temp_integer->nums[k] >= 2147483648) // 2^32 진법에서 2를 곱하면 자리올림이 발생하는 값(2^31) 이상이면
					temp_integer->nums[k] -= 1647483648; // 10^9 진법에서 2를 곱하면 자리올림이 발생하는 값(5 * 10^8)과의 차이만큼 빼줌
			}
		}
	}

	// 32비트 정수배열 트리밍
	uint32_t* trimed_array = 0;
	size_t trimed_size = bi_trim_array(&trimed_array, temp_array, temp_integer->size);
	if (trimed_size == 0) sign = 0;

	free(temp_array);

END:
	ReleaseMutableBigInteger(temp_integer);
	free(temp_str);

	return NewBigInteger(sign, trimed_array, trimed_size);
}

size_t BigIntegerToString(BigInteger* val, char* str)
{
	char* org = str;
	if (val->sign == 0) {
		str[0] = '0';
		str[1] = '\0';
		return 1;
	}

	// 32비트단위 Double-Dabble 알고리즘 수행
	// a진수를 b진수로 변환시 필요크기 logb(a) log10^9(2^32) = 약 1.1
	size_t size10 = val->size * 12 / 10 + 2;
	MutableBigInteger* temp_integer = NewMutableBigInteger(size10);
	temp_integer->actual_size = 1;
	for (size_t i = 0; i < val->size; i++) {
		for (size_t j = 0; j < 32; j++) {
			for (size_t k = 0; k < temp_integer->actual_size; k++) {
				if (temp_integer->nums[k] >= 500000000) // 10^9 진법에서 2를 곱하면 자리올림이 발생하는 값(5 * 10^8) 이상이면
					temp_integer->nums[k] += 1647483648; // 2^32 진법에서도 2를 곱하면 자리올림이 발생하는 값(2^32)과의 차이만큼 더해줌
			}
			MutableBigIntegerShiftLeft(temp_integer, 1); // 2^32 진법으로 저장된 값을 1비트씩 왼쪽으로 시프트해서 넘겨줌
			temp_integer->nums[0] |= (val->nums[val->size - i - 1] >> 31 - j) & 1;
		}
	}

	if (val->sign == -1) str++[0] = '-';

	char ksh[10];
	sprintf(ksh, "%d", temp_integer->nums[temp_integer->actual_size - 1]);
	memcpy(str, ksh, strlen(ksh));
	str += strlen(ksh);
	for (size_t i = 1; i < temp_integer->actual_size; i++) {
		sprintf(str, "%09d", temp_integer->nums[temp_integer->actual_size - i - 1]);
		str += 9;
	}
	str[0] = '\0';

	ReleaseMutableBigInteger(temp_integer);
}
