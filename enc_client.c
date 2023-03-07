/*
Project - One Time Pad
By: Frankie Herbert
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

void setupAddressStructInClient(struct sockaddr_in* address, int portNumber,
    char* hostname) {

    memset((char*)address, '\0', sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);
    struct hostent* hostInfo = gethostbyname(hostname);

    if (hostInfo == NULL) {
        fprintf(stderr, "ENC_CLIENT: ERROR, no such host\n");
        exit(0);
    }
    memcpy((char*)&address->sin_addr.s_addr, hostInfo->h_addr_list[0],
        hostInfo->h_length);
}


bool NoGoodChecker(char* r, char* charset) {
    bool NoGoodChar = true; // initialize NoGoodChar flag to true
    for (int i = 0; i < strlen(r); i++) { // iterate through each character in r
        for (int j = 0; j < strlen(charset); j++) { // iterate through each character in charset
            if (r[i] == charset[j]) { // if the character in r matches a character in charset
                NoGoodChar = false; // set NoGoodChar flag to false
                break; // exit the inner loop
            }
        }
        if (NoGoodChar) // if NoGoodChar flag is still true, meaning no match found in charset
            break; // exit the outer loop
    }
    return NoGoodChar; // return the NoGoodChar flag value
}

int main(int argc, char* argv[]) {
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char plaintext[70000];
    char ciphertext[70000];
    char key[70000];
    char buffer[70000]; 
    bool NoGoodPlain, NoGoodKey;
    FILE* PlainFile, * KeyFile;
    char charset[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    PlainFile = fopen(argv[1], "r");
    KeyFile = fopen(argv[2], "r");

    if (PlainFile == NULL) {
        // send appropriate error text to stderr
        fprintf(stderr, "ERROR opening plaintext file\n");
        exit(1);
    }
    if (KeyFile == NULL) {
        // send appropriate error text to stderr
        // using fprintf instead of perror since it kept appending a '%'
        fprintf(stderr, "ERROR opening key file\n");
        exit(1);
    }
    fgets(plaintext, sizeof(plaintext), PlainFile);
    plaintext[strcspn(plaintext, "\n")] = '\0';
    fgets(key, sizeof(key), KeyFile);
    key[strcspn(key, "\n")] = '\0';

    if (strlen(key) < strlen(plaintext)) {
        fprintf(stderr, "ERROR key is shorter than plaintext\n");
        exit(1);
    }
    NoGoodPlain = NoGoodChecker(plaintext, charset);
    NoGoodKey = NoGoodChecker(key, charset);
    if (NoGoodPlain || NoGoodKey) {
        fprintf(stderr, "ERROR bad character\n");
        exit(1);
    }

    // Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        fprintf(stderr, "ENC_CLIENT: ERROR opening socket\n");
        exit(1);
    }
    setupAddressStructInClient(&serverAddress, atoi(argv[3]), "localhost");
    if (connect(socketFD, (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)) < 0) {
        fprintf(stderr, "ENC_CLIENT: ERROR connecting\n");
        exit(1);
    }
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "enc,%s,%s,", plaintext, key);
    charsWritten = 0;
    while (charsWritten < strlen(buffer)) {
        if (charsWritten < 0) {
            fprintf(stderr, "ENC_CLIENT: ERROR writing to socket\n");
            exit(1);
        }
        charsWritten += send(socketFD, buffer, strlen(buffer), 0);
    }

    if (charsWritten < sizeof(buffer)) {
        fprintf(stderr, "ENC_CLIENT: WARNING: Not all data written to socket!\n");
        exit(1);
    }
    memset(buffer, '\0', sizeof(buffer));
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
    if (strcmp(buffer, "DEC_SERVER") == 0) {
        fprintf(stderr, "ENC_CLIENT: ERROR connecting to wrong server at port %d\n",
            ntohs(serverAddress.sin_port));
        exit(2);
    }
    printf("%s\n", buffer);
    // Close the socket
    close(socketFD);
    return 0;
}