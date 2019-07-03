#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>

double LinkID;
double pktsize;
double power;
double Tt;
double Tp;
double Delay;

int main()
{
    while(1)
	{
		//create socket for monitor using TCP connection
 		int monitor_socket = socket(AF_INET, SOCK_STREAM,0);

 		//specify the IP address and port number of monitor
	 	struct sockaddr_in server_address;
 		server_address.sin_family = AF_INET;
	 	server_address.sin_port = htons(26517);
 		server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

 		//connect to AWS
		connect(monitor_socket, (struct sockaddr *) &server_address, sizeof(server_address));
 		printf("The monitor is up and running.\n\n");

		//receive client's input from AWS
		recv(monitor_socket, &LinkID, sizeof(LinkID), 0);
		recv(monitor_socket, &pktsize, sizeof(pktsize), 0);
		recv(monitor_socket, &power, sizeof(power), 0);
		
		printf("The monitor received Link ID = <%d>, size = <%.2lf>, and power = <%.2lf> from the AWS.\n\n", (int)LinkID, pktsize, power);

		//reveive match_flag from aws
		double match_flag;
		recv(monitor_socket, &match_flag, sizeof(match_flag), 0);
		recv(monitor_socket, &Tt, sizeof(Tt), 0);
		recv(monitor_socket, &Tp, sizeof(Tp), 0);
		recv(monitor_socket, &Delay, sizeof(Delay), 0);

		if (match_flag == 1)
		{
			//receive all data from aws
			recv(monitor_socket, &Tt, sizeof(Tt), 0);
			recv(monitor_socket, &Tp, sizeof(Tp), 0);
			recv(monitor_socket, &Delay, sizeof(Delay), 0);
			printf("The results for for link <%d>:\nTt = <%.2e> ms\nTp = <%.2e> ms\nDelay = <%.2e> ms\n\n", (int)LinkID, Tt, Tp, Delay);
			//close(monitor_socket);
		} else {
			printf("Found no match for link <%d>.\n\n", (int)LinkID);
			//close(monitor_socket);
		}
		close(monitor_socket);
	}
}
