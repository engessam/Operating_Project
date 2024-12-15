#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <stdio.h>
#include <stdlib.h>
// typedef short bool;
// #define true 1
// #define false 0

/***************************
 *                                   Process                                   *
 * ***************************/
struct process_input_data
{
    int id;
    int arrivalTime;
    int runTime;
    int priority;
    // int mem;nooooooooo
    // int dum; noooooo
    // int dum2;noooooooo
    //  int dum3;noooooooooo
};
/***************************
 *                                        PCB Table                             *
 ***************************/
struct PCB
{
    int id;
    int pid;
    char*state;
    int arrivalTime;
    int runTime;
    int priority;
    int waitingTime;
    int remainingTime;
    int startTime;
    int endTime;
    int stoppedTime;
    // int start;
    // int waitingst;
    // int runningst;
    //// int stoppedst;
    // int memory;

    // int startmem;
    // int endmem;
};

/***************************
 *                                   PCB Functions                             *
 * ***************************/
// void setPCB(struct PCB *pcb, int id, int pid, int arrivalTime, int runTime, int priority, int waitingTime, int remainingTime, int startTime, int endTime, int stoppedTime){
//     pcb->id = id;
//     pcb->pid = pid;
//     pcb->arrivalTime = arrivalTime;
//     pcb->runTime = runTime;
//     pcb->priority = priority;
//     pcb->waitingTime = waitingTime;
//     pcb->remainingTime = remainingTime;
//     pcb->startTime = startTime;
//     pcb->endTime = endTime;
//     pcb->stoppedTime = stoppedTime;
// }

void setPCB(struct PCB *pcb, int id, int arrivalTime, int runTime, int priority, int waitingTime, int remainingTime, int startTime, int endTime, int stoppedTime)
{
    pcb->id = id;
    pcb->arrivalTime = arrivalTime;
    pcb->runTime = runTime;
    pcb->priority = priority;
    pcb->waitingTime = waitingTime;
    pcb->remainingTime = remainingTime;
    pcb->startTime = startTime;
    pcb->endTime = endTime;
    pcb->stoppedTime = stoppedTime;
}

/***************************
 *                                  Queue                                    *
 * ***************************/
typedef struct node
{
    struct PCB *pcb;
    struct node *next;
    // int priority;
} Node;

struct node *front = NULL;
struct node *rear = NULL;
struct node *current = NULL;

// insert node at the end of the list
void Enqueue(struct PCB *data)
{
    // Allocate memory for new node;
    struct node *link = (struct node *)malloc(sizeof(struct node));

    // insert the data to the new node and make the next point to null
    link->pcb = data;
    link->next = NULL;

    // If front is empty(empty queue), create new list
    if (front == NULL)
    {
        front = link;
        rear = link;
        return;
    }

    // make the last node in the old list point to the new node
    rear->next = link;

    // move the rear to the new last node
    rear = link;
}

// remove node from the front(head) of the list
void Dequeue(void)
{
    // point to the front(head) using the deleted_node
    struct node *deleted_node = front;

    // check if the Queue is empty
    if (front == NULL)
    {
        return;
    }

    // make the the front(head) point to the next node
    front = front->next;

    // remove the current front(head) node
    free(deleted_node);
}

// read the value from the front(head) node
struct PCB *Peek(void)
{
    // read the front(head) value
    return front->pcb;
}

// display the list
void printList(void)
{
    struct node *ptr = front;

    printf("[Front] =>");

    // start looping from the front(head) node
    while (ptr != NULL)
    {
        printf(" %d =>", ptr->pcb->id);
        ptr = ptr->next;
    }

    printf(" [null]\n");
}
/***************************
 *                                  Queue                                    *
 * ***************************/
/***************************
 *                                  linked_list_functions                                    *
 * ***************************/

struct node *head = NULL;
struct node *prev = NULL;

