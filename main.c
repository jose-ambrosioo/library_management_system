#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

// Define a structure to represent a book
struct Book {
    char title[100];
    char author[100];
    int year;
    long long isbn;
    struct Book *left;
    struct Book *right;
};

// Global constants
#define BST_BLOCK_SIZE 256

// Define the node structure for memory management
typedef struct bst_node_t {
    struct bst_node_t* next;  // Pointer to the next node in the free list
    struct Book book;  // Actual BookNode structure embedded within
} bst_node_t;

bst_node_t* bst_current_block = NULL;
int bst_size_left;
bst_node_t* bst_free_list = NULL;

// Create a new node for the Binary Search Tree
bst_node_t* get_node() {
    bst_node_t* tmp;

    if (bst_free_list != NULL) {
        tmp = bst_free_list;
        bst_free_list = bst_free_list->next;
    } else {
        if (bst_current_block == NULL || bst_size_left == 0) {
            // Allocate a new memory block
            bst_current_block = (bst_node_t*)malloc(BST_BLOCK_SIZE * sizeof(bst_node_t));
            bst_size_left = BST_BLOCK_SIZE;
        }
        tmp = bst_current_block++;
        bst_size_left -= 1;
    }

    return tmp;
}

// Return a node to the free list for Binary Search Tree
void return_node(bst_node_t* node) {
    if (node != NULL) {
        node->next = bst_free_list;
        bst_free_list = node;
    } else {
        printf("Error: Attempting to return an invalid node to the free list.\n");
    }
}


// Function to create a new book node
struct Book *createBook(char title[], char author[], int year, long long isbn) {
    struct Book *newBook = get_node();
    if (newBook != NULL) {
        strcpy(newBook->title, title);
        strcpy(newBook->author, author);
        newBook->year = year;
        newBook->isbn = isbn;
        newBook->left = NULL;
        newBook->right = NULL;
    }
    return newBook;
}

// Function to insert a new book into the BST
struct Book *insertBook(struct Book *root, char title[], char author[], int year, long long isbn) {
    if (root == NULL) {
        return createBook(title, author, year, isbn);
    }

    int compareResult = strcmp(title, root->title);

    if (compareResult < 0) {
        root->left = insertBook(root->left, title, author, year, isbn);
    } else if (compareResult > 0) {
        root->right = insertBook(root->right, title, author, year, isbn);
    }

    return root;
}

// Function to search for a book by its title
struct Book *searchBook(struct Book *root, const char title[]) {
    if (root == NULL || strcmp(title, root->title) == 0) {
        return root;
    }

    if (strcmp(title, root->title) < 0) {
        return searchBook(root->left, title);
    } else {
        return searchBook(root->right, title);
    }
}

