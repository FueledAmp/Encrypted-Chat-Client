/******************************************************************************/
/* BNC Chatroom Server                                                        */
/* Version: 0.1 (Prototype)                                                   */
/* Author: YourPhobia                                                         */
/* File: include/logging.c                                                    */
/******************************************************************************/
void chatLogger (char *message){
	FILE *file; // Pointer to the log file.

  // All input is appended to the file.
  file = fopen(LOG_FILE, "a+");

  // If the file does not exist after creating it,
  // something is seriously wrong.
  if(file == NULL){
    printf("No log file was found?? WTF?\n");
    exit(1);
  }

  // Print the message to the log file.
  fprintf(file,"%s\n", message);

  // Close the file.
  fclose(file);


}
