#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

double Delay; //final delay result

int main(int argc, char *argv[]) //input value in terminal window
{
	//convert char in "argv[]" to double
 	double linkID = atof(argv[1]);
 	double pktsize = atof(argv[2]);
 	double SPdbm = atof(argv[3]);

 	//creatre a client socket that used to connect the AWS
 	int client_socket = socket(AF_INET, SOCK_STREAM, 0);

 	if (client_socket == -1)
 	{
 		printf("client_socket creation error.\n\n");
 	} else {
 		printf("The client is up and running.\n\n");
 	}

 	//specify the IP address and port number
 	struct sockaddr_in server_address;

	memset(&server_address, 0, sizeof(server_address));

 	server_address.sin_family=AF_INET;
 	server_address.sin_port = htons(25517);
 	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

 	//connect to AWS
 	connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	//send input values to the AWS
	send(client_socket, &linkID, sizeof(linkID), 0);
	send(client_socket, &pktsize, sizeof(pktsize), 0);
	send(client_socket, &SPdbm, sizeof(SPdbm), 0); 
	printf("The client sent ID= <%d>, size= <%.2lf>, and power= <%.2lf> to AWS.\n\n", (int)linkID, pktsize, SPdbm);

	//receive final results from AWS
	double match_flag; //match_flag to see if found data in database_a or database_b
	recv(client_socket, (char *) &match_flag, sizeof(match_flag), 0);

	if (match_flag==1){
		//receive end-to-end Delay from aws
		recv(client_socket, (char *) &Delay, sizeof(Delay), 0);
		printf("The Delay for link <%d> is <%.2e> ms.\n\n", (int)linkID, Delay);
	} else {
		printf("Found no match for link <%d>.\n\n", (int)linkID);
	}
close (client_socket);
 	
}

