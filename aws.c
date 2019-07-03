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

//varialbes received from client
double LinkID, pktsize, SPdbm;

//variables used in searchA()
double LinkID_A;
double BW_A;
double Length_A;
double Velocity_A;
double NP_A;
double match_A;

//variables used in searchB()
double LinkID_B;
double BW_B;
double Length_B;
double Velocity_B;
double NP_B;
double match_B;

//variables of calculated results
double Tp, Tt, Delay; 

double ValueA[7]; //values received from serverA and send to serverC
double ValueB[7]; //values received from serverB and send to serverC

//communicate with serverA
int searchA()
{
	//**************************************************************************************************
	struct sockaddr_in serverA_send_address, serverA_recv_address;

	memset(&serverA_send_address, 0, sizeof(serverA_send_address));
	memset(&serverA_recv_address, 0, sizeof(serverA_recv_address));
 	
	serverA_send_address.sin_family = AF_INET;
	serverA_send_address.sin_port = htons (21517);
	serverA_send_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	serverA_recv_address.sin_family = AF_INET;
	serverA_recv_address.sin_port = htons (24517);
	serverA_recv_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	//*******************************************************************************************************
	//create socket to send data
	int serverA_socket = socket(AF_INET, SOCK_DGRAM, 0);

	//bind IP address and port number to serverA_socket_send
  	bind(serverA_socket, (const struct sockaddr*) &serverA_recv_address, sizeof(serverA_recv_address));

  	socklen_t serverA_socket_send_len;
	serverA_socket_send_len = sizeof(serverA_send_address);

	socklen_t serverA_socket_recv_len;
	serverA_socket_recv_len = sizeof(serverA_recv_address);

  	//*******************************************************************************************************

	//send
	sendto(serverA_socket, &LinkID, sizeof(LinkID), 0, (const struct sockaddr *) &serverA_send_address, serverA_socket_send_len);
	printf("The AWS sent link ID= <%d> to Backend-Server A using UDP over port <24517>.\n\n", (int)LinkID);

	//receive
	recvfrom(serverA_socket, &match_A, sizeof(match_A), 0, (struct sockaddr *) &serverA_recv_address, &serverA_socket_recv_len);	
	recvfrom(serverA_socket, &LinkID_A, sizeof(LinkID_A), 0, (struct sockaddr *) &serverA_recv_address, &serverA_socket_recv_len);
	recvfrom(serverA_socket, &BW_A, sizeof(BW_A), 0, (struct sockaddr *) &serverA_recv_address, &serverA_socket_recv_len);
	recvfrom(serverA_socket, &Length_A, sizeof(Length_A), 0, (struct sockaddr *) &serverA_recv_address, &serverA_socket_recv_len);
	recvfrom(serverA_socket, &Velocity_A, sizeof(Velocity_A), 0, (struct sockaddr *) &serverA_recv_address, &serverA_socket_recv_len);
	recvfrom(serverA_socket, &NP_A, sizeof(NP_A), 0, (struct sockaddr *) &serverA_recv_address, &serverA_socket_recv_len);
	
	ValueA[0]=LinkID_A; //linkID_A	
	ValueA[1]=BW_A; //BW
	ValueA[2]=Length_A; //L
	ValueA[3]=Velocity_A; //V
	ValueA[4]=pktsize; //pktsize
	ValueA[5]=SPdbm; //power
	ValueA[6]=NP_A; //noise

	printf("%.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf\n\n", LinkID_A, BW_A, Length_A, Velocity_A, pktsize, SPdbm, NP_A);
	close(serverA_socket);

	return 0;
}

