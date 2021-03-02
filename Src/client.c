
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "../include/client.h"
#include "../include/error_handling.h"

#define DEFAULT_PORT 10000

int main(int argc, char *argv[]){
	int sockfd; // The client socket file descriptor.
	int inputLength = 0;
	int ret;
	struct sockaddr_in hostAddr; // Information pertaining to the routing.
	char bufferIn[MESSAGE_BUFFER_SIZE]; // Buffer for recieving server responses.
	char bufferOut[MESSAGE_BUFFER_SIZE]; // Buffer for sending messages to the server.

	memset(bufferIn, '\0', sizeof(bufferIn));

	// Make sure the socket works.
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fatalError("cannot connect to socket");
	} else {
		success("client socket connected!");
	}

	// Set the routing information.
	memset(&hostAddr, '\0', sizeof(hostAddr));
	hostAddr.sin_family = AF_INET; // Host byte order
	hostAddr.sin_port = htons(DEFAULT_PORT); // Network byte order
	hostAddr.sin_addr.s_addr = INADDR_ANY; // Automatically fill with the server's IP.

	// No pipe signals.
	signal(SIGPIPE, SIG_IGN);

	// If the connection is set up properly...
	if((ret = connect(sockfd, (struct sockaddr*)&hostAddr, sizeof(hostAddr))) < 0){
		fatalError("connected failed.");
	} else {
		success("connected to server!");
	}

	// Infinite loop for sending and receiving messages.
	while((inputLength = write(sockfd, bufferIn, sizeof(bufferIn)-1)) > 0){
		bufferIn[inputLength] = '\0'; // Add a null terminator.
		bufferOut[0] = '\0'; // Add a null terminator.

		if(read(sockfd, bufferIn, MESSAGE_BUFFER_SIZE) < 0){
			fatalError("error in reading data.");

		} else {
			printf("%s\n", bufferIn);
		}

		// Strip newline characters.
		stripNewLine(bufferOut);

		//Send some data
		if(write(sockfd, bufferOut, strlen(bufferOut)) < 0){
				fatalError("error in sending data.");

		} else {
			
		}
	}

	return 0;

}
