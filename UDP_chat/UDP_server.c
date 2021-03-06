#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_PORT 53000
#define MAX_USR_NUM 20
#define MAX_NICK_LEN 100
#define MAX 5
#define MAX_LEN 1000
#define INRODUCE_NEW_USER " join the chat"

typedef struct USERS_T
{
  char nickname[MAX_NICK_LEN];
  struct sockaddr_in client_addr;
}users_t;

int main(int argc, char** argv[])
{
  int socket_fd = 0;
  int usercount = 0;
  int msg_to_send_len = 0;
  int clilen, msg_size_rcv, i, j;
  char msg_rcv[MAX_LEN] = {};
  char msg_send[MAX_LEN] = {};
  struct sockaddr_in serv_addr = {};
  struct sockaddr_in client_addr = {};
  users_t clients_arr[MAX] = {};

  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror(NULL);
    exit(1);
  } 

  if (bind(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
  {
    perror(NULL);
    close(socket_fd);
    exit(1);
  }

  clilen = sizeof(client_addr);

  while (1)
  {
    if ((msg_size_rcv = recvfrom(socket_fd, msg_rcv, MAX_LEN, 0, (struct sockaddr*)&client_addr, &clilen)) < 0)
    {
      perror(NULL);
      close(socket_fd);
      exit(1);
    }

    printf("%s %d\n", msg_rcv, msg_size_rcv);

    int flag = 0;

    for(i = 0; i < usercount; i++)
    {
      if (clients_arr[i].client_addr.sin_port ==  client_addr.sin_port && clients_arr[i].client_addr.sin_addr.s_addr ==  client_addr.sin_addr.s_addr)
      {
        flag = 1;
        break;
      }
    }

    if(flag == 0)
    { 
      strcpy(clients_arr[usercount].nickname, msg_rcv); 
      clients_arr[usercount].nickname[strlen(msg_rcv)] = ':';
      clients_arr[usercount].nickname[strlen(msg_rcv) + 1] = ' ';
      clients_arr[usercount].client_addr.sin_port = client_addr.sin_port;
      clients_arr[usercount].client_addr.sin_addr.s_addr = client_addr.sin_addr.s_addr;
      usercount++;
    }
        
    if(0 == flag) 
    {
      strcpy(msg_send, clients_arr[i].nickname);
      strcpy((msg_send + strlen(clients_arr[i].nickname) - 2), INRODUCE_NEW_USER);	
    }
    else
    {
      strcpy(msg_send, clients_arr[i].nickname);
      strcpy((msg_send + strlen(clients_arr[i].nickname)), msg_rcv);	
    }
    
    for(j = 0; j < usercount; j++)  
    {
      if(j == 0)
      {
        if(flag == 1)
          msg_to_send_len = strlen(clients_arr[i].nickname) + strlen(msg_rcv);
        else
          msg_to_send_len = strlen(clients_arr[i].nickname) + strlen(INRODUCE_NEW_USER);
        msg_send[msg_to_send_len] = '\0';
      } 
      printf("flag = %d %d\n", flag, msg_to_send_len);
      
      if(i != j)
      {
        if (sendto(socket_fd, msg_send, msg_to_send_len, 0, (struct sockaddr*)&clients_arr[j].client_addr, clilen) < 0)
        {
          perror(NULL);
          close(socket_fd);
          exit(1);
        }
      }  
    }

   flag = 0;
  }
  return 0;
}
