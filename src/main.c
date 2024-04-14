#include <stdio.h>
#include <time.h>

#include "expression_parser.h"

int main(int argc, char** argv) {

	int dec_len = 300; // 기본적으로 소수점 아래 300자까지 출력
	if (argc > 1) {
		dec_len = atoi(argv[1]); // 매개변수 넘겨 받으면 그만큼 출력
	}

	LinkedList* queue = NewLinkedList(0);

	ExpressionParser* parser = NewExpressionParser();

	while (1) { // 문자열 하나씩 링크드 리스트에 받아서
		char temp;
		int e = scanf("%c", &temp);
		if (e == -1) break;
		if (temp == '\n') {
			break;
		}
		LinkedListPushItem(queue, (void*) (size_t) temp);
	}

	char* buffer = malloc(sizeof(char) * (queue->size + 1)); // 링크드 리스트 길이 + 1(\0)만큼 문자배열 동적할당
	int i = 0;
	while(LinkedListHasNext(queue)) {
		buffer[i++] = (size_t) LinkedListPopItem(queue) & 0xFF;
	}
	buffer[i] = '\0'; // 문자열 끝 표시
	ReleaseLinkedList(queue);

	ExpressionParserEvaluate(parser, buffer);
	free(buffer);
	BigRationalNumber* result = ExpressionParserGetResult(parser);
	if (!result) {
		printf("Not evaluated.\n"); // 연산 실패시
	} else {
		dec_len = (dec_len > result->denominator->size * 10 + 1) ? dec_len : result->denominator->size * 10 + 1; // 소수점 아래 값이 기본 길이보다 길면 거기까지 출력
		BigInteger* bir = BigIntegerDivide(result->numerator, result->denominator);
		buffer = malloc(sizeof(char) * ((bir->size * 12 / 10 + 2) * 9 + dec_len + 5)); // 최대한 메모리 낭비가 없도록 출력버퍼 크기 계산
		ReleaseBigInteger(bir);
		BigRationalNumberToString(result, buffer, dec_len);
		printf("%s\n", buffer);
		free(buffer);
	}

	ReleaseExpressionParser(parser);

	return 0;
}
