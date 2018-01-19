//BY ANDREW MCCAMENT//
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <sqlite3.h>
#include <time.h>


//Initialize the lock, log files etc
pthread_mutex_t dbLock = PTHREAD_MUTEX_INITIALIZER;
FILE *log;
time_t tod;

struct serverValue { int connection;  };

static int callback(int socket, int argc, char **argv, char **azColName) {
	int i;

	for (i = 0; i < argc; i++) 
	{
		//send the output chunk by chunk
		send(socket, "\n", strlen("\n"), 0);
		send(socket, azColName[i], strlen(azColName[i]), 0);
		send(socket, " = ", strlen(" = "), 0);
		send(socket, argv[i], strlen(argv[i]), 0);
	}

	return 0;
}


void *server(void *parameter)
{
	#define thisServer ((struct serverValue *) parameter)

	int messageLength = 0;
	char message[4096];
	sqlite3 *db;
	char *error = 0;
	char *back = "Server: Message Received";
	sqlite3_config(SQLITE_CONFIG_SERIALIZED);


	//open the SQL database
	if (sqlite3_open("a4.db", &db))
	{
		printf("Server: Couldn't open database\n");
		close(thisServer->connection);
		free(thisServer);
		return 0;
	}
	else
	{
		printf("Server: Opened database successfully\n");
	}

	if (thisServer->connection < 0)
	{
		perror("Server: Failure to accept connection.  Exiting...");
		return(0);
	}
	else printf("Server: Accepted connection\n");

	//start listening for commands
	while (1)
	{
		//read in the message
		messageLength = read(thisServer->connection, message, sizeof(message) - 1);

		//while we're getting something
		if (messageLength != 0)
		{
			//we lock this entire function so we can write to the file and database without other threads/clients messing us up
			pthread_mutex_lock(&dbLock);
			log = fopen("a4ServerLog.txt", "a");

			tod = time(NULL);

			//display time of day, thread ID and process ID
			printf("Time of Day: %s", ctime(&tod));
			printf("Thread ID: %d\n", pthread_self());
			printf("Process ID: %d\n\n", getpid());
			fprintf(log, "Time of Day: %s", ctime(&tod));
			fprintf(log, "Thread ID: %d\n", pthread_self());
			fprintf(log, "Process ID: %d\n\n", getpid());

			//null terminate
			message[messageLength] = '\0';

			//print success messages
			printf("Server: Message received: %s\n", message);
			fprintf(log, "Server: Message received: %s\n", message);

			//tell the client we got it
			send(thisServer->connection, back, strlen(back), 0);
			send(thisServer->connection, "\n", strlen("\n"), 0);

			//now the database is open, so we execute whatever command the client wants
			while (1) //we may have to wait for the database to be ready
			{ 
					if (sqlite3_exec(db, message, callback, thisServer->connection, &error) != SQLITE_OK)
					{
						fprintf(stderr, "SQL error: %s\n", error);
						fprintf(log, "SQL error: %s\n", error);
						sqlite3_free(error);
						break;
					}
					else
					{
						printf("Command Executed Successfully: %s\n\n\n", message);	
						fprintf(log, "Command Executed Successfully: %s\n\n\n", message);
						break;
					}
			}

			fclose(log);
			pthread_mutex_unlock(&dbLock);

			//we've already used message so clear out the buffer
			memset(message, '\0', sizeof(message));
			messageLength = 0;
		}
	}

	//cleanup
	//if the user control Zs or Cs we may never reach this point...
	sqlite3_close(db);
	close(thisServer->connection);
	free(thisServer);
	return(0);
}


main(int argc, char *argv[])
{
	int thisSocket;
	struct sockaddr_in thisAddress;
	struct serverValue *myServer;
	int thisConnection;
	pthread_t thisThread;

	thisSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (thisSocket < 0) {
		perror("Server: Error opening socket.");
		exit(1);
	}
	printf("\nServer: Opened Socket");

	//start fresh
	remove("a4ServerLog.txt");

	//set up our new socket
	thisAddress.sin_family = AF_INET;
	thisAddress.sin_addr.s_addr = INADDR_ANY;
	thisAddress.sin_port = htons(atoi(argv[1]));

	//bind
	if (bind(thisSocket, (struct sockaddr *) &thisAddress, sizeof(thisAddress)) < 0)
	{
		perror("Binding: ");
	}
	else perror("Binding: ");

	//now we're listening
	if (listen(thisSocket, 5) != 0) {
		perror("Error listening"); 
		exit(1);
	}
	else perror("Server: Listening");

	//now we wait for new clients 
	while (1)
	{
		//accept the new client for who they are
		thisConnection = accept(thisSocket, NULL, NULL);

		//now we create the thread for this new client
		myServer = (struct serverValue *)malloc(sizeof(struct serverValue));

		myServer->connection = thisConnection;

		//now we devote an entire thread just for this one client
		int newThread = pthread_create(&thisThread, NULL, server, (void *)myServer);
		if (newThread != 0)
		{
			perror("Server: Could not create the thread for the new client");
			close(thisConnection);
			close(thisSocket);
			exit(1);
		}
	}

	//free up lock memory (again, probably won't reach this point?)
	pthread_mutex_destroy(&dbLock);
	free(&dbLock);
}