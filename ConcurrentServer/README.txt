Greetings!  This is a concurrent, multithreaded server and client written in C.  The server responds to SQL requests sent from the Client and sends back the results to the Clients.  It involves using mutexes to prevent synchronized access to the SQL database by multiple clients.  

In order to run this program correctly, use the Makefile to compile all four .c files.  Start up the Server by using the command:

 ./Server.c *enter PORT to bind to*

Then, start up each Client by using the command

./ClientX.c *enter IP address of server* *enter PORT of server here*