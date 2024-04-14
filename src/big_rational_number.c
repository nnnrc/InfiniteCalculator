#include <stdio.h>
#include <string.h>

#include "big_rational_number.h"

/* ---------- static functions ---------- */

// 분수를 하한항으로 만드는 함수(약분)
static void simplify(BigRationalNumber* val)
{
	if (val->sign == 0) return;
	BigInteger* gcd = BigIntegerGcd(val->numerator, val->denominator); // 최대공약수 계산
	BigInteger* one_temp = NewBigIntegerFromString("1");
	if (BigIntegerCompareAbs(gcd, one_temp) != 0) { // 최대 공약수가 1이 아닌경우
		BigInteger* new_numerator = BigIntegerDivide(val->numerator, gcd); // 분자 = 분자 / 최대공약수
		BigInteger* new_denominator = BigIntegerDivide(val->denominator, gcd); // 분모 = 분모 / 최대공약수
		ReleaseBigInteger(val->numerator);
		ReleaseBigInteger(val->denominator);
		val->numerator = new_numerator;
		val->denominator = new_denominator;
	}
	ReleaseBigInteger(one_temp);
	ReleaseBigInteger(gcd);
}

// 부호 확인
static void check_sign(BigRationalNumber* val)
{
	if (val->denominator->sign == 0) { // 분모가 0인경우
		printf("*** check_sign : Division by 0.\n");
		exit(133);
	}
	
	if (val->numerator->sign == 0) { // 분자가 0인경우
		val->sign = 0; // 부호 0으로 설정
		ReleaseBigInteger(val->numerator);
		ReleaseBigInteger(val->denominator);
		val->numerator = NewBigIntegerFromString("0");
		val->denominator = NewBigIntegerFromString("1"); // 0 / 1 꼴로 나타냄
		return;
	}

	val->sign = (val->numerator->sign == val->denominator->sign) ? 1 : -1; // 부호 설정
	val->numerator->sign = 1;
	val->denominator->sign = 1;
}

/* -------------------------------------- */

BigRationalNumber* BigRationalNumberAdd(BigRationalNumber* val1, BigRationalNumber* val2)
{
	// 통분 과정
	// a / b + c / d = (ad + bc) / bd
	BigInteger* val1_numerator = BigIntegerMultiply(val1->numerator, val2->denominator); // ad
	BigInteger* val2_numerator = BigIntegerMultiply(val2->numerator, val1->denominator);// bc
	val1_numerator->sign = val1->sign; // BigInteger 연산을 사용하기 위해 분자에 부호 설정
	val2_numerator->sign = val2->sign;
	BigInteger* new_numerator = BigIntegerAdd(val1_numerator, val2_numerator); // ad + bc
	BigInteger* new_denominator = BigIntegerMultiply(val1->denominator, val2->denominator); // bd

	BigRationalNumber* result = NewBigRationalNumberFromBigIntegers(new_numerator, new_denominator); // (ad + bc) / bd
	ReleaseBigInteger(val1_numerator);
	ReleaseBigInteger(val2_numerator);
	ReleaseBigInteger(new_numerator);
	ReleaseBigInteger(new_denominator);

	return result;
}

BigRationalNumber* BigRationalNumberSubtract(BigRationalNumber* val1, BigRationalNumber* val2)
{
	// 통분 과정
	// a / b + c / d = (ad - bc) / bd
	BigInteger* val1_numerator = BigIntegerMultiply(val1->numerator, val2->denominator); // ad
	BigInteger* val2_numerator = BigIntegerMultiply(val2->numerator, val1->denominator); // bc
	val1_numerator->sign = val1->sign; // BigInteger 연산을 사용하기 위해 분자에 부호 설정
	val2_numerator->sign = val2->sign;
	BigInteger* new_numerator = BigIntegerSubtract(val1_numerator, val2_numerator); // ad - bc
	BigInteger* new_denominator = BigIntegerMultiply(val1->denominator, val2->denominator); // bd

	BigRationalNumber* result = NewBigRationalNumberFromBigIntegers(new_numerator, new_denominator); // (ad - bc) / bd
	ReleaseBigInteger(val1_numerator);
	ReleaseBigInteger(val2_numerator);
	ReleaseBigInteger(new_numerator);
	ReleaseBigInteger(new_denominator);

	return result;
}

BigRationalNumber* BigRationalNumberMultiply(BigRationalNumber* val1, BigRationalNumber* val2)
{
	if (val1->sign == 0 || val2->sign == 0) return NewBigRationalNumber("0"); // 피연산자중 하나라도 0이면 0 반환

	BigRationalNumber* result = malloc(sizeof(BigRationalNumber));
	result->sign = (val1->sign == val2->sign) ? 1 : -1;
	result->numerator = BigIntegerMultiply(val1->numerator, val2->numerator); // 분모, 분자끼리 곱해줌
	result->denominator = BigIntegerMultiply(val1->denominator, val2->denominator);
	simplify(result); // 약분

	return result;
}

BigRationalNumber* BigRationalNumberDivide(BigRationalNumber* val1, BigRationalNumber* val2)
{
	if (val2->sign == 0) {
		printf("*** BigRationalNumberDivide : Division by 0.\n");
		return NULL;
	}
	if (val1->sign == 0) return NewBigRationalNumber("0"); // 피연산자중 하나라도 0이면 0 반환

	BigRationalNumber* result = malloc(sizeof(BigRationalNumber));
	result->sign = (val1->sign == val2->sign) ? 1 : -1;
	result->numerator = BigIntegerMultiply(val1->numerator, val2->denominator); // val2를 뒤집어서 곱함((a / b) / (c / d) = (a / b) * (d / c))
	result->denominator = BigIntegerMultiply(val1->denominator, val2->numerator);
	simplify(result); // 약분

	return result;
}

