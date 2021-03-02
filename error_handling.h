
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALERT_BUFFER 100 // Buffer for error messages and success messages.

// Error function that kill the program upon receiving a serious error.
void fatalError(char *message){
	char errorMessage[ALERT_BUFFER]; // Error message buffer

	// This is temporary, this needs to be changed to prevent buffer overflows.
	strcpy(errorMessage, "[!!!] Fatal Error: ");
	strncat(errorMessage, message, 81);
	perror(errorMessage);

	// Kill the program.
	exit(1);
}

void success(char *message){
	char successMessage[ALERT_BUFFER];

	// This is temporary, this needs to be changed to prevent buffer overflows.
  strcpy(successMessage, "[+] Success: ");
  strncat(successMessage, message, 87);
	printf("%s\n", successMessage);
}
