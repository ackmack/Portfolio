#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

int main(int argc, char *argv[])
{	
	int clientSock;
	struct sockaddr_in serverAddress;
	int PORT = htons(atoi(argv[2]));
	int IP = inet_addr(argv[1]);
	char servReturn[4096]; //the results from the server
	int messageLength = 0;
	char** message = "select * from classics;"; //this client's command
	time_t tod;
	pthread_t myThread = pthread_self();

	//deal with file IO
	FILE *log;
	remove("a4Client1Log.txt");//start fresh

	//create a client socket
	if ((clientSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Client: Socket");
		exit(2);
	}

	//memory devoted to server
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = IP; //sets IP to argument 1
	serverAddress.sin_port = PORT; //sets PORT to argument 2

	printf("\nClient: Socket created\n");

	if (connect(clientSock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("Client: Connection Error");
		exit(3);
	}

	printf("Client: Connected to server\n\n");

	while (1)
	{	
		if (send(clientSock, message, strlen(message), 0) > 0)
		{
			printf("Sent the SQL command %s\n\n", message);
			
		}
		else
		{
			perror("Could not send the message: ");
			break;
		}

		//receive message from server
		while (1)
		{
			messageLength = read(clientSock, servReturn, sizeof(servReturn) - 1);

			//while we're getting something
			if (messageLength != 0)
			{
				tod = time(NULL);
				log = fopen("a4Client1Log.txt", "a");

				//display time of day, thread ID and process ID
				printf("Time of Day: %s", ctime(&tod));
				printf("Thread ID: %d\n", myThread);
				printf("Process ID: %d\n\n", getpid());
				fprintf(log, "Time of Day: %s", ctime(&tod));
				fprintf(log, "Thread ID: %d\n", myThread);
				fprintf(log, "Process ID: %d\n\n", getpid());

				//append mode so that we can close and open every time loop runs


				//null terminate
				servReturn[messageLength] = '\0';
				printf("Client: Message from server: \n\n%s\n\n", servReturn);
				fprintf(log, "Client: Message from server: \n\n%s\n\n", &servReturn);

				//we've already used message so clear out the buffer
				memset(servReturn, '\0', sizeof(servReturn));

				tod = time(NULL);
				//display time of day, thread ID and process ID
				printf("Time of Day: %s", ctime(&tod));
				printf("Thread ID: %d\n", myThread);
				printf("Process ID: %d\n\n", getpid());
				fprintf(log, "Time of Day: %s", ctime(&tod));
				fprintf(log, "Thread ID: %d\n", myThread);
				fprintf(log, "Process ID: %d\n\n", getpid());

				fclose(log);
				sleep(3);
				break;
			}
		}
	}
	
	return 0;
	exit(0);
}