#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// All the uppercase letters and space
static const char charset[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

int main(int argc, char* argv[]) {
    // Seed the random number generator
    srand(time(NULL));

    // Check if the user has provided the key length
    if (argc < 2) {
        fprintf(stderr, "Error: insufficient input.\nUsage: keygen key_length\n");
        exit(1);
    }

    // Get the key length from the input argument
    int key_length = atoi(argv[1]);

    // Allocate memory for the key and null-terminate it
    char* key = malloc(key_length + 1);
    if (key == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for key.\n");
        exit(1);
    }
    key[key_length] = '\0';

    // Generate the key
    for (int i = 0; i < key_length; i++) {
       // int random_index = rand() % 27;
        key[i] = charset[rand() % 27];
    }

    // Print the key
    printf("%s\n", key);

    // Free the memory allocated for the key
    free(key);

    return 0;
}
