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
  FILE *file = fopen("database_b.csv","r"); 
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

  //prepare info for serverB_socket
  struct sockaddr_in serverB_recv_addr, serverB_send_addr;
  memset(&serverB_recv_addr, 0, sizeof(serverB_recv_addr));
  memset(&serverB_send_addr, 0, sizeof(serverB_send_addr));

  //Address info of serverB_recv_socket
  serverB_recv_addr.sin_family = AF_INET;
  serverB_recv_addr.sin_port = htons(22517);
  serverB_recv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  //Address info for serverB_send_socket
  serverB_send_addr.sin_family = AF_INET;
  serverB_send_addr.sin_port = htons(24517);
  serverB_send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  //******************************************************************************************************************

  //create socket for serverB 
  int serverB_socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (serverB_socket == -1)
  {
    printf("serverB_socket creation error.\n\n");
  } else {
    printf("The Server B is up and running using UDP on port <22517>.\n\n");
  } 

  //bind IP address and port number to serverB_socket_recv
  bind(serverB_socket, (const struct sockaddr*) &serverB_recv_addr, sizeof(serverB_recv_addr)); 


  //create serverA_socket_send to send data back to AWS
  //int serverB_socket_send = socket(AF_INET, SOCK_DGRAM, 0);

  //if (serverB_socket_send == -1)
  //{
  //  printf("serverB_socket_send creation error.\n\n");
  //}

  //bind IP address and port number to serverB_socket 
  //bind(serverB_socket_send, (const struct sockaddr*) &serverB_send_addr, sizeof(serverB_send_addr));

//**********************************************************************************************************8

  //keep receiving data
  while (1)
  {
    socklen_t serverB_recv_len;
    serverB_recv_len = sizeof(serverB_recv_addr);

    recvfrom(serverB_socket, &LinkID, sizeof(LinkID), 0, (struct sockaddr *) &serverB_recv_addr, &serverB_recv_len);
      
    printf("The server B received input <%d>.\n\n", (int)LinkID);

    //search for data in the database_a
    search_data(LinkID);

    socklen_t serverB_send_len;
    serverB_send_len = sizeof(serverB_send_addr);

    //send data back to AWS anyway
    if (found[0]!=0) {
      for (int i=0; i<6; i++) {
        double *temp = found;
        sendto(serverB_socket, &temp[i], sizeof(temp[i]), 0, (struct sockaddr *) &serverB_send_addr, serverB_send_len);
      }   
    } else if (found[0]==0) {
      for (int i=0; i<6; i++) {
        double *temp = found;
        sendto(serverB_socket, &temp[i], sizeof(temp[i]), 0, (struct sockaddr *) &serverB_send_addr, serverB_send_len);\
      }
    }

    printf("The server B has found <%d> match.\n\n", (int)found[0]);
    
    if (found[0]==1) printf("The Server B finished sending the output to AWS.\n\n");
  }
  //close(serverB_socket);  
}

