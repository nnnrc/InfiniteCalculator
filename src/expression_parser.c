#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression_parser.h"

void ExpressionParserEvaluate(ExpressionParser* parser, char* str)
{
	if (parser->result) ReleaseBigRationalNumber(parser->result);
	parser->result = NULL;
	size_t str_length = strlen(str);

	int add = 0, sub = 0, mul = 1, div = 1, o = 2;
	int* arr[58];
	arr['('] = &o;
	arr['+'] = &add;
	arr['-'] = &sub;
	arr['*'] = &mul;
	arr['/'] = &div;

	LinkedList* queue = NewLinkedList(0);
	LinkedList* stack = NewLinkedList(1);
	char* buffer = malloc(sizeof(char) * 1000000);
	int pre_opr = 0;
	for (size_t i = 0; i < str_length; i++) {
		if (str[i] == ' ') continue;
		if ((str[i] >= '-' && str[i] <= '9') || (str[i] >= '(' && str[i] <= '+')) {
			if ((str[i] >= '0' && str[i] <= '9') || str[i] == '.' || (pre_opr == '(' && str[i] == '-')) {
				if (pre_opr == ')') {
					while (LinkedListHasNext(stack) && *((int*)LinkedListPeekItem(stack)) >= *arr['*'] && LinkedListPeekItem(stack) != &o) {
						LinkedListPushItem(queue, LinkedListPopItem(stack));
					}
					LinkedListPushItem(stack, arr['*']);
				}
				buffer[0] = str[i];
				int is_dec = 0;
				int ff = 0;
				for (int j = 1; ; j++) {
					if (!((str[i + j] >= '0' && str[i + j] <= '9') || str[i + j] == '.')) {
						i += j - 1;
						buffer[j] = '\0';
						if (strlen(buffer) == 1 && buffer[0] == '-') {
							LinkedListPushItem(queue, NewBigRationalNumber("0"));
							while (LinkedListHasNext(stack) && *((int*)LinkedListPeekItem(stack)) >= *arr['-'] && LinkedListPeekItem(stack) != &o) {
								LinkedListPushItem(queue, LinkedListPopItem(stack));
							}
							LinkedListPushItem(stack, arr['-']);
							ff = 1;
							break;;
						}
						break;
					}
					if (str[i + j] == '.') {
						if (is_dec) {
							printf("Error : There are two or more decimal points.\n");
							goto END;
						}
						is_dec = 1;
					}
					buffer[j] = str[i + j];
				}
				if (ff) continue;
				BigRationalNumber* t = NewBigRationalNumber(buffer);
				if (!t) {
					printf("Error : Unknown error.\n");
					goto END;
				}
				LinkedListPushItem(queue, t);
				pre_opr = 0;
			}
			else {
				if (pre_opr && pre_opr != ')' && str[i] != '(') {
					printf("Error : Incorrect use of operator.\n");
					goto END;
				}
				if (!pre_opr && !LinkedListHasNext(queue) && str[i] == '-') {
					LinkedListPushItem(queue, NewBigRationalNumber("0"));
				}
				if (str[i] == ')') {
					while (1) {
						if (!LinkedListHasNext(stack)) {
							printf("Error : Incorrect number of parentheses.\n");
							goto END;
						}
						int* opr = LinkedListPopItem(stack);
						if (opr == &o) break;
						LinkedListPushItem(queue, opr);
					}
				}
				else {
					if (str[i] == '(' && ((!pre_opr && LinkedListHasNext(queue)) || pre_opr == ')')) {
						while (LinkedListHasNext(stack) && *((int*)LinkedListPeekItem(stack)) >= *arr['*'] && LinkedListPeekItem(stack) != &o) {
							LinkedListPushItem(queue, LinkedListPopItem(stack));
						}
						LinkedListPushItem(stack, arr['*']);
					}
					while (LinkedListHasNext(stack) && *((int*)LinkedListPeekItem(stack)) >= *arr[str[i]] && LinkedListPeekItem(stack) != &o) {
						LinkedListPushItem(queue, LinkedListPopItem(stack));
					}
					LinkedListPushItem(stack, arr[str[i]]);
				}
				pre_opr = str[i];
			}
		}
		else {
			printf("Error : Unexpected character : %c\n", str[i]);
			goto END;
		}
	}

	while (LinkedListHasNext(stack)) {
		LinkedListPushItem(queue, LinkedListPopItem(stack));
	}

	while (LinkedListHasNext(queue)) {
		void* item = LinkedListPopItem(queue);
		if (item == &o) {
			printf("Error : Incorrect number of parentheses.\n");
			goto END;
		}
		if (!(item == &add || item == &sub || item == &mul || item == &div)) {
			LinkedListPushItem(stack, item);
			continue;
		}
		BigRationalNumber* val2 = LinkedListPopItem(stack);
		BigRationalNumber* val1 = LinkedListPopItem(stack);
		if (!val1 || !val2) {
			if (val1) ReleaseBigRationalNumber(val1);
			if (val2) ReleaseBigRationalNumber(val2);
			printf("Error : Too few operands.\n");
			goto END;
		}
		BigRationalNumber* result = NULL;
		if (item == &add) result = BigRationalNumberAdd(val1, val2);
		else if (item == &sub) result = BigRationalNumberSubtract(val1, val2);
		else if (item == &mul) result = BigRationalNumberMultiply(val1, val2);
		else if (item == &div) { 
			if (val2->sign == 0) {
				ReleaseBigRationalNumber(val1);
				ReleaseBigRationalNumber(val2);
				printf("Error : Division by 0.\n");
				goto END;
			}
			result = BigRationalNumberDivide(val1, val2);
		}
		if (!result) {
			printf("Error : Calculation error.\n");
			goto END;
		}

		ReleaseBigRationalNumber(val1);
		ReleaseBigRationalNumber(val2);

		LinkedListPushItem(stack, result);
	}

	if (stack->size > 1) {
		printf("Error : Too many operands.\n");
		goto END;
	}

	parser->result = LinkedListPopItem(stack);

END:
	while (LinkedListHasNext(queue)) {
		void* item = LinkedListPopItem(queue);
		if(!(item == &add || item == &sub || item == &mul || item == &div || item == &o))
			ReleaseBigRationalNumber(item);
	}

	while (LinkedListHasNext(stack)) {
		void* item = LinkedListPopItem(stack);
		if (!(item == &add || item == &sub || item == &mul || item == &div || item == &o))
			ReleaseBigRationalNumber(item);
	}

	ReleaseLinkedList(queue);
	ReleaseLinkedList(stack);
	free(buffer);
}

BigRationalNumber* ExpressionParserGetResult(ExpressionParser* parser)
{
	return parser->result;
}

ExpressionParser* NewExpressionParser()
{
	ExpressionParser* result = malloc(sizeof(ExpressionParser));
	result->result = NULL;
	return result;
}

void ReleaseExpressionParser(ExpressionParser* parser)
{
	if (!parser) return;
	if (parser->result) ReleaseBigRationalNumber(parser->result);
	free(parser);
}
