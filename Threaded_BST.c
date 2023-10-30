// Doubly threaded inorder BST
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Node structure for Threaded Binary Search Tree
typedef struct Node 
{
    int data;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
    bool isLeftThreaded;
    bool isRightThreaded;
}
Node;


// Function to create a new node with the given data and pointers NULL
Node *createNode(int value) 
{
    Node *newNode = malloc(sizeof(Node));
    newNode->data = value;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;
    newNode->isLeftThreaded = 0;
    newNode->isRightThreaded = 0;
    return newNode;
}


// Function to find the in-order predecessor of a node
Node *inorderPredecessor(Node *node, Node *head) 
{
    if (node->left == head)
        return NULL;

    else if (node->isLeftThreaded)
        return node->left;
    
    else
    {
        node = node->left;
        while (node->isRightThreaded == 0)
            node = node->right;

        return node;
    }
}


// Function to find the in-order successor of a node
Node *inorderSuccessor(Node *node, Node *head)
{
    if (node->right == head)
        return NULL;

    if (node->isRightThreaded)
        return node->right;

    else
    {
        node = node->right;
        while (node->isLeftThreaded == 0)
        {
            node = node->left;
        }
        return node;
    }
}


// Function to insert a new node in the threaded BST
Node *insert(Node *root, Node *head, int value)
{
    // Node is first in the tree
    if (root == NULL)
    {
        Node *newNode = createNode(value);
        newNode->left = head;
        newNode->isLeftThreaded = 1;
        newNode->right = head;
        newNode->isRightThreaded = 1;
        newNode->parent = NULL;

        return newNode;
    }

    // Node is left of root
    if (value < root->data)
    {
        // Node is left child
        if (root->isLeftThreaded)
        {
            Node *newNode = createNode(value);
            newNode->left = root->left;
            newNode->isLeftThreaded = 1;
            newNode->right = root;
            newNode->isRightThreaded = 1;
            newNode->parent = root;
            root->left = newNode;
            root->isLeftThreaded = 0;
        }
        
        else
            root->left = insert(root->left, head, value);
    }
    
    // Node is right of root
    else
    {
        // Node is right child
        if (root->isRightThreaded)
        {
            Node *newNode = createNode(value);
            newNode->right = root->right;
            newNode->isRightThreaded = 1;
            newNode->left = root;
            newNode->isLeftThreaded = 1;
            newNode->parent = root;
            root->right = newNode;
            root->isRightThreaded = 0;
        }

        else
            root->right = insert(root->right, head, value);
    }

    return root;
}


// Function to delete a node from the threaded BST
Node *deleteNode(Node *root, Node *head, int value)
{
    // Tree is empty
    if (root == NULL)
    {
        printf("%d not found\n", value);
        return root;
    }

    // Finding the node to be deleted
    int found = 0;
    Node *current = root;
    while (current != NULL)
    {
        // Node found
        if (value == current->data)
        {
            found = 1;
            break;
        }

        // Node might be in left sub-tree
        else if (value < current->data)
        {
            if (current->isLeftThreaded == 1)
                break;
            
            else
                current = current->left;
        }

        // Node might be in right sub-tree
        else
        {
            if (current->isRightThreaded == 1)
                break;
            
            else
                current = current->right;
        }
    }

    // Node is not present in tree
    if (!found)
    {
        printf("%d not found\n", value);
        return root;
    }

    // Node is found at this stage
    Node *parent = current->parent;

    // Case 1: Node has no children
    if ((current->isLeftThreaded) && (current->isRightThreaded))
    {
        // It is the root node
        if (parent == NULL)
        {
            root = NULL;
            free(current);
            return root;
        }

        // It is a left child
        if (current == parent->left)
        {
            parent->left = current->left;
            parent->isLeftThreaded = 1;

            // Updating the thread of it's predecessor
            if ((current->left != head) && ((current->left)->isRightThreaded))
                (current->left)->right = parent;
        }

        // It is a right child
        else
        {
            parent->right = current->right;
            parent->isRightThreaded = 1;

            // Updating the thread of it's successor
            if ((current->right != head) && ((current->right)->isLeftThreaded))
                (current->right)->left = parent;
        }

        free(current);
    }

    // Case 2: Node has two children
    else if ((!current->isLeftThreaded) && (!current->isRightThreaded))
    {
        Node *leftchild = current->left;

        // Updating the thread pointing to the node
        Node *pred = inorderPredecessor(current, head);
        pred->right = inorderSuccessor(current, head);

        // Min of right subtree
        Node *minofright = current->right;
        while (!minofright->isLeftThreaded)
            minofright = minofright->left;

        // It is the root node
        if (parent == NULL)
        {
            root = current->right;
            root->parent = NULL;
            minofright->left = leftchild;
            minofright->isLeftThreaded = 0;
            leftchild->parent = minofright;

            free(current);
            return root;
        }

        // It is a left child
        if (current == parent->left)
            parent->left = current->right;

        // It is a right child
        else if (current == parent->right)
            parent->left = current->right;

        (current->right)->parent = parent;
        minofright->left = leftchild;
        minofright->isLeftThreaded = 0;
        leftchild->parent = minofright;

        free(current);
    }

    // Case 3: Node has only one child
    else
    {
        // Case 3.1: It has left child only
        if (!current->isLeftThreaded)
        {
            // Left child and predecessor of node
            Node *leftchild = current->left;    
            Node *pred = inorderPredecessor(current, head);
            Node *succ = inorderSuccessor(current, head);

            // It is the root node
            if (parent == NULL)
            {
                root = leftchild;
                root->parent = NULL;
                pred->right = head;

                free(current);
                return root;
            }

            if (current == parent->left)
                parent->left = leftchild;

            else if (current == parent->right)
                parent->right = leftchild;

            leftchild->parent = parent;
            pred->right = succ;

            if (succ == NULL)
                pred->right = head;

            if ((succ != NULL) && (succ->isLeftThreaded))
                succ->left = pred;
        }

        // Case 3.2: It has right child only
        else
        {
            // Right child and min of right sub-tree
            Node *rightchild = current->right;
            Node *succ = inorderSuccessor(current, head);
            Node *pred = inorderPredecessor(current, head);

            // It is the root node
            if (parent == NULL)
            {
                root = rightchild;
                root->parent = NULL;
                succ->left = head;

                free(current);
                return root;
            }

            if (current == parent->left)
                parent->left = rightchild;

            else if (current == parent->right)
                parent->right = rightchild;

            rightchild->parent = parent;
            succ->left = pred;

            if (pred == NULL)
                succ->left = head;

            if ((pred != NULL) && (pred->isRightThreaded))
                pred->right = succ;
        }
    }

    return root;
}


