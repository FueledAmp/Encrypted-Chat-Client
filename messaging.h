/******************************************************************************/
/* BNC Chatroom Server                                                        */
/* Version: 0.1 (Prototype)                                                   */
/* Author: YourPhobia                                                         */
/* File: include/messages.h                                                   */
/******************************************************************************/
#define MESSAGE_BUFFER_SIZE 1024

// Send message only to the sender, for warnings and such.
void alertUser(const char *message, int connection_fd){
  // Write the message.
	write(connection_fd, message, strlen(message));
}

// Send message to all clients except sender.
void sendMessage_excludingSender(char *message, int userID){
	size_t i;

  // Iterate through the list of clients.
	for(i = 0; i < MAX_CLIENTS; i++){

    // If the client is found (in this case, clients that are not the sender)...
		if(chatClients[i]){

      // If the client's userID is not the userID passed as a commandParametereter...
			if(chatClients[i]->userID != userID){

        // Write the message.
				write(chatClients[i]->connection_fd, message, strlen(message));
			}
		}
	}
}

// Send message to all clients.
void sendMessage_AllClients(char *message){
	size_t i;

  // Iterate through the list of clients.
	for(i = 0; i < MAX_CLIENTS; i++){

    // If the client is found (in this case all clients)...
		if(chatClients[i]){

      // Write the message.
			write(chatClients[i]->connection_fd, message, strlen(message));
		}
	}
}


// Send a private message.
void sendMessage_Private(char *message, int userID){
	size_t i;

  // Iterate through the list of clients.
	for(i = 0;i < MAX_CLIENTS; i++){

		// If client is found...
		if(chatClients[i]){

			// If client's userID matches the userID passed as a commandParametereter
			if(chatClients[i]->userID == userID){
				// Write the message.
				write(chatClients[i]->connection_fd, message, strlen(message));
			}
		}
	}
}

// List active clients.
void listActiveClients(int connection_fd){
	size_t i;
	char message[MESSAGE_BUFFER_SIZE]; // Message buffer

	for(i = 0;i < MAX_CLIENTS; i++){

		if(chatClients[i]){
			sprintf(message, "[*] UserID: %d | Username: %s\r\n", chatClients[i]->userID, chatClients[i]->username);
			alertUser(message, connection_fd);
		}
	}
}

// Security measure to ensure against CRLF Injection.
void stripNewLine(char *messageChar){

  // While the message character is not a null terminator...
	while(*messageChar != '\0'){

    // If the message character is a carriage return or newline...
		if(*messageChar == '\r' || *messageChar == '\n'){

      // Terminate the message.
			*messageChar = '\0';
		}

    // Increment the pointer to character in the message.
		messageChar++;
	}
}

void listCommands(char *bufferOut){
  // Concatenate the command info to the output buffer.
  strcat(bufferOut, "!disconnect\tDisconnect from chatroom.\r\n");
  strcat(bufferOut, "!test\tServer test\r\n");
  strcat(bufferOut, "!username\t<username> Change username.\r\n");
  strcat(bufferOut, "!pm <userID> <message> Send private message.\r\n");
  strcat(bufferOut, "!list\tShow active clients.\r\n");
  strcat(bufferOut, "!help\tList valid commands.\r\n");
}