BigRationalNumber* NewBigRationalNumber(char* str)
{
	BigRationalNumber* result = malloc(sizeof(BigRationalNumber));
	result->sign = 1;
	if (str[0] == '-') {
		result->sign = -1;
		str++;
	}

	size_t str_length = strlen(str);
	char* numerator_str = malloc(sizeof(char) * (str_length + 1));
	char* denominator_str = NULL;
	int is_decimal = 0; 
	size_t numerator_str_index = 0;
	size_t denominator_str_size = 0; // 분모 길이
	for (size_t i = 0; i < str_length; i++) {
		if ((str[i] < '0' || str[i] > '9') && str[i] != ('.')) {
			printf("*** NewBigRationalNumber : not a decimal string.\n");
			goto END1;
		}
		if (str[i] == '.') {
			if (is_decimal) { // 이미 .이 나왔는데 또 나온 경우 (ex. 12.34.3)
				printf("*** NewBigRationalNumber : not a decimal string.\n");
				goto END1;
			}
			is_decimal = 1; // 다음부터 소수점 아래 값
			continue;
		}
		numerator_str[numerator_str_index++] = str[i];
		if (is_decimal) denominator_str_size++;
	}
	numerator_str[numerator_str_index] = '\0';
	// 소수를 분수로 나타내는 로직
	// ex. 3427948.21383 -> 342794821383 / 100000
	denominator_str = malloc(sizeof(char) * (denominator_str_size + 2));
	denominator_str[0] = '1';
	denominator_str++;
	memset(denominator_str, '0', denominator_str_size);
	denominator_str[denominator_str_size] = '\0';
	denominator_str--;
	BigInteger* new_numerator = NewBigIntegerFromString(numerator_str);
	BigInteger* new_denominator = NewBigIntegerFromString(denominator_str);
	if (!new_numerator || !new_denominator) {
		if (new_numerator) ReleaseBigInteger(new_numerator);
		if (new_denominator) ReleaseBigInteger(new_denominator);
		printf("*** NewBigRationalNumber : An error occured at create instance.\n");
		goto END1;
	}
	result->numerator = new_numerator;
	result->denominator = new_denominator;

	if (new_numerator->sign == 0) check_sign(result);
	simplify(result);

	goto END2;

END1:
	ReleaseBigRationalNumber(result);
	result = NULL;
END2:
	free(numerator_str);
	if (denominator_str) free(denominator_str);

	return result;
}

BigRationalNumber* NewBigRationalNumberFromBigIntegers(BigInteger* numerator, BigInteger* denominator)
{
	if (denominator->sign == 0) {
		printf("*** NewBigRationalNumberFromBigIntegers : Division by 0.\n");
		return NULL;
	}
	BigRationalNumber* result = malloc(sizeof(BigRationalNumber));
	result->numerator = CloneBigInteger(numerator);
	result->denominator = CloneBigInteger(denominator);
	check_sign(result);
	simplify(result);
	return result;
}

void ReleaseBigRationalNumber(BigRationalNumber* val)
{
	if (!val) return;
	if (val->numerator) ReleaseBigInteger(val->numerator);
	if (val->denominator) ReleaseBigInteger(val->denominator);
	free(val);
}

void BigRationalNumberToString(BigRationalNumber* val, char* str, unsigned int decimal_length)
{
	if (val->sign == -1) {
		str[0] = '-';
		str++;
	}
	BigInteger* remainder;
	BigInteger* quotient = BigIntegerDivideAndMod(val->numerator, val->denominator, &remainder);
	if (!quotient) {
		printf("*** BigRationalNumberToString : An error occured.\n");
		return;
	}
	BigIntegerToString(quotient, str); // 정수부
	str += strlen(str); 
	str[0] = '.';
	str[1] = '\0';
	str++;
	ReleaseBigInteger(quotient);
	BigInteger* multiplier = NewBigIntegerFromString("10");
	int temp_length = 0;
	// 순환소수 이거나 decimal_length보다 소수부가 길다면 decimal_length까지 소수부 작성
	while (remainder->sign != 0 && temp_length < decimal_length) {
		BigInteger* temp = BigIntegerMultiply(remainder, multiplier); // 나머지에 10 곱해준 뒤
		ReleaseBigInteger(remainder);
		quotient = BigIntegerDivideAndMod(temp, val->denominator, &remainder); // 분모로 나눠줌, 몫은 소수부에 이어서 작성
		if (!quotient) {
			printf("*** BigRationalNumberToString : An error occured.\n");
			ReleaseBigInteger(temp);
			ReleaseBigInteger(multiplier);
			return;
		}
		BigIntegerToString(quotient, str);
		ReleaseBigInteger(temp);
		ReleaseBigInteger(quotient);
		str++;
		temp_length++;
	}
	ReleaseBigInteger(multiplier);
	ReleaseBigInteger(remainder);
	if (*(str - 1) == '.') { // 정수인경우 . 제거
		*(str - 1) = '\0';
	}
	str[0] = '\0';
}
