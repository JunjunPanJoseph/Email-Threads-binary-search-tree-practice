typedef struct ThreadTreeNode *Link;

typedef struct ThreadTreeNode {
	MailMessage mesg;
	Link next, replies;
} ThreadTreeNode;

typedef struct ThreadTreeRep {
	Link messages;
} ThreadTreeRep;

// Auxiliary data structures and functions

// Add any new data structures and functions here ...

static void doDropThreadTree (Link t);
static void doShowThreadTree (Link t, int level);