// Create Linked List
void insertAtLast(struct PCB *data)
{
    // Allocate memory for new node
    struct node *link = (struct node *)malloc(sizeof(struct node));

    // insert the data to the new node and make the next point to null
    link->pcb = data;
    link->next = NULL;

    // If head is empty, create new list
    if (head == NULL)
    {
        head = link;
        // printf("Head  %p\n",head);
        return;
    }

    current = head;

    // loop until reach the last node in the old list
    while (current->next != NULL)
    {
        current = current->next;
    }

    // Make the last node in the old list point to new node
    current->next = link;
}

// display the list
void printListlinked_list(void)
{
    struct node *ptr = head;

    printf("[head] =>");

    // start looping from the head until the last node
    while (ptr != NULL)
    {
        printf(" %d =>", ptr->pcb->id);
        ptr = ptr->next;
    }

    printf(" [null]\n");
}

void removeNodeFromTheList(struct PCB *data)
{
    int pos = 0;
    struct node *delete_node = NULL;

    // check if the linked list is empty
    if (head == NULL)
    {
        printf("Linked List is empty");
        return;
    }

    // check the first node in the linked list
    if (head->pcb->id == data->id)
    {
        delete_node = head;
        // move head to next node
        head = head->next;
        printf("%d found at position %d and deleted \n", data->id, pos);
        free(delete_node);
        return;
    }

    // check the rest of nodes in the linked list
    pos++;
    current = head->next;
    prev = head;
    while (current != NULL)
    {
        if (head->pcb->id == data->id)
        {
            // element found
            delete_node = current;
            prev->next = current->next;
            printf("%d found at position %d and deleted \n", data->id, pos);
            free(delete_node);
            return;
        }
        prev = current;
        current = current->next;
        pos++;
    }

    // in case the data not found in the linked list
    if (delete_node == NULL)
    {
        printf("%d not found in the List.", data->id);
    }
}

/***************************
 *                                   Node Functions                            *
 * ***************************/
// Node *createNode(struct PCB *pcb, int priority)
// {
//     Node *node = (Node *)malloc(sizeof(Node));
//     node->pcb = pcb;
//     node->priority = priority;
//     node->next = NULL;
//     return node;
// }

// struct PCB *getPCB(Node *node)
// {
//     return node->pcb;
// }
// /*peek pcb */
// struct PCB *peek(Node **head)
// {
//     return (*head)->pcb;
// } // end peek
// /pop pcb/
// struct PCB *pop(Node **head)
// {
//     Node *temp = *head;
//     (*head) = (*head)->next;
//     struct PCB *pcb = temp->pcb;
//     free(temp);
//     return pcb;
// } // end pop
// /push pcb/
// void push(Node **head, struct PCB *pcb, int priorityno)
// {
//     Node* start = (*head);
//     if (start == NULL)
//     {
//         (*head) = createNode(pcb, priorityno);
//         return;
//     }
//     Node *temp = createNode(pcb, priorityno);
//     if ((*head)->priority > priorityno)
//     {
//         temp->next = *head;
//         (*head) = temp;
//     }
//     else
//     {
//         while (start->next != NULL && start->next->priority <= priorityno) /find the right position to insert/

//         {
//             start = start->next;
//         }
//         temp->next = start->next;
//         start->next = temp;
//     }
// }// end push
// /isEmpty/
// int isEmpty(Node **head)
// {
//     return (*head) == NULL;
// } // end isEmpty
// /printQueue/
// void printQueue(Node **head)
// {
//     Node *temp = *head;
//     while (temp != NULL)
//     {
//         printf("%d ", temp->pcb->id);
//         temp = temp->next;
//     }
//     printf("\n");
// } // end printQueue
// /getQueueSize/
// int getQueueSize(Node **head)
// {
//     Node *temp = *head;
//     int size = 0;
//     while (temp != NULL)
//     {
//         size++;
//         temp = temp->next;
//     }
//     return size;
// } // end getQueueSize
// /bool check_item/
// bool check_item(Node **head, int id)
// {
//     Node *temp = *head;
//     while (temp != NULL)
//     {
//         if (temp->pcb->id == id)
//         {
//             return true;
//         }
//         temp = temp->next;
//     }
//     return false;
// } // end check_item

