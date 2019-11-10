// ThreadTree.c ... implementation of Tree-of-Mail-Threads ADT
// Written by John Shepherd, Feb 2019

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "MMList.h"
#include "MMTree.h"
#include "MailMessage.h"
#include "ThreadTree.h"

// Representation of ThreadTree's

typedef struct ThreadTreeNode *Link;

typedef struct ThreadTreeNode {
	MailMessage mesg;
	Link next;
	Link replies;
} ThreadTreeNode;

typedef struct ThreadTreeRep {
	Link messages;
} ThreadTreeRep;

// Auxiliary data structures and functions

// Add any new data structures and functions here ...

static void doDropThreadTree (Link t);
static void doShowThreadTree (Link t, int level);

// END auxiliary data structures and functions

// create a new empty ThreadTree
ThreadTree newThreadTree (void)
{
	ThreadTreeRep *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate ThreadTree");
	*new = (ThreadTreeRep) { };
	return new;
}

void dropThreadTree (ThreadTree tt)
{
	assert (tt != NULL);
	doDropThreadTree (tt->messages);
}

// free up memory associated with list
static void doDropThreadTree (Link t)
{
	if (t == NULL)
		return;

	for (Link curr = t, next; curr != NULL; curr = next) {
		next = curr->next;
		doDropThreadTree (curr->replies);
		// don't drop curr->mesg, in case referenced elsehwere
		free (curr);
	}
}

void showThreadTree (ThreadTree tt)
{
	assert (tt != NULL);
	doShowThreadTree (tt->messages, 0);
}

// display thread tree as hiearchical list
static void doShowThreadTree (Link t, int level)
{
	if (t == NULL)
		return;
	for (Link curr = t; curr != NULL; curr = curr->next) {
		showMailMessage (curr->mesg, level);
		doShowThreadTree (curr->replies, level + 1);
	}
}

// insert mail message into ThreadTree
// if a reply, insert in appropriate replies list
// whichever list inserted, must be in timestamp-order
struct stackNode{
    char *id;
    struct stackNode * next;
};
struct stackNode *stackPush(struct stackNode * n, char *id){
    struct stackNode *tmp = malloc(sizeof(struct stackNode));
    tmp->id = id;
    tmp->next = n;
    return tmp;
}
struct stackNode *stackPop(struct stackNode * n){
    if (n == NULL) return NULL;
    struct stackNode *tmp = n->next;
    free(n);
    return tmp;
}
int isEmptyStack(struct stackNode * n){
    return n == NULL;
}
Link recInsert(Link root, struct stackNode *s, Link nodeInsert){
    if (isEmptyStack(s)) return nodeInsert;
    if (strcmp(MailMessageID(root), s->stackNode->id)){
        s = stackPop(s);
        root->replies = recInsert(root->replies, s, nodeInsert);
    } else {
        root->next = recInsert(root->replies, s, nodeInsert);
    }
    return root;
}
ThreadTree ThreadTreeBuild (MMList mesgs, MMTree msgids)
{
	// You need to implement this
    ThreadTree resTree = newThreadTree();
    MMListStart (mesgs);
    MailMessage tmpMesg;
    MailMessage tmpTreeMesg;
    struct stackNode *nodeStack = NULL;
    Link mainLink = NULL;
    Link tmpInsertLink;
    Link preInsertLink;
	int i = 0;
    while(!MMListEnd (mesgs)){
        tmpMesg = MMListNext (mesgs);
        if (MailMessageRepliesTo(tmpMesg) == NULL){
            if (resTree->messages == NULL){
                resTree->messages = malloc(sizeof(ThreadTreeNode));
                resTree->messages->mesg = tmpMesg;
                resTree->messages->next = NULL;
                resTree->messages->replies = NULL;
                mainLink = resTree->messages;
            } else {
                mainLink->next = malloc(sizeof(ThreadTreeNode));
                mainLink->next->mesg = tmpMesg;
                mainLink->next->next = NULL;
                mainLink->next->replies = NULL;
                mainLink = mainLink->next;
            }
        } else {
            tmpTreeMesg = tmpMesg;
            do {
                tmpTreeMesg = MMTreeFind(msgids, MailMessageRepliesTo(tmpTreeMesg));
                nodeStack = stackPush(nodeStack, MailMessageID(tmpTreeMesg));
                
            } while(MailMessageRepliesTo(tmpTreeMesg) != NULL);
            /*
            tmpTreeMesg = MMTreeFind(msgids, MailMessageRepliesTo(tmpTreeMesg));
            nodeStack = stackPush(nodeStack, MailMessageID(tmpTreeMesg));
            while(MailMessageRepliesTo(tmpTreeMesg) != NULL){
                tmpTreeMesg = MMTreeFind(msgids, MailMessageRepliesTo(tmpTreeMesg));
                nodeStack = stackPush(nodeStack, MailMessageID(tmpTreeMesg));
            }
            */
            tmpInsertLink = resTree->messages;
            preInsertLink = NULL;
		    //printf("pass Do-While\n");
		    //getchar();
            while(!isEmptyStack(nodeStack)){
                if (strcmp(MailMessageID(tmpInsertLink->mesg), nodeStack->id) == 0){
                    preInsertLink = tmpInsertLink;
                    tmpInsertLink = tmpInsertLink->replies;
                    nodeStack = stackPop(nodeStack);
                } else {
                    preInsertLink = tmpInsertLink;
                    tmpInsertLink = tmpInsertLink->next;
                }
            }
            if (tmpInsertLink == NULL){
                preInsertLink->replies = malloc(sizeof(ThreadTreeNode));
                preInsertLink->replies->mesg = tmpMesg;
                preInsertLink->replies->next = NULL;
                preInsertLink->replies->replies = NULL;
            } else {
                while(tmpInsertLink != NULL){
                    preInsertLink = tmpInsertLink;
                    tmpInsertLink = tmpInsertLink->next;
                }
                preInsertLink->next = malloc(sizeof(ThreadTreeNode));
                preInsertLink->next->mesg = tmpMesg;
                preInsertLink->next->next = NULL;
                preInsertLink->next->replies = NULL;
            }
            
        }
    }
	return resTree; // change this line
}
