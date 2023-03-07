gcc -std=gnu99 -o enc_server enc_server.c
gcc -std=gnu99 -o enc_client enc_client.c
gcc -std=gnu99 -o dec_server dec_server.c
gcc -std=gnu99 -o dec_client dec_client.c
gcc -std=gnu99 -o keygen keygen.c

./p5testscript 56712 64513 > mytestresults 2>&1







cited work: 

URL: https://en.wikipedia.org/wiki/One-time_pad

URL: https://man7.org/linux/man-pages/man3/rand.3.html
CODE: NAME         top
       rand, rand_r, srand - pseudo-random number generator
SYNOPSIS         top
       #include <stdlib.h>

       int rand(void);
       int rand_r(unsigned int *seedp);
       void srand(unsigned int seed);

   Feature Test Macro Requirements for glibc (see
   feature_test_macros(7)):

       rand_r():
           Since glibc 2.24:
               _POSIX_C_SOURCE >= 199506L
           Glibc 2.23 and earlier
               _POSIX_C_SOURCE
DESCRIPTION         top
       The rand() function returns a pseudo-random integer in the range
       0 to RAND_MAX inclusive (i.e., the mathematical range
       [0, RAND_MAX]).

       The srand() function sets its argument as the seed for a new
       sequence of pseudo-random integers to be returned by rand().
       These sequences are repeatable by calling srand() with the same
       seed value.

       If no seed value is provided, the rand() function is
       automatically seeded with a value of 1.

       The function rand() is not reentrant, since it uses hidden state
       that is modified on each call.  This might just be the seed value
       to be used by the next call, or it might be something more
       elaborate.  In order to get reproducible behavior in a threaded
       application, this state must be made explicit; this can be done
       using the reentrant function rand_r().

       Like rand(), rand_r() returns a pseudo-random integer in the
       range [0, RAND_MAX].  The seedp argument is a pointer to an
       unsigned int that is used to store state between calls.  If
       rand_r() is called with the same initial value for the integer
       pointed to by seedp, and that value is not modified between
       calls, then the same pseudo-random sequence will result.

       The value pointed to by the seedp argument of rand_r() provides
       only a very small amount of state, so this function will be a
       weak pseudo-random generator.  Try drand48_r(3) instead.
RETURN VALUE         top
       The rand() and rand_r() functions return a value between 0 and
       RAND_MAX (inclusive).  The srand() function returns no value.
ATTRIBUTES         top
       For an explanation of the terms used in this section, see
       attributes(7).

       ┌──────────────────────────────────────┬───────────────┬─────────┐
       │Interface                             │ Attribute     │ Value   │
       ├──────────────────────────────────────┼───────────────┼─────────┤
       │rand(), rand_r(), srand()             │ Thread safety │ MT-Safe │
       └──────────────────────────────────────┴───────────────┴─────────┘
CONFORMING TO         top
       The functions rand() and srand() conform to SVr4, 4.3BSD, C89,
       C99, POSIX.1-2001.  The function rand_r() is from POSIX.1-2001.
       POSIX.1-2008 marks rand_r() as obsolete.
NOTES         top
       The versions of rand() and srand() in the Linux C Library use the
       same random number generator as random(3) and srandom(3), so the
       lower-order bits should be as random as the higher-order bits.
       However, on older rand() implementations, and on current
       implementations on different systems, the lower-order bits are
       much less random than the higher-order bits.  Do not use this
       function in applications intended to be portable when good
       randomness is needed.  (Use random(3) instead.)
EXAMPLES         top
       POSIX.1-2001 gives the following example of an implementation of
       rand() and srand(), possibly useful when one needs the same
       sequence on two different machines.

           static unsigned long next = 1;

           /* RAND_MAX assumed to be 32767 */
           int myrand(void) {
               next = next * 1103515245 + 12345;
               return((unsigned)(next/65536) % 32768);
           }

           void mysrand(unsigned int seed) {
               next = seed;
           }

       The following program can be used to display the pseudo-random
       sequence produced by rand() when given a particular seed.

           #include <stdlib.h>
           #include <stdio.h>

           int
           main(int argc, char *argv[])
           {
               int r, nloops;
               unsigned int seed;

               if (argc != 3) {
                   fprintf(stderr, "Usage: %s <seed> <nloops>\n", argv[0]);
                   exit(EXIT_FAILURE);
               }

               seed = atoi(argv[1]);
               nloops = atoi(argv[2]);

               srand(seed);
               for (int j = 0; j < nloops; j++) {
                   r =  rand();
                   printf("%d\n", r);
               }

               exit(EXIT_SUCCESS);
           }
______________________________________________________________________________________________


