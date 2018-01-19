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
	char** message1 = "DELETE from CLASSICS where ISBN = 9781598184891"; //this client's command1
	char** message2 = "INSERT INTO `classics` (`author`, `title`, `category`, `year`, `isbn`) VALUES('Mark Twain', 'The Adventures of Tom Sawyer', 'Fiction', '1876', '9781598184891')"; //this client's command1

	time_t tod;
	pthread_t myThread = pthread_self();

	//deal with file IO
	FILE *log;
	remove("a4Client3Log.txt"); //start fresh

	//create a client socket
	if ((clientSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Client: Socket");
		exit(2);
	}

	//devote memory to our server
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = IP; //sets IP to argument 1
	serverAddress.sin_port = PORT; //sets PORT to argument 2

	printf("\nClient: Socket created\n");

	//connect to the server
	if (connect(clientSock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		perror("Client: Connection Error");
		exit(3);
	}

	printf("Client: Connected to server\n\n");

	while (1)
	{
		//try to send command1
		if (send(clientSock, message1, strlen(message1), 0) > 0)
		{
			printf("Sent the SQL command %s\n\n", message1);

		}
		else
		{
			perror("Could not send the message: ");
			break;
		}

		//receive the message from our server
		while (1)
		{
			//read in the message
			messageLength = read(clientSock, servReturn, sizeof(servReturn) - 1);

			if (messageLength != 0)
			{
				tod = time(NULL);
				log = fopen("a4Client3Log.txt", "a");

				//display time of day, thread ID and process ID
				printf("Time of Day: %s", ctime(&tod));
				printf("Thread ID: %d\n", myThread);
				printf("Process ID: %d\n\n", getpid());
				fprintf(log, "Time of Day: %s", ctime(&tod));
				fprintf(log, "Thread ID: %d\n", myThread);
				fprintf(log, "Process ID: %d\n\n", getpid());

				//null terminate
				servReturn[messageLength] = '\0';
				printf("Client: Message from server: \n\n%s\n\n", servReturn);
				fprintf(log, "Client: Message from server: \n\n%s\n\n", &servReturn);

				//we've already used message so clear out the buffer
				memset(servReturn, '\0', sizeof(servReturn));
				break;
			}
		}

		//wait two seconds
		sleep(2);

		//now we send the second command
		if (send(clientSock, message2, strlen(message2), 0) > 0)
		{
			printf("Sent the SQL command %s\n\n", message2);

		}
		else
		{
			perror("Could not send the message: ");
			break;
		}

		//again lets hear what our server has to say
		while (1)
		{
			//read in the message
			messageLength = read(clientSock, servReturn, sizeof(servReturn) - 1);

			if (messageLength != 0)
			{
				tod = time(NULL);
				log = fopen("a4Client3Log.txt", "a");

				//display time of day, thread ID and process ID
				printf("Time of Day: %s", ctime(&tod));
				printf("Thread ID: %d\n", myThread);
				printf("Process ID: %d\n\n", getpid());
				fprintf(log, "Time of Day: %s", ctime(&tod));
				fprintf(log, "Thread ID: %d\n", myThread);
				fprintf(log, "Process ID: %d\n\n", getpid());

				//null terminate
				servReturn[messageLength] = '\0';
				printf("Client: Message from server: \n\n%s\n\n", servReturn);
				fprintf(log, "Client: Message from server: \n\n%s\n\n", &servReturn);

				//we've already used message so clear out the buffer
				memset(servReturn, '\0', sizeof(servReturn));
				break;
			}
		}

		fclose(log);
		sleep(2);
	}

	return 0;
	exit(0);
}