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

double array[5], found[6];
double LinkID;

//data search function
int search_data(double clientID)
{
  FILE *file = fopen("database_a.csv","r"); 
  while (!feof(file))
  {
    fscanf(file,"%lf,%lf,%lf,%lf,%lf",&array[0],&array[1],&array[2],&array[3],&array[4]); //to check if there are data in seleceted line
    //search for corresponding data
    if (array[0]==clientID)
    {
      found[0]=1; //match 
      found[1]=array[0]; //linkID
      found[2]=array[1]; //BW
      found[3]=array[2]; //Length
      found[4]=array[3]; //Velocity
      found[5]=array[4]; //NPdbmd
      return 1;
    }
    if(array[0]!=clientID)
    {
      found[0] = 0;
      found[1] = 0;
      found[2] = 0;
      found[3] = 0;
      found[4] = 0;
      found[5] = 0;
    }
  } 
  return 0;
}

int main()
{
  //********************************************************************************************************************

  //prepare info for serverA_socket_recv and serverA_socket_send
  struct sockaddr_in serverA_recv_addr, serverA_send_addr;

  memset(&serverA_recv_addr, 0, sizeof(serverA_recv_addr));
  memset(&serverA_send_addr, 0, sizeof(serverA_send_addr));

  //Address info for serverA_socket_recv
  serverA_recv_addr.sin_family = AF_INET;
  serverA_recv_addr.sin_port = htons(21517);
  serverA_recv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  //Address info for serverA_socket_send
  serverA_send_addr.sin_family = AF_INET;
  serverA_send_addr.sin_port = htons(24517);
  serverA_send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  //******************************************************************************************************************

  //create serverA_socket_recv
  int serverA_socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (serverA_socket == -1)
  {
    printf("serverA_socket_recv creation error.\n\n");
  } else {
    printf("The Server A is up and running using UDP on port <21517>.\n\n");
  } 

  //bind IP address and port number to serverB_socket_recv
  bind(serverA_socket, (const struct sockaddr*) &serverA_recv_addr, sizeof(serverA_recv_addr)); 

  //********************************************************************************************************

  //keep receiving data
  while (1)
  {
    socklen_t serverA_recv_len;
    serverA_recv_len = sizeof(serverA_recv_addr);

    recvfrom(serverA_socket, &LinkID, sizeof(LinkID), 0, (struct sockaddr *) &serverA_recv_addr, &serverA_recv_len);
      
    printf("The server A received input <%d>.\n\n", (int)LinkID);

    //search for data in the database_a
    search_data(LinkID);

    socklen_t serverA_send_len;
    serverA_send_len = sizeof(serverA_send_addr);

    //printf("%lf %lf %lf\n\n", found[0], found[1], found[2]);

    //send data back to AWS anyway
    if (found[0]!=0) {
      for (int i=0; i<6; i++) {
        double *temp = found;
        sendto(serverA_socket, &temp[i], sizeof(temp[i]), 0, (struct sockaddr *) &serverA_send_addr, serverA_send_len);
      }   
    } else if (found[0]==0) {
      for (int i=0; i<6; i++) {
        double *temp = found;
        sendto(serverA_socket, &temp[i], sizeof(temp[i]), 0, (struct sockaddr *) &serverA_send_addr, serverA_send_len);\
      }
    }
      //send
      printf("The server A has found <%d> match.\n\n", (int)found[0]);
      if (found[0]==1)  printf("The Server A finished sending the output to AWS.\n\n");
  }

  //close(serverA_socket);  
}

