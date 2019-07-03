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
#include <math.h>

//variables for received data
double LinkID;
double pktsize;
double BW;
double Length; 
double Velocity;
double SPdbm;
double NPdbm;

//Variables for intermediate and final results
double SP;
double NP;
double SNR;
double Capacity;
double Tp;
double Tt;
double Delay;

//Calculation part
int calculate()
{
	SP = pow(10, SPdbm / 10) / 1000; 				//signal power in Watt
	NP = pow(10, NPdbm / 10) / 1000; 				//Noise power in Watt
	SNR = SP / NP; 									//Signal and noise ratio
	Capacity = 1000000 * BW * (log(1 + SNR) / log(2)); //Channon's formula
	Tp = Length / Velocity / 10; 						//Propagation delay
	Tt = 1000 * pktsize / Capacity; 						//transmission delay
	Delay = Tt + Tp; 								//Total delay

	printf("The server C finsihed calculation for link <%d>.\n\n", (int)LinkID);
	
	return 0;
}

int main(){

	//********************************************************************************************************************

  	//prepare info for serverB_socket
  	struct sockaddr_in serverC_recv_addr, serverC_send_addr;
  	memset(&serverC_recv_addr, 0, sizeof(serverC_recv_addr));
  	memset(&serverC_send_addr, 0, sizeof(serverC_send_addr));

  	//Address info of serverB_recv_socket
  	serverC_recv_addr.sin_family = AF_INET;
  	serverC_recv_addr.sin_port = htons(23517);
  	serverC_recv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  	//Address info for serverB_send_socket
  	serverC_send_addr.sin_family = AF_INET;
  	serverC_send_addr.sin_port = htons(24517);
  	serverC_send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  	//******************************************************************************************************************

  	//create socket for serverB 
  	int serverC_socket = socket(AF_INET, SOCK_DGRAM, 0);

  	if (serverC_socket == -1)
  	{
  	  printf("serverC_socket creation error.\n\n");
  	} else {
  	  printf("The Server C is up and running using UDP on port <23517>.\n\n");
  	} 

  	//bind IP address and port number to serverC_socket_recv
  	bind(serverC_socket, (const struct sockaddr*) &serverC_recv_addr, sizeof(serverC_recv_addr)); 

  	socklen_t serverC_recv_len;
  	serverC_recv_len = sizeof(serverC_recv_addr);

  	socklen_t serverC_send_len;
  	serverC_send_len = sizeof(serverC_send_addr);

//**********************************************************************************************************

   	//Communication with AWS
	while(1)
	{
		//receive data from AWS
		recvfrom(serverC_socket, &LinkID, sizeof(LinkID), 0, (struct sockaddr *) &serverC_recv_addr, &serverC_recv_len);
		recvfrom(serverC_socket, &BW, sizeof(BW), 0, (struct sockaddr *) &serverC_recv_addr, &serverC_recv_len);
		recvfrom(serverC_socket, &Length, sizeof(Length), 0, (struct sockaddr *) &serverC_recv_addr, &serverC_recv_len);
		recvfrom(serverC_socket, &Velocity, sizeof(Velocity), 0, (struct sockaddr *) &serverC_recv_addr, &serverC_recv_len);
		recvfrom(serverC_socket, &pktsize, sizeof(pktsize), 0, (struct sockaddr *) &serverC_recv_addr, &serverC_recv_len);
		recvfrom(serverC_socket, &SPdbm, sizeof(SPdbm), 0, (struct sockaddr *) &serverC_recv_addr, &serverC_recv_len);	
		recvfrom(serverC_socket, &NPdbm, sizeof(NPdbm), 0, (struct sockaddr *) &serverC_recv_addr, &serverC_recv_len);
		
		printf("The Server C received link information of link <%d>, file size <%.2lf>, and signal power <%.2lf>.\n\n", (int)LinkID, pktsize, NPdbm);
		
		//Do calculation
		calculate();

		//printf("%lf %lf %lf\n\n", Tt, Tp, Delay);

		//send final results back to AWS
		sendto(serverC_socket, &Tt, sizeof(Tt), 0, (struct sockaddr *) &serverC_send_addr, serverC_send_len);
		sendto(serverC_socket, &Tp, sizeof(Tp), 0, (struct sockaddr *) &serverC_send_addr, serverC_send_len);
		sendto(serverC_socket, &Delay, sizeof(Delay), 0, (struct sockaddr *) &serverC_send_addr, serverC_send_len);
		
		printf("The sever C finished sending the output to AWS.\n\n");
	}

//close(serverC_socket);
}
