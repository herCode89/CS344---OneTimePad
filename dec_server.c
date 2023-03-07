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


void error(const char* msg) {
    perror(msg);
    exit(1);
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

int FindChar(char st, char* charset) {
    int temp;
    for (int i = 0; i < 27; i++) {
        if (st == charset[i]) {
            temp = i;
            break;
        }
    }
    return temp;
}

char* CodeDecrypt(char* ciphertext, char* key, char* charset)
{
    // Allocate memory for the decrypted message
    char* msgs = calloc(strlen(ciphertext) + 1, sizeof(char));

    // Initialize pointers for the ciphertext, key, and decrypted message
    char* cipherPtr = ciphertext;
    char* keyPtr = key;
    char* msgPtr = msgs;

    // Loop through each character in the ciphertext
    while (*cipherPtr != '\0') {

        // Find the index of the character in the ciphertext and the key in the character set
        int inMSG = FindChar(*cipherPtr, charset);
        int inKEY = FindChar(*keyPtr, charset);

        // Subtract the key index from the ciphertext index to get the decrypted value
        int value3 = inMSG - inKEY;
        if (value3 < 0)
            value3 += 27;

        // Take the modulus of the decrypted value to get the actual index in the character set
        int value2 = value3 % 27;

        // Use the index to get the decrypted character from the character set
        int inMSC = charset[value2];
        *msgPtr = inMSC;     // help from cited source chatgpt as approved by professor

        // Move the pointers to the next character in each string
        cipherPtr++;
        keyPtr++;
        msgPtr++;
    }

    // Add a null terminator to the end of the decrypted message
    *msgPtr = '\0';

    // Return the decrypted message
    return msgs;
}

int main(int argc, char* argv[]) {
    int DecodeConnectionSocket, charsRead, charsWritten;
    char plaintext[70000];
    char ciphertext[70000];
    char msg[70000];
    char key[70000];
    char buffer[70000];
    // All the uppercase letters and space
    char charset[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    pid_t PID;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Check usage & args
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("ERROR opening socket");
    }
    setupAddressStructInServer(&serverAddress, atoi(argv[1]));
    if (bind(listenSocket, (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)) < 0) {
        error("ERROR on binding");
    }
    listen(listenSocket, 5);
    while (1) {
        DecodeConnectionSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, &sizeOfClientInfo);
        if (DecodeConnectionSocket < 0) {
            error("ERROR on accept");
        }
        PID = fork();
        if (PID == -1) {
            error("fork() failed.");
            //break;
        }
        else if (PID == 0) {
            memset(buffer, '\0', sizeof(buffer));
            charsRead = recv(DecodeConnectionSocket, buffer, sizeof(buffer) - 1, 0);
            if (charsRead < 0) {
                error("ERROR reading from socket");
            }
            char* token = strtok(buffer, ",");
            if (strcmp(token, "dec") != 0) {
                send(DecodeConnectionSocket, "DEC_SERVER", 10, 0);
            }
            else {
                token = strtok(NULL, ",");
                strcpy(ciphertext, token);
                token = strtok(NULL, ",");
                strcpy(key, token);
                char* ciphertext = calloc(strlen(ciphertext) + 1, sizeof(char));
                ciphertext = CodeDecrypt(ciphertext, key, charset);
                charsWritten = 0;
                while (charsWritten < strlen(plaintext)) {
                    if (charsWritten < 0) {
                        error("ERROR writing to socket");
                    }
                    charsWritten += send(DecodeConnectionSocket, plaintext, strlen(plaintext), 0);
                }
            }
            close(DecodeConnectionSocket);
            exit(0);
            break;
        }

        else {
            //break;
        }

        close(DecodeConnectionSocket);

    }
    close(listenSocket);
    return 0;
}