
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "../include/client.h"
#include "../include/server_settings.h"
#include "../include/error_handling.h"
#include "../include/logging.h"
#include "../include/messaging.h"
#include "../include/security/crypto/crypto.h"

static unsigned int clientCount = 0; // Keep track of the number of clients.

// Handler function prototype.
void *clientHandler(void *clientToHandle);

// Magic starts here.
int main(int argc, char *argv[]){

	int listenfd = 0, connection_fd = 0; // File descriptors.
	struct sockaddr_in hostAddress; // Host Remote Address.
	struct sockaddr_in clientAddress; // Client Remote Address.

	int hostPort; // Host port.
	pthread_t threadID; // Thread ID.

	// Check for command-line arguments.
	if(argc >= 2){
			printf("[!] Command line argument parsing is not set up yet.\n");
			return 1;

	} else {
		printf("[!] No command line arguments were provided.\n");
		printf("[!] Using default settings.\n\n");
	}

	hostPort = DEFAULT_PORT

	// All the connection information.
	listenfd = socket(AF_INET, SOCK_STREAM, 0); // TCP.
	hostAddress.sin_family = AF_INET; // IPv4
	hostAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Automatically fill in hostAddress.
	hostAddress.sin_port = htons(hostPort); // Set port.

	// No pipe signals.
	signal(SIGPIPE, SIG_IGN);

	// Make sure the socket can bind.
	if(bind(listenfd, (struct sockaddr*)&hostAddress, sizeof(hostAddress)) < 0){
		fatalError("socket could not bind.");
	}

	// No problems.
	success("socket has binded.");

	// Make sure socket listening is possible.
	if(listen(listenfd, 10) < 0){
		fatalError("socket listen failed.");
	}

	// No problems.
	success("socket is listening.");

	printf("+---------------------------------------------------------------+\n");
	printf("\t\t\tSERVER STARTED on:\n\t\t\t");
	printIP(hostAddress);
	printf("+---------------------------------------------------------------+\n");

	// Infinite loop for server to accept clients.
	while(1){
		socklen_t clientLength = sizeof(clientAddress); // Length of the client address.
		connection_fd = accept(listenfd, (struct sockaddr*)&clientAddress, &clientLength);

		// Check if the max number of clients has been reached.
		if((clientCount + 1) == MAX_CLIENTS){
			printf("[!] Max Number of Clients Reached!\n");
			printf("[!] Rejected connection from: ");
			printIP(clientAddress);
			printf("\n");

			// Close connection with the client.
			close(connection_fd);
			continue;
		}

		// Client Settings

		// Allocate memory for each client that connects.
		chatClient_t *client = (chatClient_t *)malloc(sizeof(chatClient_t));
		client->address = clientAddress; // Set the client's remote address.
		client->connection_fd = connection_fd; // Set the client's file descriptor.

		// This line is temporary, it will need to be changed to be some sort of hash.
		client->userID = generateUserID(); // Increment the client's userID for unique addressing.
		sprintf(client->username, "%d", client->userID);

		// Add client and create a thread for it.
		addClient(client);
		pthread_create(&threadID, NULL, &clientHandler, (void*)client);
	}
}


/******************************************************************************/
/* Client Handler                                                             */
/******************************************************************************/

