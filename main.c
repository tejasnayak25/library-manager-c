#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct book {
    char* name;
    char* genre;
    char* status;
    struct book * prev;
    struct book * next;
} * Book;

Book first = NULL, last = NULL;

typedef struct category {
    char* name;
    Book* books;
    int rear;
    int numBooks;
    struct category * next;
    struct category * prev;
} * Category;

Category firstcat = NULL, lastcat = NULL;

void displayLoop(Book item) {
    if(item != NULL) {
        printf("| %-25s | %-25s | %-25s |\n", item->name, item->genre, item->status);
        displayLoop(item->next);
    }
}

void displayCatLoop(Category item) {
    if(item != NULL) {
        printf("| %-25s | %-25d |\n", item->name, item->rear+1);
        displayCatLoop(item->next);
    }
}

void displayBooks() {
    printf("Books: \n");
    printf("| %-25s | %-25s | %-25s |\n", "-------------------------", "-------------------------", "-------------------------");
    printf("| %-25s | %-25s | %-25s |\n", "Name", "Genre", "Status");
    printf("| %-25s | %-25s | %-25s |\n", "-------------------------", "-------------------------", "-------------------------");
    displayLoop(first);
    printf("| %-25s | %-25s | %-25s |\n", "-------------------------", "-------------------------", "-------------------------");
}

void displayCats() {
    printf("Genres: \n");
    printf("| %-25s | %-25s |\n", "-------------------------", "-------------------------");
    printf("| %-25s | %-25s |\n", "Name", "No. Of Books");
    printf("| %-25s | %-25s |\n", "-------------------------", "-------------------------");
    displayCatLoop(firstcat);
    printf("| %-25s | %-25s |\n", "-------------------------", "-------------------------");
}

void readString(char* buffer) {
    if (fgets(buffer, 1024, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

Book create() {
    Book temp = (Book) malloc(sizeof(struct book));

    temp->prev = NULL;
    temp->next = NULL;

    return temp;
}

Book insertRear() {
    Book temp = create();

    if(first == NULL) {
        last = first = temp;
    } else {
        last->next = temp;
        temp->prev = last;
        last = temp;
    }

    return temp;
}

Book appendRear(Book first, Book last, Book temp) {
    if(first == NULL) {
        last = first = temp;
    } else {
        last->next = temp;
        temp->prev = last;
        last = temp;
    }

    return temp;
}

Book appendToCat(Category cat, Book item) {
    cat->numBooks++;
    cat->books = (Book *) realloc(cat->books, cat->numBooks*sizeof(Book));
    cat->rear++;
    cat->books[cat->rear] = item;
}

Book search(char key[]) {
    Book temp;
    temp = first;

    while(temp != NULL) {
        if(strcmp(temp->name, key) == 0) {
            return temp;
        }
        temp=temp->next;
    }

    return NULL;
}

Category createCat() {
    Category temp = (Category) malloc(sizeof(struct category));

    temp->rear = -1;
    temp->numBooks = 1;
    temp->prev = NULL;
    temp->next = NULL;
    
    temp->books = (Book *) malloc(temp->numBooks*sizeof(Book));

    return temp;
}

Category insertRearCat() {
    Category temp = createCat();

    if(firstcat == NULL) {
        lastcat = firstcat = temp;
    } else {
        lastcat->next = temp;
        temp->prev = lastcat;
        lastcat = temp;
    }

    return temp;
}

Category searchCat(char key[]) {
    Category temp;
    temp = firstcat;

    while(temp != NULL) {
        if(strcmp(temp->name, key) == 0) {
            return temp;
        }
        temp=temp->next;
    }

    return NULL;
}

int searchByGenre(char key[]) {
    Category cat = searchCat(key);

    if(cat == NULL) return 0;

    Book temp;

    printf("Genre - %s:\n", cat->name);

    if(cat->rear == -1) {
        printf("Nothing here!");
        return 1;
    }

    printf("| %-25s | %-25s |\n", "-------------------------", "-------------------------");
    printf("| %-25s | %-25s |\n", "Name", "Status");
    printf("| %-25s | %-25s |\n", "-------------------------", "-------------------------");

    for (int i = 0; i <= cat->rear; i++)
    {
        temp = cat->books[i];
        printf("| %-25s | %-25s |\n", temp->name, temp->status);
    }
    
    printf("| %-25s | %-25s |\n", "-------------------------", "-------------------------");

    return 1;
}

void initLibrary() {
    FILE * f = fopen("books.csv", "r");
    if (f == NULL) {
        printf("Error during initialization of library.\n");
        exit(0);
    }

    char line[1024];

    while (fgets(line, sizeof(line), f) != NULL) {
        char name[256];
        char genre[256];
        
        if (sscanf(line, "%255[^,],%255[^\n]", name, genre) == 2) {
            Book temp = insertRear();

            temp->name = strdup(name);
            temp->genre = strdup(genre);
            temp->status = "Available";

            Category cat = searchCat(temp->genre);
            if(cat != NULL) {
                appendToCat(cat, temp);
            } else {
                Category cat = insertRearCat();
                cat->name = strdup(genre);
                appendToCat(cat, temp);
            }
        }
    }

    fclose(f);
}

void freeMemory(Book item) {
    freeMemory(item->next);
    free(item);
}

void main() {
    int ch;
    Book result;
    char * name;

    initLibrary();

    printf("Library Manager:\n");
    printf("1. Display Books\n2. Search\n3. Borrow\n4. Return Book\n5. Display Genres\n6. Search By Genre\n7. Exit\n");

    while(1) {
        printf("Enter your choice: ");
        scanf("%d", &ch);

        switch(ch) {
            case 1: displayBooks();break;
            case 2:
            printf("Enter book name: ");
            fflush(stdin);
            scanf("%256[^\n]", name);
            
            result = search(name);
            if(result != NULL) {
                printf("Name: %s, Genre: %s, Status: %s\n", result->name, result->genre, result->status);
            } else {
                printf("Book Not Found!\n");
            }
            break;
            case 3:
            printf("Enter book name: ");
            fflush(stdin);
            scanf("%256[^\n]", name);
            
            result = search(name);
            if(result != NULL) {
                result->status = "Borrowed";
                printf("Book \"%s\" borrowed successfully!\n", result->name);
            } else {
                printf("Book Not Found!\n");
            }
            break;
            case 4:
            printf("Enter book name: ");
            fflush(stdin);
            scanf("%256[^\n]", name);
            
            result = search(name);
            if(result != NULL) {
                if(result->status != "Borrowed") {
                    printf("Book \"%s\" not borrowed!\n", result->name);
                } else {
                    result->status = "Available";
                    printf("Book \"%s\" returned successfully!\n", result->name);
                }
            } else {
                printf("Book Not Found!\n");
            }
            break;
            case 5: displayCats(); break;
            case 6: 
            printf("Enter genre name: ");
            fflush(stdin);
            scanf("%256[^\n]", name);
            
            int res = searchByGenre(name);
            if(res == 0) {
                printf("Genre Not Found!\n");
            }
            break;
            case 7: exit(0);
            default: printf("Invalid choice\n");
        }
    }
    
    freeMemory(first);
}