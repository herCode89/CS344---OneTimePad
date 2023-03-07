/*
Project - One Time Pad
By: Frankie Herbert
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


// Error function used for reporting issues
void error(const char* msg) {
    perror(msg);
    exit(1);
}

// This function takes a character 'st' and a character array 'charset' as input
// and searches for the index of the first occurrence of 'st' in 'charset'.

int FindChar(char st, char* charset) {
    // Loop through each character in 'charset'
    for (int i = 0; charset[i] != '\0'; i++) {
        // If 'st' is found at index 'i', return the index
        if (st == charset[i]) {
            return i;
        }
    }
    // If 'st' is not found, return -1
    return -1;
}

char* CodeEncrypt(char* msg, char* key, char* charset)
{
    // Allocate memory for ciphertext and initialize pointers
    char* ciphertext = calloc(strlen(msg) + 1, sizeof(char)); // allocate memory for ciphertext
    char* msgPtr = msg; // initialize pointer to msg
    char* keyPtr = key; // initialize pointer to key
    char* cipherPtr = ciphertext; // initialize pointer to ciphertext
    while (*msgPtr != '\0') { // loop through message until end of string is reached
        // Find the index of the character in the message and key
        int inMSG = FindChar(*msgPtr, charset); // find index of current character in message
        int inKEY = FindChar(*keyPtr, charset); // find index of current character in key
        // Add the index values of message and key
        int value1 = inMSG + inKEY;
        // If value1 is less than 0, subtract 27 from it
        if (value1 < 0)
            value1 -= 27;
        // Get the remainder of value1 divided by 27 and use it as the index
        int value2 = value1 % 27;
        int inCIPHER = charset[value2]; // find the corresponding character in the character set
        // Assign the cipher character to the current position in the ciphertext array
        *cipherPtr = inCIPHER;
        // Move the pointers to the next character in the message, key, and ciphertext arrays
        msgPtr++;
        keyPtr++;
        cipherPtr++;
    }
    // Add null terminator to the end of the ciphertext array
    *cipherPtr = '\0';
    // Return a pointer to the ciphertext array
    return ciphertext;
}

void setupAddressStructInServer(struct sockaddr_in* address, int portNumber) {

    // Clear out the address struct
    memset((char*)address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);
    // Allow a client at any address to connect to this server
    address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char* argv[]) {
    // Declare and initialize variables for the program
    int EncodeConnectionSocket, charsRead, charsWritten;
    char plaintext[70000];
    char msg[70000];
    char key[70000];
    char buffer[70000];
    // All the uppercase letters and space
    char charset[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    pid_t PID;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Check usage & args
// If less than 2 arguments are passed, print an error message and exit the program
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }
    // Create a new socket for listening
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("ERROR opening socket");
    }
    // Set up the server address struct
    setupAddressStructInServer(&serverAddress, atoi(argv[1]));
    // Bind the listen socket to the server address
    if (bind(listenSocket, (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)) < 0) {
        error("ERROR on binding");
    }
    // Start listening for connections
    listen(listenSocket, 5);
    // Loop infinitely to accept and handle client connections
    while (1) {
        // Accept a connection from a client
        EncodeConnectionSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, &sizeOfClientInfo);
        if (EncodeConnectionSocket < 0) {
            error("ERROR on accept");
        }
        // Fork a new process to handle the client request
        PID = fork();
        if (PID == -1) {
            error("fork() failed.");
            //break;
        }
        // If the PID is 0, handle the client request
        else if (PID == 0) {
            // Clear the buffer and read data from the socket
            memset(buffer, '\0', sizeof(buffer));
            charsRead = recv(EncodeConnectionSocket, buffer, sizeof(buffer) - 1, 0);
            if (charsRead < 0) {
                error("ERROR reading from socket");
            }
            // Parse the request from the client
            char* token = strtok(buffer, ",");
            if (strcmp(token, "enc") != 0) {
                // If the request is not "enc", send an error message back to the client
                send(EncodeConnectionSocket, "ENC_SERVER", 10, 0);
            }
            else {
                // If the request is "enc", parse the plaintext and key from the request and encrypt the plaintext
                token = strtok(NULL, ",");
                strcpy(plaintext, token);
                token = strtok(NULL, ",");
                strcpy(key, token);
                char* ciphertext = calloc(strlen(plaintext) + 1, sizeof(char));
                ciphertext = CodeEncrypt(plaintext, key, charset);
                // Send the encrypted ciphertext back to the client
                charsWritten = 0;
                while (charsWritten < strlen(ciphertext)) {
                    if (charsWritten < 0) {
                        error("ERROR writing to socket");
                    }
                    charsWritten += send(EncodeConnectionSocket, ciphertext, strlen(ciphertext), 0);
                }
            }
            // Close the connection and exit the child process
            close(EncodeConnectionSocket);
            exit(0);
            break;
        }
        // If the PID is not 0, close the connection and continue waiting for connections

        else {
            //break;
        }
        // Close the connection
        close(EncodeConnectionSocket);

    }
    // Close the listening socket
    close(listenSocket);

    // Return 0 to indicate successful execution of the program
    return 0;
}