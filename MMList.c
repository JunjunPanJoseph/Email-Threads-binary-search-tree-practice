// MMList.c ... implementation of List-of-Mail-Messages ADT
// Written by John Shepherd, Feb 2019

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "MMList.h"
#include "MailMessage.h"

// data structures representing MMList

typedef struct MMListNode *Link;

typedef struct MMListNode {
	MailMessage data; // message associated with this list item
	Link next;		  // pointer to node containing next element
} MMListNode;

typedef struct MMListRep {
	Link first; // node containing first value
	Link last;  // node containing last value
	Link curr;  // current node (for iteration)
} MMListRep;

static Link newMMListNode (MailMessage mesg);

// create a new empty MMList
MMList newMMList (void)
{
	MMListRep *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate MMList");
	*new = (MMListRep) { };
	return new;
}

// free up memory associated with list
// note: does not remove Mail Messages
void dropMMList (MMList L)
{
	assert (L != NULL);
	Link curr = L->first;
	while (curr != NULL) {
		Link next = curr->next;
		free (curr);
		curr = next;
	}
	free (L);
}

// display list as one integer per line on stdout
void showMMList (MMList L)
{
	assert (L != NULL);
	for (Link curr = L->first; curr != NULL; curr = curr->next)
		showMailMessage (curr->data, 0);
}

// insert mail message in order
// ordering based on MailMessageDateTime
void MMListInsert (MMList L, MailMessage mesg)
{
	//printf("111\n");
	assert (L != NULL);
	assert (MMListIsOrdered (L));

	assert (mesg != NULL);
    Link newNode = malloc(sizeof(struct MMListNode));
    newNode->next = NULL;
    newNode->data = mesg;
	if (L->first == NULL){
        
        L->last = newNode;
        L->first = newNode;
        L->curr = newNode;    
        //printf("Empty\n");
        return;
    }
    DateTime currDate = MailMessageDateTime (mesg);
    DateTime tmpDate = MailMessageDateTime (L->first->data);
    if (DateTimeBefore(currDate, tmpDate)){
        newNode->next = L->first;
        L->first = newNode;
        //printf("First\n");
        return;
    }
    Link preNode = L->first;
    Link tmpNode = L->first->next;
    while (tmpNode != NULL){
        tmpDate = MailMessageDateTime (tmpNode->data);
        if (DateTimeBefore(currDate, tmpDate)){
            newNode->next = tmpNode;
            preNode->next = newNode;
            //printf("Mid\n");
            return;
        }
        preNode = preNode->next;
        tmpNode = tmpNode->next;
    }
    L->last->next = newNode;
    L->last = newNode;
    //printf("Last\n");
    return;
}

// create a new MMListNode for mail message
// (this function is used only within the ADT)
static Link newMMListNode (MailMessage mesg)
{
	Link new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate MMList node");
	*new = (MMListNode) { .data = mesg };
	return new;
}

// check whether a list is ordered (by MailMessageDate)
bool MMListIsOrdered (MMList L)
{
	DateTime prevDate = NULL;
	for (Link n = L->first; n != NULL; n = n->next) {
		DateTime currDate = MailMessageDateTime (n->data);
		if (prevDate != NULL && DateTimeAfter (prevDate, currDate))
			return false;
		prevDate = currDate;
	}
	return true;
}

// start scan of an MMList
void MMListStart (MMList L)
{
	assert (L != NULL);
	L->curr = L->first;
}

// get next item during scan of an MMList
MailMessage MMListNext (MMList L)
{
	assert (L != NULL);
	if (L->curr == NULL)
		// this is probably an error
		return NULL;

	MailMessage mesg = L->curr->data;
	L->curr = L->curr->next;
	return mesg;
}

// check whether MMList scan is complete
bool MMListEnd (MMList L)
{
	assert (L != NULL);
	return (L->curr == NULL);
}