Author: CS 344 Explorations
URL: https://replit.com/@cs344/82socketaddressc
Code:
int main(int argc, char *argv[]) {
  int   portNumber;
  struct hostent* hostInfo;
  struct sockaddr_in address;

  // Check usage & args
  if (argc < 3) { 
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]);
    fprintf(stderr,"For example: %s oregonstate.edu 8080\n", argv[0]); 
    exit(0); 
  } 

  // Clear out the address struct
  memset((char*) &address, '\0', sizeof(address));

  // The address should be network capable
  address.sin_family = AF_INET;

  // Store the port number
  portNumber = atoi(argv[2]);
  address.sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  hostInfo = gethostbyname(argv[1]); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy(&address.sin_addr.s_addr, 
          hostInfo->h_addr_list[0],
          hostInfo->h_length);

  // Print the IP address in the socket address as a dotted decimal string
  printf("IP address for %s: %s\n", argv[1], inet_ntoa(address.sin_addr));

  // EXERCISE: Replace ... with code that prints the port number in the socket address in the variable address
  // printf("Port number in the socket address: %d\n", ...);

  return 0;
}
=====================================================================================================================
Author: CS 344 Explorations
URL: https://replit.com/@cs344/83clientc
Code:
/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  char buffer[256];
  // Check usage & args
  if (argc < 3) { 
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
    exit(0); 
  } 

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[2]), argv[1]);

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }
  // Get input message from user
  printf("CLIENT: Enter text to send to the server, and then hit enter: ");
  // Clear out the buffer array
  memset(buffer, '\0', sizeof(buffer));
  // Get input from the user, trunc to buffer - 1 chars, leaving \0
  fgets(buffer, sizeof(buffer) - 1, stdin);
  // Remove the trailing \n that fgets adds
  buffer[strcspn(buffer, "\n")] = '\0'; 

  // Send message to server
  // Write to the server
  charsWritten = send(socketFD, buffer, strlen(buffer), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(buffer)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

  // Get return message from server
  // Clear out the buffer again for reuse
  memset(buffer, '\0', sizeof(buffer));
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
  printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

  // Close the socket
  close(socketFD); 
  return 0;
}
============================================================================================================================
Author: CS 344 Explorations
URL: https://replit.com/@cs344/83serverc
Code:
// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead;
  char buffer[256];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

    printf("SERVER: Connected to client running at host %d port %d\n", 
                          ntohs(clientAddress.sin_addr.s_addr),
                          ntohs(clientAddress.sin_port));

    // Get the message from the client and display it
    memset(buffer, '\0', 256);
    // Read the client's message from the socket
    charsRead = recv(connectionSocket, buffer, 255, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
    }
    printf("SERVER: I received this from the client: \"%s\"\n", buffer);

    // Send a Success message back to the client
    charsRead = send(connectionSocket, 
                    "I am the server, and I got your message", 39, 0); 
    if (charsRead < 0){
      error("ERROR writing to socket");
    }
    // Close the connection socket for this client
    close(connectionSocket); 
  }
  // Close the listening socket
  close(listenSocket); 
  return 0;
}
================================================================================================================================
Author: CS 344 Explorations
URL: https://replit.com/@cs344/84selectc
Code:
#define TIMEOUT_SEC 25