// Function to perform inorder traversal of the threaded BST
void inorderTraversal(Node *root, Node *head)
{
    if (root == NULL)
    {
        printf("Tree is empty\n");
        return;
    }

    Node *curr = root;
    while (curr->isLeftThreaded == 0)
        curr = curr->left;

    while (curr != NULL)
    {
        printf("%d ", curr->data);
        curr = inorderSuccessor(curr, head);
    }

    printf("\n");
}


// Function to perform preorder traversal of the threaded BST
void preorderTraversal(Node *root, Node *head)
{
    if (root == head)
        return;

    printf("%d ", root->data);
    if (!root->isLeftThreaded)
        preorderTraversal(root->left, head);
    if (!root->isRightThreaded)
        preorderTraversal(root->right, head);
}


// Function to perform postorder traversal of the threaded BST
void postorderTraversal(Node *root, Node *head)
{
    if (root == head)
        return;

    if (!root->isLeftThreaded)
        postorderTraversal(root->left, head);
    if (!root->isRightThreaded)
        postorderTraversal(root->right, head);
    printf("%d ", root->data);
}


// Function to deallocate the entire BST
void deallocateTree(Node *root)
{
    if (root != NULL)
    {
        if (root->isLeftThreaded == 0)
            deallocateTree(root->left);

        if (root->isRightThreaded == 0)
            deallocateTree(root->right);

        free(root);
    }
}


// Main function
int main()
{
    // Dummy head node to handle dangling threads
    Node *head = createNode(-1);
    head->isLeftThreaded = 0;
    head->isRightThreaded = 0;
    head->right = head;

    Node *root = NULL;
    head->left = root;
    char operation;
    int value;

    while (1)
    {
        scanf(" %c", &operation);

        if (operation == 'x')
            break;

        switch (operation)
        {
            case 'i': // Insert
                scanf("%d", &value);
                root = insert(root, head, value);
                head->left = root;
                break;

            case 'd': // Delete
                scanf("%d", &value);
                root = deleteNode(root, head, value);
                head->left = root;
                break;

            case 't': // Inorder Traversal
                inorderTraversal(root, head);
                break;

            case 'y': // Preorder Traversal
                if (root == NULL)
                {
                    printf("Tree is empty\n");
                    break;
                }

                preorderTraversal(root, head);
                printf("\n");
                break;

            case 'u': // Postorder Traversal
                if (root == NULL)
                {
                    printf("Tree is empty\n");
                    break;
                }

                postorderTraversal(root, head);
                printf("\n");
                break;
        }
    }

    // Free all the allocated memory
    deallocateTree(root);
    free(head);
    return 0;
}
