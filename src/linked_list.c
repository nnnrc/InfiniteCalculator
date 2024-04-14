#include "linked_list.h"

void* NodeGetItem(Node* node)
{
	return node->item;
}

Node* NewNode(Node* next, void* item)
{
	Node* result = malloc(sizeof(Node));
	result->next = next;
	result->item = item;
	return result;
}

void ReleaseNode(Node* node)
{
	if (node) free(node);
}

void LinkedListPushItem(LinkedList* list, void* item)
{
	if (list->stack) {
		Node* node = NewNode(list->last, item);
		list->last = node;
	} else {
		Node* node = NewNode(NULL, item);
		if (!list->last) list->last = node;
		else {
			Node* temp = list->last;
			while (temp->next != NULL) {
				temp = temp->next;
			}
			temp->next = node;
		}
	}
	list->size++;
}

void* LinkedListPopItem(LinkedList* list)
{
	if (list->size == 0) return NULL;
	Node* node = list->last;
	void* result = NodeGetItem(node);
	list->last = node->next;
	list->size--;
	ReleaseNode(node);

	return result;
}

void* LinkedListPeekItem(LinkedList* list)
{
	return NodeGetItem(list->last);
}

int LinkedListHasNext(LinkedList* list)
{
	return list->size > 0;
}

LinkedList* NewLinkedList(int stack)
{
	LinkedList* result = malloc(sizeof(LinkedList));
	result->last = NULL;
	result->size = 0;
	result->stack = stack;
	return result;
}

void ReleaseLinkedList(LinkedList* list)
{
	if (!list) return;
	while (LinkedListHasNext(list)) {
		LinkedListPopItem(list);
	}
	free(list);
}
