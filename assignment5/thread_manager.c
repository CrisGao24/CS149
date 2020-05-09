#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

// Include ListNode and THREAD_DATA structs
#include "ThreadData.h"
#include "ListNode.h"


// Thread mutex lock for access to the log index
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;
// Thread mutex lock for critical sections of allocating THREADDATA
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;

// pthread, start routine, and THREADDATA object pointers
pthread_t tid1, tid2;
void* thread_runner(void*);
THREAD_DATA* p = NULL;

// Variable for indexing of messages by the logigng function
int logIndex = 0;
int *logIndexPtr = &logIndex;

// Flag to indicate if the reading of input is complete, so the other thread knows when to stop (turn this into a cond. var)
bool is_reading_complete = false;


/* Function main creates 2 threads and waits for them to finish */ 
int main() {
	
	printf("Create First Thread");
	pthread_create(&tid1, NULL, thread_runner, NULL);
	
	printf("Create Second Thread");
	pthread_create(&tid2, NULL, thread_runner, NULL);

	printf("Wait for the First Thread to exit");
	pthread_join(tid1, NULL);
	printf("First Thread exited!");

	printf("Wait for the Second Thread to exit");
	pthread_join(tid2, NULL);
	printf("Second Thread exited!");

	exit(0);
}


/* Function thread_runner runs inside each thread */
void* thread_runner(void* x) {

	// Initializing the linked list head, currnode, prevNode pointers for thread_runner (shares the call stack)
	ListNode* headNode = (ListNode*) malloc(sizeof(ListNode));
	free(headNode);
	ListNode* currNode = NULL;
	
	// For standard input
	char buffer[100];
	char* input;

	pthread_t currThread;
	currThrad = pthread_self();
	printf("This is thread %ld (p=%p)", currThread, p);
	

	// CRITICAL SECTION: allocating memory for THREAD_DATA object
	pthread_mutex_lock(&tlock2);
	if(p == NULL) {
		p = (THREAD_DATA*) malloc(sizeof(THREAD_DATA));
		p->creator = currThread;
	}
	pthread_mutex_unlock(&tlock2);

	
	// Thread 1 (creator): creating a linked list that stores contents from stdin
	if(p != NULL && p->creator == currThread) {
		printf("This is thread %ld and I created the THREAD_DATA %p", currThread, p);

		// Continue to read from stdin until Ctrl+C or '\n'
		while((input = fgets(buffer, 100, stdin)) != NULL) {
			if(*input == '\n') break;

			// Building the linked list (prepending to head for quicker access times in Thread 2)
			currNode = (ListNode*) malloc(sizeof(ListNode));
			CreateListNode(currNode, input, headNode);
			headNode = currNode;
		}
	} 
	// Thread 2 (sleeper): prints the head of the linkedlist
	else {
		printf("This is thead %ld and I can access the THREAD_DATA %p", currThread, p);		
	}


	// CRITICAL SECTION: deallocating memory from the THREAD_DATA object
	pthread_mutex_lock(&tlock2);
	if(p != NULL && p->creator == currThread) {
		printf("This is thread %ld and I did not touch THREAD_DATA object", currThread);
	} else {
		free(p);
		p = NULL;
		printf("This is thread %ld and I deleted the THREAD_DATA object", currThread);
	}
	pthread_mutex_unlock(&tlock2);
	

	pthread_exit(NULL);
	return NULL;
}
