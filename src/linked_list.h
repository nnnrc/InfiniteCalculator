#pragma once

#include <stdlib.h>

/*
 연결리스트의 한 값을 저장하는 타입
*/

typedef struct Node {
	struct Node* next; // 다음 노드
	void* item;        // 실제 값
} Node;

/*
 스택이나 큐로 사용할 수 있는 연결리스트
*/

typedef struct {
	Node* last;  // 마지막(Pop하면 나오는) 값을 담고 있는 노드
	size_t size; // 노드 개수
	int stack;   // 0(false)인 경우 큐, 1(true)인 경우 스택
} LinkedList;

void* NodeGetItem(Node*);

// 새로운 Node 객체를 만드는 함수, 다음 Node와 값을 받아서 저장장
Node* NewNode(Node*, void*);
// Node 객체의 메모리 할당 해제
void ReleaseNode(Node*);

// LinkedList에 값을 넣음
void LinkedListPushItem(LinkedList*, void*);
// LinkedList 값을 뺌
void* LinkedListPopItem(LinkedList*);
// Pop을 하면 나올 값을 빼지 않고 확인만 함
void* LinkedListPeekItem(LinkedList*);
// LinkedList가 비어있지 않으면 1, 비어있으면 0을 리턴
int LinkedListHasNext(LinkedList*);

// 새로운 LinkedList 객체를 만드는 함수, 매개변수가 0(false)인 경우 큐, 1(true)인 경우 스택
LinkedList* NewLinkedList(int);
// LinkedList 객체의 메모리 할당을 해제하는 함수
void ReleaseLinkedList(LinkedList*);
