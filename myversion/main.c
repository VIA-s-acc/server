#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
     
#define TRUE   1 
#define FALSE  0


int main(int argc, char *argv[]){
  if (argc != 3 ) 
  {
    printf("Неверное количество аргументов \n");
    return 0;
  }
  int ls;
  int len;
  int opt = TRUE;
  int connfd;
  int max_sd;
  int new_socket;
  int activity;
  int valread;
  int sd;
  int i ;
  char buffer[1025];
  char *message = "ЛУЧШИЙ СЕРВЕР НА СВЕТЕ !!!!! \r\n";  
  fd_set readfds;  
  char *p,*p1;
  int port = strtol(argv[1], &p, 10);
  int maxpl = strtol(argv[2],&p1,10);
  if(port < 1024){
    printf("---неверный порт---");
    return 0;
  }
  ls = socket(AF_INET,SOCK_STREAM,0);
  int player_list[maxpl];
  for (int i = 0 ; i<maxpl ; i++)
  {
    player_list[i] = 0;
  }
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if( setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
  bind(ls, (struct sockaddr *) &addr, sizeof(addr));
  if ((listen(ls, 5)) != 0) {
        printf("Прослушивание провалено...\n");
        exit(0);
    }
    else
        printf("ожидание игроков..\n");
    len = sizeof(addr);
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(ls, &readfds);  
        max_sd = ls;      
        //add child sockets to set 
        for ( i = 0 ; i < maxpl ; i++)  
        {  
            //socket descriptor 
            sd = player_list[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
    if ((activity < 0) && (errno!=EINTR))  
    {  
            printf("Ошибка select");  
    }
    if (FD_ISSET(ls, &readfds))  
        {  
            if ((new_socket = accept(ls,(struct sockaddr *)&addr, (socklen_t*)&len))<0)  
            {  
                perror("Ошибка accept");  
                exit(EXIT_FAILURE);  
            }  
             
            //inform user of socket number - used in send and receive commands 
            printf("Новое соединение , fd сокета %d , ip  : %s , port : %d  \n" , new_socket , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port));  
           
            //send new connection greeting message 
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            {  
                perror("Ошибка send");  
            }  
                 
            puts("Приветственное сообщение отправлено успешно");  
                 
            //add new socket to array of sockets 
            for (i = 0; i < maxpl; i++)  
            {  
                //if position is empty 
                if( player_list[i] == 0 )       
                {  
                    player_list[i] = new_socket;  
                    printf("Добавление в список сокетов ,как %d\n" , i);  
                         
                    break;  
                }  
            }  
        }  
    //else its some IO operation on some other socket
    for (i = 0; i < maxpl; i++)  
        {  
            sd = player_list[i];  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&addr , \
                        (socklen_t*)&len);  
                    printf("Хост отключился , ip %s , port %d \n" , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port));  
                         
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    player_list[i] = 0;  
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';  
                    if(strncmp(buffer,"help",4) != 0 && strncmp(buffer,"market",6) != 0 && strncmp(buffer,"player",6) != 0 && strncmp(buffer,"prod",4) != 0 && strncmp(buffer,"buy",3) != 0 && strncmp(buffer,"sell",4) != 0 && strncmp(buffer,"build",5) != 0 &&  strncmp(buffer,"turn",4) != 0){
                        printf("%d",strncmp(buffer,"help",4));
                        send(sd , "Неверная команда ---- для получения информации введите help" , strlen("Неверная команда ---- для получения информации введите help") , 0 );  
                    
                     }
                    else
                    {
                        if(strncmp(buffer,"help",4) == 0 ){  
                           send(sd , "help - помощь \nmarket - магазин \nplayer - информация об игроке - \nprod - продукция - \nbuy - купить -  \nsell - продать - \nbuild - построить - \nturn - ход -\n" , strlen("help - помощь \nmarket - магазин \nplayer - информация об игроке - \nprod - продукция - \nbuy - купить -  \nsell - продать - \nbuild - построить - \nturn - ход -\n") , 0 );  
                        }
                    }
                }  
            }  
        } 
    }  

    return 0 ;
}