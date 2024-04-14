#pragma once

#include "big_rational_number.h"
#include "linked_list.h"

/*
 문자열로된 수식을 받아와서 중위표기법을 후위표기법으로 전환하여 연산하는 수식 파서
*/

typedef struct {
	BigRationalNumber* result;
} ExpressionParser;

// 문자열로된 수식을 받아와서 연산함. +, -, *, /, (, ) 사용가능
void ExpressionParserEvaluate(ExpressionParser*, char*);
// 계산된 결과(BigRationalNumber)를 리턴하는 함수
BigRationalNumber* ExpressionParserGetResult(ExpressionParser*);

// 새로운 ExpressionParser 객체를 만드는 함수
ExpressionParser* NewExpressionParser();
// ExpressionParser 객체의 메모리 할당을 해제하는 함수
void ReleaseExpressionParser(ExpressionParser*);