//communicate with serverB
int searchB()
{
	//***************************************************************************************

	//prepare info for serverB_socket_send
	struct sockaddr_in serverB_send_address, serverB_recv_address;

	memset(&serverB_send_address, 0, sizeof(serverB_send_address));
	memset(&serverB_recv_address, 0, sizeof(serverB_recv_address));

	serverB_send_address.sin_family = AF_INET;
	serverB_send_address.sin_port = htons (22517);
	serverB_send_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	serverB_recv_address.sin_family = AF_INET;
	serverB_recv_address.sin_port = htons (24517);
	serverB_recv_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	//****************************************************************************************************************************

	//create socket for search
	int serverB_socket = socket(AF_INET, SOCK_DGRAM, 0);

	//bind IP address and port number to serverB_socket_send
  	bind(serverB_socket, (const struct sockaddr*) &serverB_recv_address, sizeof(serverB_recv_address));

  	// //create socket to receive data
  	// int serverB_socket_recv = socket(AF_INET, SOCK_DGRAM, 0);
  	// //bind IP address and port number to serverB_socket_send
  	// bind(serverB_socket_recv, (const struct sockaddr*) &serverB_recv_address, sizeof(serverB_recv_address));

  	socklen_t serverB_socket_send_len;
	serverB_socket_send_len = sizeof(serverB_send_address);

	socklen_t serverB_socket_recv_len;
	serverB_socket_recv_len = sizeof(serverB_recv_address);

	//****************************************************************************************************************************

	sendto(serverB_socket, &LinkID, sizeof(LinkID), 0, (const struct sockaddr *) &serverB_send_address, serverB_socket_send_len);
	printf("The AWS sent link ID= <%d> to Backend-Server B using UDP over port <24517>.\n\n", (int)LinkID);

	//receive
	recvfrom(serverB_socket, &match_B, sizeof(match_B), 0, (struct sockaddr *) &serverB_recv_address, &serverB_socket_recv_len);	
	recvfrom(serverB_socket, &LinkID_B, sizeof(LinkID_B), 0, (struct sockaddr *) &serverB_recv_address, &serverB_socket_recv_len);
	recvfrom(serverB_socket, &BW_B, sizeof(BW_B), 0, (struct sockaddr *) &serverB_recv_address, &serverB_socket_recv_len);
	recvfrom(serverB_socket, &Length_B, sizeof(Length_B), 0, (struct sockaddr *) &serverB_recv_address, &serverB_socket_recv_len);
	recvfrom(serverB_socket, &Velocity_B, sizeof(Velocity_B), 0, (struct sockaddr *) &serverB_recv_address, &serverB_socket_recv_len);
	recvfrom(serverB_socket, &NP_B, sizeof(NP_B), 0, (struct sockaddr *) &serverB_recv_address, &serverB_socket_recv_len);

	ValueB[0]=LinkID_B; //linkID_A	
	ValueB[1]=BW_B; //BW
	ValueB[2]=Length_B; //L
	ValueB[3]=Velocity_B; //V
	ValueB[4]=pktsize; //pktsize
	ValueB[5]=SPdbm; //power
	ValueB[6]=NP_B; //noise

	//printf("%.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf\n\n", LinkID_A, BW_A, Length_A, Velocity_A, pktsize, SPdbm, NP_A);
	close(serverB_socket);

	return 0;
}

int calculation(double *temp_X)
{
	//***************************************************************************************************

	//prepare info for serverC_socket_send
	struct sockaddr_in serverC_send_address, serverC_recv_address;

	memset(&serverC_send_address, 0, sizeof(serverC_send_address));
	memset(&serverC_recv_address, 0, sizeof(serverC_recv_address));

	serverC_send_address.sin_family = AF_INET;
	serverC_send_address.sin_port = htons (23517);
	serverC_send_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	serverC_recv_address.sin_family = AF_INET;
	serverC_recv_address.sin_port = htons (24517);
	serverC_recv_address.sin_addr.s_addr = inet_addr("127.0.0.1");

//********************************************************************************************************
	
	//create socket to send data
	int serverC_socket = socket(AF_INET, SOCK_DGRAM, 0);

	//bind IP address and port number to serverB_socket_send
  	bind(serverC_socket, (const struct sockaddr*) &serverC_recv_address, sizeof(serverC_recv_address));

	// //create socket to receive data from serverB
	// int serverC_socket_recv = socket(AF_INET, SOCK_DGRAM, 0);

	//***************************************************************************************************

	socklen_t serverC_socket_send_len;
	serverC_socket_send_len = sizeof(serverC_send_address);

	//send info got from serverA or serverB to serverC 
	for (int i=0; i<7; i++)
	{
		sendto(serverC_socket, &temp_X[i], sizeof(temp_X[i]), 0, (const struct sockaddr *) &serverC_send_address, serverC_socket_send_len);
	}

	printf("The AWS sent link ID= <%d>, size = <%.2lf>, power = <%.2lf> and link information to Backend-Server C using UDP over port <24517>.\n\n", (int)LinkID, pktsize, SPdbm);
	
	//*************************************************************************************************************************************

	socklen_t serverC_socket_recv_len;
	serverC_socket_recv_len = sizeof(serverC_recv_address);

	//receive
	recvfrom(serverC_socket, &Tt, sizeof(Tt), 0, (struct sockaddr *) &serverC_recv_address, &serverC_socket_recv_len);
	recvfrom(serverC_socket, &Tp, sizeof(Tp), 0, (struct sockaddr *) &serverC_recv_address, &serverC_socket_recv_len);
	recvfrom(serverC_socket, &Delay, sizeof(Delay), 0, (struct sockaddr *) &serverC_recv_address, &serverC_socket_recv_len);

	printf("The AWS received outputs from Backend-Server C using UDP overport <24517>.\n\n");

	//printf("Tt is: %.2lf ms, Tp is: %.2lf ms, Delay is: %.2lf ms.\n\n", Tt, Tp, Delay);
	close(serverC_socket);

	return 0;
}