// /***************************
//  *                                priority Queue                                     *
//  * ***************************/
//***************************
//*                                priority Queue Functions                      *
//* ***************************/
// Define a simple queue for PCBs
typedef struct Queue
{
    struct node *front;
    struct node *rear;
    // int priority;
} Queue;

struct node *current_queue;
typedef struct NodePriority
{
    struct Queue *queue;
} NodePriority;
struct NodePriority prio[11];
void intializeQueue()
{
    for (int i = 0; i < 11; i++)
    {
        prio[i].queue = (Queue *)malloc(sizeof(Queue));
        prio[i].queue->front = NULL;
        prio[i].queue->rear = NULL;
    }
}
void enqueue(struct PCB *data,int priority)
{
    // Allocate memory for new node;
    struct node *link = (struct node *)malloc(sizeof(struct node));
    // insert the data to the new node and make the next point to null
    link->pcb = data;
    link->next = NULL;

    if (prio[priority].queue->front == NULL)
    {
        prio[priority].queue->front = link;
        prio[priority].queue->rear = link;
        return;
    }

    // make the last node in the old list point to the new node
    prio[priority].queue->rear->next = link;

    // move the rear to the new last node
    prio[priority].queue->rear = link;
}

// remove node from the front(head) of the list
void dequeue(struct PCB *data,int priority)
{
    // point to the front(head) using the deleted_node
    struct node *deleted_node = prio[priority].queue->front;

    // check if the Queue is empty
    if (prio[priority].queue->front == NULL)
    {
        return;
    }

    // make the the front(head) point to the next node
    prio[priority].queue->front = prio[priority].queue->front->next;

    // remove the current front(head) node
    free(deleted_node);
}

// display the list
void PrintQueue(void)
{
    for (int i = 0; i < 11; i++)
    {
        struct node *ptr = prio[i].queue->front;

        printf("[Front number %d] =>", i);

        // start looping from the front(head) node
        while (ptr != NULL)
        {
            printf(" %d =>", ptr->pcb->id);
            ptr = ptr->next;
        }

        printf(" [null]\n");
    }
    
}
// /***************************
//  *                                priority Queue                                     *
//  * ***************************/
//***************************
typedef struct Node_P {
    struct PCB *pcb;           // Data to store
    int priority;       // Priority of the data
    struct Node_P* next;  // Pointer to the next node
} Node_P;



struct Node_P * pq_front = NULL;
struct Node_P * current_ptr = NULL;
Node_P* createNode(struct PCB *data, int priority) {
    Node_P* newNode = (Node_P*)malloc(sizeof(Node_P));
    newNode->pcb = data;
    newNode->priority = priority;
    newNode->next = NULL;
    return newNode;
}
void enqueueprio( struct PCB * data, int priority) {
    Node_P* newNode = createNode(data, priority);

    // If the queue is empty or the new node has the highest priority
    if (pq_front== NULL || priority < pq_front->priority) {
        newNode->next = pq_front;
        pq_front = newNode;
    } else {
        // Traverse the queue to find the correct position
        Node_P* current = pq_front;
        while (current->next != NULL && current->next->priority <= priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    //printf("Enqueued: %d with priority %d\n", data, priority);
}
struct PCB* dequeueprio() 
{
    if (pq_front == NULL) {
        printf("Queue is empty. Cannot dequeue.\n");
        return NULL;
    }
    struct PCB * pcb;
    Node_P* temp = pq_front;
    pq_front = pq_front->next;
    pcb = temp->pcb;
    //printf("Dequeued: %d with priority %d\n", temp->data, temp->priority);
    free(temp);
    return pcb;
}

void printprioQueue() {
    if (pq_front == NULL) {
        printf("Queue is empty.\n");
        return;
    }

    printf("Priority Queue:\n");
    Node_P* current = pq_front;
    while (current != NULL) {
        printf("Data: %d | Priority: %d\n", current->pcb->id, current->priority);
        current = current->next;
    }
}
//*                                priority Queue Functions                      *
//* ***************************/
// Define a simple queue for PCBs

#endif