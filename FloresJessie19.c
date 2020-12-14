/*
Program uses linked list to randomize integers and insert into list, 
sums, counts, and calculates average of elements in list
Flores Jessie
ICS 212 Assignment 19
04/5/16
*/


#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

struct node{
   int data;
   struct node *next;
};

typedef struct node Node;
typedef struct node* NodePointer;

void randomInsert(int, NodePointer *);
int sum(NodePointer);
int count(NodePointer);


int main() {

   int num;

   /* uses time function to randomize runs */
   srand(time(NULL));

   /* determines length of list */
   num = rand() % 51 + 25;

   /* Initalizes list */ 
   NodePointer head = NULL;

   /* While we haven't reached end of list */
   while(num > 0) {

	/* Generates a random number to insert */
	int num2 = rand() % 101;	
	randomInsert(num2, &head);
	
	num--;
   }
   /* Calculate and display results */
   int result = sum(head);
   printf("sum = %d\ncount = %d\naverage = %.2f\n", result, count(head), (float)result/count(head));

  return 0;
}

/* Inserts an integer into linked list */
void randomInsert(int num, NodePointer *node) {
   /* Initalize list components */
   NodePointer newNode = NULL;
   NodePointer previous = NULL;
   NodePointer current = *node;

   /* Make node in the heap */
   newNode = malloc(sizeof(Node));

   if(NULL != newNode) {

	/* set node's data to integer given*/
	newNode->data = num;

	/* Determine where to put node in list */
	while(NULL != current && (num - current->data) > 0) {

	   previous = current;

           current = current->next;
	}
	/* If list is empty */
	if(NULL == previous) {
	
	   newNode->next = current;

           *node = newNode;
	}
	/* Otherwise insert between previous and current*/
	else {
           previous->next = newNode;
           newNode->next = current;
        }
   }
}

/* Displays list and sums integers in linked list */
int sum(NodePointer node) {

   int result = 0;
   printf("The list is: ");

   /* loop until end of list */
   while(NULL != node) {

	printf("%d ", node->data);

	/* add current node's data to current result */
	result += node->data;

	node = node->next;
   }
   return result;
}   

/* counts elements in list */	
int count(NodePointer node) {

   int result = 0;

   /* loop until end of list */
   while(NULL != node) {

	result++;

	node = node->next;
   }
   return result;
}
   