int main(){
	
	//create socket receive from client and define the server address of itself
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client_address;
	client_address.sin_family = AF_INET;
	client_address.sin_port = htons (25517);
	client_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	//bind the IP address and port number to client_socket
	bind(client_socket, (struct sockaddr*) &client_address, sizeof(client_address));

	//************************************************************************************************

	//create a socket connecte to monitor
	int monitor_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (monitor_socket == -1){
		printf("socket create error.\n\n");
		return 1;
	}

	struct sockaddr_in monitor_address;
	monitor_address.sin_family = AF_INET;
	monitor_address.sin_port = htons (26517);
	monitor_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	//bind the IP address and port number to the socket
	int monitor_bind = bind(monitor_socket, (struct sockaddr*) &monitor_address, sizeof(monitor_address));
	if (monitor_bind == -1)
	{
		printf("monitor socket bind error\n\n");
	} else {
		printf("The AWS is up and running.\n\n" );
	}

	//************************************************************************************************
	//always keep AWS online
	while(1)
	{	
		//listen request from client and monitor
		listen(client_socket, 10);
		listen(monitor_socket, 10);

		socklen_t clientaddr_len, monitor_len;
		int monitor_child_socket, client_child_socket;
		struct sockaddr_storage client_addr, monitor_addr;
		monitor_len = sizeof(monitor_addr);

		monitor_child_socket = accept(monitor_socket, (struct sockaddr*) &monitor_addr, &monitor_len);

		if (monitor_child_socket == -1)
		{
			printf("no request come in.\n\n");
		}

		clientaddr_len = sizeof(client_addr);

		client_child_socket = accept(client_socket,(struct sockaddr*) &client_addr, &clientaddr_len);

		recv(client_child_socket, &LinkID, sizeof(LinkID), 0);
		recv(client_child_socket, &pktsize, sizeof(pktsize), 0);
		recv(client_child_socket, &SPdbm, sizeof(SPdbm), 0);
		printf("The AWS received link ID = <%d>, pktsize = <%.2lf>, and power = <%.2lf> from the client using TCP over port <25517>.\n\n", (int)LinkID, pktsize, SPdbm);

		//send received data to monitor
		send(monitor_child_socket, &LinkID, sizeof(LinkID), 0);
		send(monitor_child_socket, &pktsize, sizeof(pktsize), 0);
		send(monitor_child_socket, &SPdbm, sizeof(SPdbm), 0);
		printf("The AWS sent link ID = <%d>, pktsize = <%.2lf>, and power = <%.2lf> to the monitor using TCP over port <26517>.\n\n", (int)LinkID, pktsize, SPdbm);
	    
	    //*************************************************************************************************************************

		//function part in AWS
	    searchA();
	    printf("The serverA has already searched database_a.\n\n" );

	    searchB();
	    printf("The serverB has already searched database_b.\n\n" );

	    if (match_A != 0) 
	    {
	    	printf("The AWS recevied <%d> matches from Backend-Server A using UDP over port <21517>.\n\n", (int)match_A);

	    	calculation(ValueA);

	    	send(monitor_child_socket, (char *) &match_A, sizeof(match_A), 0);
			send(client_child_socket, (char *) &match_A, sizeof(match_A), 0);
			send(client_child_socket, (char *) &Delay, sizeof(Delay), 0);
	    	
			printf("The AWS send delay = <%.2lf> ms to the client using TCP over port <25517>.\n\n", Delay);

			//send all results to monitor
			send(monitor_child_socket, (char *) &Tt, sizeof(Tt), 0);
			send(monitor_child_socket, (char *) &Tp, sizeof(Tp), 0);
			send(monitor_child_socket, (char *) &Delay, sizeof(Delay), 0);
			printf("The AWS send detailed results to the monitor using TCP over port <26517>.\n\n");
	    }

	    if (match_B != 0)
	    {
	    	printf("The AWS recevied <%d> matches from Backend-Server B using UDP over port <21517>.\n\n", (int)match_B);
	    	
	    	calculation(ValueB);

	    	send(monitor_child_socket, (char *) &match_B, sizeof(match_B), 0);
			send(client_child_socket, (char *) &match_B, sizeof(match_B), 0);
			send(client_child_socket, (char *) &Delay, sizeof(Delay), 0);

			printf("The AWS send delay = <%.2lf> ms to the client using TCP over port <25517>.\n\n", Delay);

			//send all results to monitor
			send(monitor_child_socket, (char *) &Tt, sizeof(Tt), 0);
			send(monitor_child_socket, (char *) &Tp, sizeof(Tp), 0);
			send(monitor_child_socket, (char *) &Delay, sizeof(Delay), 0);

			printf("The AWS send detailed results to the monitor using TCP over port <26517>.\n\n");
	    }

	    if (match_A==0 && match_B==0)
	    {
	    	double no_match = 0;
	    	send(monitor_child_socket, (char *) &no_match, sizeof(no_match), 0);
	    	send(client_child_socket, (char *) &no_match, sizeof(no_match), 0);

	    	printf("The AWS sent NO MATCH to the monitor and the client using TCP over ports <26517> and <25517>, respectively.\n\n");

	    }
	memset(ValueA, 0, sizeof(ValueA));
	memset(ValueB, 0, sizeof(ValueB));

	Tt = 0;
	Tp = 0;
	Delay = 0;

	close(client_child_socket);
	close(monitor_child_socket);
    }
	//close(client_socket);
}