// Handle the communcations with the client.
void *clientHandler(void *clientToHandle){
	char bufferOut[MESSAGE_BUFFER_SIZE]; // Buffer for all outgoing messages.
	char bufferIn[MESSAGE_BUFFER_SIZE]; // Buffer for all incoming messages.
	int inputLength; // This is needed for reading.

	// Increment the number of clients.
	clientCount++;
	chatClient_t *client = (chatClient_t *)clientToHandle; // Define the client as the the proper data structure.

	// For clients joining the chatroom.
	printf("[!] New client has connected, IP address is: ");

	// Print client's IP address and information.
	printIP(client->address);
	printf("[!] User ID is: %d\n", client->userID);

	// Greet the client on the server-side.
	sprintf(bufferOut, "[!] A new user has joined the chat, %s.\r\n", client->username);
	sendMessage_AllClients(bufferOut);

	// Client input
	while((inputLength = read(client->connection_fd, bufferIn, sizeof(bufferIn)-1)) > 0){
		bufferIn[inputLength] = '\0'; // Add a null terminator.
	  bufferOut[0] = '\0'; // Add a null terminator.

		// Strip newline characters.
		stripNewLine(bufferIn);

		// If the client has typed something under 1024 characters long.
		if(strlen(bufferIn) && strlen(bufferIn) <= MESSAGE_BUFFER_SIZE){
			// Command prefex is '!'.
			if(bufferIn[0] == '!'){
				char *roomCommand, *commandParameter;
				roomCommand = strtok(bufferIn," ");

				// If the client requests to disconnect,
				// break the loop and proceed to disconnecting them from the server.
				if(!strcmp(roomCommand, "!disconnect")){
					break;

				} else if(!strcmp(roomCommand, "!test")){
					// Test the server.
					alertUser("[!] You have just pinged the server, it has responded.\r\n", client->connection_fd);

				} else if(!strcmp(roomCommand, "!username")){
					// Change client's username.
					commandParameter = strtok(NULL, " ");

					if(commandParameter){

						// Check the string length
						if(strlen(commandParameter) <= MAX_USERNAME_LENGTH){
							char *old_username = strdup(client->username); // Store old username.

							// Overwrite client's username with new username.
							strcpy(client->username, commandParameter);

							// Print the action to the server.
							sprintf(bufferOut, "[!] %s changed username to %s\r\n", old_username, client->username);

							// Free the memory allocated for the old username.
							free(old_username);

							// Inform all clients of the change.
							sendMessage_AllClients(bufferOut);

						} else {
							// To prevent buffer overflows, stop the user from changing to an
							// unnecessarily long username.
							alertUser("[!] Usernames can only be up to 32 characters long.\r\n", client->connection_fd);
						}

					} else {
						// This will fire when a user does not provide a username to change to.
						alertUser("[!] Username cannot be null.\r\n", client->connection_fd);
					}

				} else if(!strcmp(roomCommand, "!pm")){
					// Message a user privately.
					commandParameter = strtok(NULL, " "); // Allow a space before text.

					// If the command parameter is provided...
					if(commandParameter){
						int userID = atoi(commandParameter); // Convert ID from acsii to integer.
						commandParameter = strtok(NULL, " "); // Allow a space before text.

						// If nothing has gone wrong...
						if(commandParameter){
						// Print the sender's username.
						sprintf(bufferOut, "[PM][%s]", client->username);

							// While the command parameter exists...
							while(commandParameter != NULL){
								// Add a space to the buffer.
								strcat(bufferOut, " ");

								// Copy the parameter to the output buffer.
								strcat(bufferOut, commandParameter);
								commandParameter = strtok(NULL, " "); // Allow a space before text.
							}

							// Concatenate a carriage return and a newline to the output buffer.
							strcat(bufferOut, "\r\n");

							// Send the private message to the user specified.
							sendMessage_Private(bufferOut, userID);

						} else {
							// This will fire if the client does not provide a message to send.
							alertUser("[!] Message cannot be null\r\n", client->connection_fd);
						}

					} else {
						// This will fire if the client does not provide a recipient to send to.
						alertUser("[!] Recipient cannot be null\r\n", client->connection_fd);
					}

				} else if(!strcmp(roomCommand, "!list")){
					// List active clients.
					sprintf(bufferOut, "[!] Active Clients: %d\r\n", clientCount);

					// Send a message to the client and list the active clients.
					alertUser(bufferOut, client->connection_fd);
					listActiveClients(client->connection_fd);

				} else if(!strcmp(roomCommand, "!help")){

					// List the valid commands.
					listCommands(bufferOut);

					// Send the help menu to the client.
					alertUser(bufferOut, client->connection_fd);

				} else {
					// This will fire when the client tried to execute a command that does not exist.
					alertUser("[!] Invalid Command. Try !help.\r\n", client->connection_fd);
				}

			} else {
				// Send the client's message. All went well.
				sprintf(bufferOut, "[%s] %s\r\n", client->username, bufferIn);
				sendMessage_excludingSender(bufferOut, client->userID);
				chatLogger(bufferOut); // Log the message.
			}

		} else {
			// Buffer is empty, ignore it.
			continue;
		}
	}

	// Close the client's connection with the server.
	close(client->connection_fd);

	// Tell the server a client has disconnected.
	sprintf(bufferOut, "[!] %s has disconnected. \r\n", client->username);

	// Send the output buffer to all clients.
	sendMessage_AllCl12nts(bufferOut);

	// Remove client and detach the thread.
	removeClient(client->userID);
	printf("[!] A user has left the room: %s.\n", client->username);

	// Free memory allocated for the client.
	free(client);

	// Decrement the number of active clients.
	clientCount--;

	// Detach the thread attached to the client.
	pthread_detach(pthread_self());

	// Kills the function thread.
	return NULL;
}
