/******************************************************************************/
/* BNC Chatroom Server                                                        */
/* Version: 0.1 (Prototype)                                                   */
/* Author: YourPhobia                                                         */
/* File: include/client.h                                                     */
/******************************************************************************/
#include "server_settings.h"

#define MIN_ID_NUMBER 000000
#define MAX_ID_NUMBER 999999

// Client Data Structure.
typedef struct {
	struct sockaddr_in address; // Client's address
	int connection_fd; // Client connection's file descriptor
	unsigned int userID; // Client ID
	char username[100]; // Client Name
	char userRole[100]; // Client Role (Not implemented yet.)
} chatClient_t;

chatClient_t *chatClients[MAX_CLIENTS]; // List of clients.

// Function to generate random userIDs.
int generateUserID(){
    int userID = 0;
		int low_num = 0;
		int hi_num = 0;

    if (MIN_ID_NUMBER < MAX_ID_NUMBER){
        low_num = MIN_ID_NUMBER;
        hi_num = MAX_ID_NUMBER + 1;

    } else {
        low_num = MAX_ID_NUMBER + 1;
        hi_num = MIN_ID_NUMBER;
    }

    srand(time(NULL));

    userID = (rand() % (hi_num - low_num)) + low_num;

    return userID;
}


// Print Client's IP Address
void printIP(struct sockaddr_in addr){
    unsigned char bytes[4];
    bytes[0] = addr.sin_addr.s_addr & 0xFF; // First octet
    bytes[1] = (addr.sin_addr.s_addr >> 8) & 0xFF; // Second octet
    bytes[2] = (addr.sin_addr.s_addr >> 16) & 0xFF; // Third octet
    bytes[3] = (addr.sin_addr.s_addr >> 24) & 0xFF; // Fourth octet
    printf("%d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]);
}

/******************************************************************************/
/* Client Queue Functions                                                     */
/******************************************************************************/

// Add a Client to the list of clients.
void addClient(chatClient_t *client){
	size_t i;

  // Iterate through the list of clients.
	for(i = 0; i < MAX_CLIENTS; i++){

    // If the client is not found in the list of clients...
		if(!chatClients[i]){

      // Add the client to the list of clients.
			chatClients[i] = client;
			return;
		}
	}
}

// Remove a client from the list of clients.
void removeClient(int userID){
	size_t i;

  // Iterate through the list of clients.
	for(i = 0; i < MAX_CLIENTS; i++){

    // If the client is found in the list of clients...
		if(chatClients[i]){

      // If the userID matches the client's userID...
			if(chatClients[i]->userID == userID){
        // Remove the client from the list of clients.
				chatClients[i] = NULL;
				return;
			}
		}
	}
}