/*
* This program watches stdin to see when it has input. The program will wait for up to 50 seconds.
*/
int main(void){
  fd_set readFDs;
  struct timeval timeToWait;
  int retval;

  // Watch stdin (FD 0) to see when it has input
   // Zero out the set of possible read file descriptors
  FD_ZERO(&readFDs);
  // Mark only FD 0 as the one we want to pay attention to
  FD_SET(0, &readFDs); 

  // Wait up to TIMEOUT_SEC seconds
  timeToWait.tv_sec = TIMEOUT_SEC;
  timeToWait.tv_usec = 0;

  // Check to see whether any read FDs we have set have data!
  // We are only watching FD 0, so we set nfds to 1.
  retval = select(1, &readFDs, NULL, NULL, &timeToWait); 

  if (retval == -1){
    perror("select()");
  } else if (retval){
    printf("Data is available now!\n"); 
    // FD_ISSET(0, &readFDs) will return true
    printf("FD_ISSET for FD 0 returns: %d\n", FD_ISSET(0, &readFDs));
  }
  else{
    printf("No data within %d seconds\n", TIMEOUT_SEC);
  }
  return(0);
}
====================================================================================
Author: CS 344 Explorations
URL: http://beej.us/guide/bgnet/html/#sendall
https://man7.org/linux/man-pages/man2/recv.2.html
https://man7.org/linux/man-pages/man2/send.2.html
http://beej.us/guide/bgnet/
https://replit.com/@cs344/81dnsbyname
https://replit.com/@cs344/82socketaddressc
https://replit.com/@cs344/71pipesc
https://replit.com/@cs344/71fifoc
Concepts and readings read 
Code:
int main(){
  // Buffer that will be used to read data from the pipe.
  char readBuffer[10];
  // Buffer that will store the complete message read from the pipe.
  char completeMessage[512] = {0};

  int bytesRead;

  char* FIFOfilename = "myNewFifo";
  // Create the FIFO
  int newfifo = mkfifo(FIFOfilename, 0640);

  // Open the FIFO for reading
  int fd = open(FIFOfilename, O_RDONLY); 
  if (fd == -1) { 
    perror("Reader: open()"); 
    exit(1); 
  }
  while (strstr(completeMessage, "@@") == NULL){
    // Clear the read buffer before reading from the pipe
    memset(readBuffer, '\0', sizeof(readBuffer));
    // Read the next chunk of data from the pipe
    // We read one byte less than the size of readBuffer because the readBuffer is initalized to '\0' and this way the chunk we read becomes a null terminated string.
    bytesRead = read(fd, readBuffer, sizeof(readBuffer) - 1);

    // Add the chunk we read to what we have read up to now
    strcat(completeMessage, readBuffer); 
    printf("Reader: Chunk received: \"%s\" \n", 
            readBuffer);
    printf("Reader: Total received till now: \"%s\"\n", completeMessage);
    // Check for errors
    if (bytesRead == -1) { 
      // -1 indicates an error
      printf("bytesRead == -1\n"); 
      break; 
    } 
    if (bytesRead == 0) {
      // 0 indicates an error or end-of-file
      printf("bytesRead == 0\n"); 
      break; 
    }
  }
  // Find the location of the terminal indicator @@ and remove it from the message by replacing by the null character
  char* terminalLoc = strstr(completeMessage, "@@");
  *terminalLoc = '\0';
  printf("Reader: Complete string: \"%s\"\n", completeMessage);

  remove(FIFOfilename); // Delete the FIFO
}
=====================================
Code:
int main(){
  char stringToWrite[21] = "WRITER: Hi reader!@@";
  char* FIFOfilename = "myNewFifo";

  // Sleep for a short while to allow the other process to create the FIFO
  sleep(10);

  // Open the FIFO for writing
  int fd = open(FIFOfilename, O_WRONLY); 
  if (fd == -1) { 
    perror("Writer: open()"); 
    exit(1); 
  }
  // Write the entire string into the FIFO
  write(fd, stringToWrite, strlen(stringToWrite));
  // Terminate the prcoess
  exit(0);
}
============================================================================
Code:
sem_t binary_sem;

int counter = 0;
void* perform_work(void* argument){
  for(int i = 0; i < COUNT_TO; i++){

    // Critical section starts
    counter += 1;
    // Critical section ends

  }
  return NULL;
}

int main(void){
  // Initialize the semaphore
  // The 2nd argument is 0 because the semaphore will be shared among threads
  // The 3rd argument is 1, i.e., the intial value of the semaphore will be 1
  int res = sem_init(&binary_sem, 0, 1);

  pthread_t threads[NUM_THREADS];

  for(int i = 0; i < NUM_THREADS; i++){
    pthread_create(&threads[i], NULL, perform_work, NULL);
  }
  for(int i = 0; i < NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
  }
  // Destroy the semphore
  sem_destroy(&binary_sem);

  printf("Expected value of counter = %d\n", COUNT_TO * NUM_THREADS);
  printf("Actual value of counter = %d\n", counter);
  exit(EXIT_SUCCESS);
}
==================================================
Code:
nt main(int argc, char *argv[]) {
  // Check usage & args
  if (argc != 2) { 
    fprintf(stderr,"USAGE: %s $hostname \n", argv[0]); 
    fprintf(stderr,"Example: %s flip.engr.oregonstate.edu\n", argv[0]);
    exit(0); 
  }

  // Get the DNS entry associated with the domain name
  struct hostent *hostptr = gethostbyname(argv[1]);

  if(hostptr == NULL){
     printf("Couldn't locate %s\n", argv[1]);
     exit(0);
  }

  // We successfully looked up the DNS entry
  printf("Official domain name: %s\n", hostptr->h_name);

  // Print all aliases for this domain name
  int i = 0;
  printf("Aliases:\n");
  while(hostptr->h_aliases[i] != NULL){
    printf("\t%s\n", hostptr->h_aliases[i]);
    i++;
  }

  // Print all IP addresses, in dotted decimal notation, for this domain name
  int j = 0;
  printf("Adresses:\n");
  struct in_addr addr;
  while(hostptr->h_addr_list[j] != NULL){
    addr.s_addr = *(unsigned int *) hostptr->h_addr_list[j];
    printf("\t%s\n", inet_ntoa(addr));
    j++;
  }
  return 0;
}
=================================================================================