// Function to find the minimum value node in a BST
struct Book *findMin(struct Book *node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

// Function to delete a book from the BST
struct Book *deleteBook(struct Book *root, char title[]) {
    if (root == NULL) {
        return root;
    }

    int compareResult = strcmp(title, root->title);

    if (compareResult < 0) {
        root->left = deleteBook(root->left, title);
    } else if (compareResult > 0) {
        root->right = deleteBook(root->right, title);
    } else {
        // Case 1: No child or one child
        if (root->left == NULL) {
            struct Book *temp = root->right;
            return_node(root);
            return temp;
        } else if (root->right == NULL) {
            struct Book *temp = root->left;
            return_node(root);
            return temp;
        }
        // Case 2: Two children
        struct Book *temp = findMin(root->right);
        strcpy(root->title, temp->title);
        root->right = deleteBook(root->right, temp->title);
    }

    return root;
}

// Function to perform in-order traversal
void inOrderTraversal(struct Book *root) {
    if (root != NULL) {
        inOrderTraversal(root->left);
        printw("Title: %s, Author: %s, Year: %d, ISBN: %lld\n", root->title, root->author, root->year, root->isbn);
        inOrderTraversal(root->right);
    }
    else {
        printf("No books were found.\n");
    }
}

// Function to perform pre-order traversal
void preOrderTraversal(struct Book *root) {
    if (root != NULL) {
        printw("Title: %s, Author: %s, Year: %d, ISBN: %lld\n", root->title, root->author, root->year, root->isbn);
        preOrderTraversal(root->left);
        preOrderTraversal(root->right);
    }
    else {
        printf("No books were found.\n");
    }
}

// Function to perform post-order traversal
void postOrderTraversal(struct Book *root) {
    if (root != NULL) {
        postOrderTraversal(root->left);
        postOrderTraversal(root->right);
        printw("Title: %s, Author: %s, Year: %d, ISBN: %lld\n", root->title, root->author, root->year, root->isbn);
    }
    else {
        printf("No books were found.\n");
    }
}


// Function to free memory allocated for the BST
void freeTree(struct Book *root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// Function to add a book to the library
void add_book(struct Book **root) {
    char title[100], author[100];
    int year;
    long long isbn;

    printw("Enter book title: ");
    // scanf(" %[^\n]", title);
    scanw("%s", title);
    printw("Enter book author: ");
    scanw("%s", author);
    printw("Enter publication year: ");
    scanw("%d", &year);
    printw("Enter ISBN: ");
    scanw("%lld", &isbn);

    *root = insertBook(*root, title, author, year, isbn);
    printf("Book added successfully.\n");
    
    // press_enter_to_clear_screen();
}

// Function to search for a book in the library
void search_book(struct Book *root) {
    char title[100];
    printw("Enter book title to search: ");
    scanw("%s", title);

    struct Book *result = searchBook(root, title);
    if (result != NULL) {
        printf("Book found:\n");
        printf("Title: %s\nAuthor: %s\nYear: %d\nISBN: %lld\n", result->title, result->author, result->year, result->isbn);
        
        press_enter_to_clear_screen();
    } else {
        printf("Book not found.\n");
        
        press_enter_to_clear_screen();
    }
}

// Function to delete a book from the library
void delete_book(struct Book **root) {
    char title[100];
    printf("Enter book title to delete: ");
    scanf(" %[^\n]", title);

    *root = deleteBook(*root, title);
    
    if(*root != NULL) {
        printf("Book deleted successfully.\n");
    
        press_enter_to_clear_screen();
    }
    else {
        printf("Book not found.\n");
    
        press_enter_to_clear_screen();
    }
}

void press_enter_to_clear_screen() {
	
	// Wait for user's Enter key press before clearing the screen
	refresh();
	while (getch() != '\n') {} // Wait for Enter key press

	clear(); // Clear the screen after playback
}

// Function to display the menu
void display_menu() {
    clear(); // Clear the screen
    printw("** Project 2 - Library Management System **\n");
    printw("1. Add a Book\n");
    printw("2. Search for a Book\n");
    printw("3. Delete a Book\n");
    printw("4. Display Books (In-order traversal)\n");
    printw("5. Display Books (Pre-order traversal)\n");
    printw("6. Display Books (Post-order traversal)\n");
    // printw("7. Clear Screen\n");
    printw("7. Quit\n");
    printw("******************************************\n");
    refresh(); // Refresh the screen to display changes
}

// Function to handle user input
void handle_user_input(struct Book **root) {
    int choice;
    char input[100];
    
    while(1) {
        printw("Enter your choice: ");
        refresh(); // Ensure the prompt is displayed
        
        echo(); // Enable echoing of input
        getstr(input); // Use getstr to get a string input
        // noecho(); // Disable echoing to hide user input
        
        if (sscanf(input, "%d", &choice) != 1) {
            printw("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1:
                clear(); // Clear the screen
                // Add a Book
                add_book(root);
                break;
            case 2:
                clear(); // Clear the screen
                // Search for a Book
                search_book(*root);
                break;
            case 3:
                clear(); // Clear the screen
                // Delete a Book
                delete_book(root);
                break;
            case 4:
                clear(); // Clear the screen
                // Display Books (In-order traversal)
                printw("Books in the library (In-order traversal):\n");
                inOrderTraversal(*root);
                press_enter_to_clear_screen();
                break;
            case 5:
                clear(); // Clear the screen
                // Display Books (Pre-order traversal)
                printw("Books in the library (Pre-order traversal):\n");
                preOrderTraversal(*root);
                press_enter_to_clear_screen();
                break;
            case 6:
                clear(); // Clear the screen
                // Display Books (Post-order traversal)
                printw("Books in the library (Post-order traversal):\n");
                postOrderTraversal(*root);
                press_enter_to_clear_screen();
                break;
            //case 7:
            //    clear(); // Clear the screen
            //    system("clear"); // For Unix/Linux
                // system("cls"); // For Windows
            //    break;
            case 7:
                // Quit
                endwin(); // End ncurses before exiting
                printw("Exiting the program.\n");
                exit(0);
            default:
                printw("Invalid choice. Please try again.\n");
                break;
        }
        display_menu();
    }
}

int main() {
    initscr(); // Initialize ncurses
    noecho();
    cbreak();
    
    struct Book *root = NULL;

    display_menu();
    handle_user_input(&root);
    
    endwin(); // End ncurses

    // Free allocated memory for the BST
    freeTree(root);

    return 0;
}
