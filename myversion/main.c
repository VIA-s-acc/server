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
#define _OPEN_SYS_ITOA_EXT
     
#define TRUE   1 
#define FALSE  0
struct bnk
{
    int move_num;
    int lvl;
    int alive_players; 
}; 
struct PlayerInfo
{
    int alive;
    int player_num;
    int money;
    int production;
    int factories_work;
    int factories_build;
    int raw;
    int turn;
};
int main(int argc, char *argv[]){
    
  if (argc != 3 ) 
  {
    printf("Неверное количество аргументов \n");
    return 0;
  }
  int start_game = 0;
  int ls;
  int len;
  int opt = TRUE;
  int connfd;
  int count = 0;
  int max_sd;
  int new_socket;
  int activity;
  int valread;
  int sd;
  int turn;
  int test;
  int i ;
  char buffer[1025];
  test = 0 ;
  fd_set readfds;  
  char *p,*p1;
  int port = strtol(argv[1], &p, 10);
  int maxpl = strtol(argv[2],&p1,10);
  struct PlayerInfo PlayersInfo[maxpl];
  struct bnk bank;
  int turn_check = 0 ;
  int production_list[maxpl];
  for (int i = 0; i < maxpl; i++)
    {
        production_list[i] = 0;
        PlayersInfo[i].alive=TRUE;
        PlayersInfo[i].money=10000;
        PlayersInfo[i].production=2;
        PlayersInfo[i].factories_work=2;
        PlayersInfo[i].factories_build=0;
        PlayersInfo[i].raw=4;
        PlayersInfo[i].turn = 1;
    }
   bank.move_num = 2;
   bank.lvl = 2;
   bank.alive_players = 0;
int statistics[maxpl][7];
for (int i = 0; i<maxpl; i++)
        {
            statistics[i][0]= bank.move_num = 1;
            statistics[i][1]= bank.alive_players;
            statistics[i][2]=PlayersInfo[i].alive; //how to pick player #i?
            statistics[i][3]=PlayersInfo[i].money;
            statistics[i][4]=PlayersInfo[i].production;
            statistics[i][5]=PlayersInfo[i].factories_work;
            statistics[i][6]=PlayersInfo[i].factories_build;
            statistics[i][7]=PlayersInfo[i].raw;
        }
float market[4][4]; 
for (int i=0; i<5; i++)
    {
        market[i][0]=i;
        market[i][1]= bank.alive_players*(1+i/2);
        market[i][3]= bank.alive_players*(3-i/2);
        market[i][4]= 6500-i*500;
    }
    market[0][2]=800;
    market[1][2]=650;
    market[2][2]=500;
    market[3][2]=400;
    market[4][2]=300;


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
            if(start_game != 1){
            if ((new_socket = accept(ls,(struct sockaddr *)&addr, (socklen_t*)&len))<0)  
            {  
                perror("Ошибка accept");  
                exit(EXIT_FAILURE);  
            } 
            
            //inform user of socket number - used in send and receive commands
            count += 1;
            if (count == maxpl){
                start_game += 1;
            }    
            printf("Новое соединение , fd сокета %d , ip  : %s , port : %d  \n  start_game -->%d/%d<---" , new_socket , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port),count,maxpl);  
            
            
            // //send new connection greeting message 
            // if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            // {  
            //     perror("Ошибка send");  
            // }  
                 
            // puts("Приветственное сообщение отправлено успешно");  
                 
            //add new socket to array of sockets 
            for (i = 0; i < maxpl; i++)  
            {  
                //if position is empty 
                if( player_list[i] == 0 )       
                {  
                    player_list[i] = new_socket;  
                    PlayersInfo[i].alive=TRUE;
                    bank.alive_players += 1;
                    for (int i=0; i<5; i++)
                    {
                        market[i][0]=i;
                        market[i][1]= bank.alive_players*(1+i/2);
                        market[i][3]= bank.alive_players*(3-i/2);
                        market[i][4]= 6500-i*500;
                    }
                    printf("Добавление в список сокетов ,как %d\n" , i);  
                         
                    break;  
                }  
            }
            }
        else{
            int scd = accept(ls,(struct sockaddr *)&addr, (socklen_t*)&len);
            send(scd,"\n\n\nИгра уже началась !\n\n\n",strlen("\n\n\nИгра уже началась !\n\n\n"),0);
            close(scd);
        }
        }
    if(count == maxpl){
        if(test == 0){
                            for(int i = 0 ; i < bank.alive_players; i++){             
                            send(player_list[i],"\n\n\n=======\nИГРА НАЧАЛАСЬ\nРаунд : 1\nДля помощи пропишите help\n=======\n\n\n",strlen("\n\n\n=======ИГРА НАЧАЛАСЬ\nРаунд : 1\nДля помощи пропишите help=======\n\n\n"),0);
                            test = 1;
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
                    printf("Игрок отключился , ip %s , port %d , num  %d \n " , inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port),i);  
                    bank.alive_players -= 1;
                    for (int i=0; i<5; i++)
                    {
                        market[i][0]=i;
                        market[i][1]= bank.alive_players*(1+i/2);
                        market[i][3]= bank.alive_players*(3-i/2);
                        market[i][4]= 6500-i*500;
                    }
                    PlayersInfo[i].alive=FALSE;
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
                    if(start_game == 1){ 
                        if(strncmp(buffer,"help",4) != 0 && strncmp(buffer,"market",6) != 0 && strncmp(buffer,"player",6) != 0 && strncmp(buffer,"prod",4) != 0 && strncmp(buffer,"buy",3) != 0 && strncmp(buffer,"sell",4) != 0 && strncmp(buffer,"build",5) != 0 &&  strncmp(buffer,"turn",4) != 0 && strncmp(buffer,"stats",5) != 0 ){
                            if (PlayersInfo[i].alive!=0){
                            send(sd , "Неверная команда ---- для получения информации введите help\n" , strlen("Неверная команда ---- для получения информации введите help\n") , 0 );  
                            }
                        }
                        else
                        {
                            if(strncmp(buffer,"help",4) == 0 ){ 
                            printf("\n\nЗапрос ""help"" от игрока %d\n\n",i); 
                            if (PlayersInfo[i].alive!=0){
                            send(sd , "\n\nhelp - помощь \nmarket - магазин \nplayer - информация об игроке - \nprod - продукция - \nbuy - купить -  \nsell - продать - \nbuild - построить - \nturn - ход -\n\n" , strlen("\n\nhelp - помощь \nmarket - магазин \nplayer - информация об игроке - \nprod - продукция - \nbuy - купить -  \nsell - продать - \nbuild - построить - \nturn - ход -\n\n") , 0 );  
                            }}
                            if(strncmp(buffer,"market",5) == 0 ){ 
                                printf("\n\nЗапрос ""market"" от игрока %d\n\n",i);
                                if (PlayersInfo[i].alive!=0){
                                if(PlayersInfo[i].turn != 0 ){
                                for(int i = 0 ; i < 4 ;i++){
                                    for (int j = 0 ; j < 4 ; j++)
                                    {
                                        printf("%lf\t",market[i][j]);
                                    }
                                    printf("\n");
                                    }}}
                            }
                            // if(strncmp(buffer,"build",5) == 0 ){}
                            // if(strncmp(buffer,"sell",4) == 0 ){}
                            // if(strncmp(buffer,"buy",3) == 0 ){}
                            if(strncmp(buffer,"prod",4) == 0 ){
                               
                                printf("\n\nЗапрос ""prod"" от игрока %d\n\n",i);
                                if (PlayersInfo[i].alive!=0){
                                        if(PlayersInfo[i].turn != 0 ){                     
                                            if(atoi(buffer+5) > PlayersInfo[i].factories_work)
                                            {
                                                send(sd,"\n\n\nНе хватает заводов ! \n\n\n",strlen("\n\n\n Не хватает заводов ! \n\n\n"),0);
       
                                            }
                                            else if(atoi(buffer+5) * 2000 > PlayersInfo[i].money){
                                                printf("%d",atoi(buffer+5) * 2000);
                                                send(sd,"\n\n\nНе хватает денег ! \n\n\n",strlen("\n\n\n Не хватает денег ! \n\n\n"),0);
                                                
                                            }
                                            else if(atoi(buffer+5) > PlayersInfo[i].raw){
                                                 send(sd,"\n\n\nНе хватает сырья ! \n\n\n",strlen("\n\n\n Не хватает сырья ! \n\n\n"),0);
                                                 
                                            }
                                            else {
                                                send(sd,"\n\n\nзаяква принята \n\n\n",strlen("\n\n\nзаяква принята \n\n\n"),0);
                                                production_list[i] = atoi(buffer+5); 
                                                PlayersInfo[i].raw -= atoi(buffer+5);
                                                
                                            }
                                            break;

                                    }
                                }              
                            }                 
                            if(strncmp(buffer,"turn",4) == 0 ){
                                printf("\n\nЗапрос ""turn"" от игрока %d\n\n",i);
                                if (PlayersInfo[i].alive!=0){
                                if(PlayersInfo[i].turn != 0 ){ 
                                       PlayersInfo[i].turn = 0;
                                       turn_check += 1;
                                send(sd,"--ход окончен--\n" ,strlen("--ход окончен--\n"),0);}}
                            }
                            if(strncmp(buffer,"player",6) == 0){
                                char player[100] ;
                                printf("--%d--\n",PlayersInfo[i].turn);
                                printf("\n\nЗапрос ""player"" от игрока %d\n\n",i);
                                
                                if (PlayersInfo[i].alive!=0){
                                if(PlayersInfo[i].turn != 0 ){
                                snprintf(player,100,"\n\nid = %d \nalive = %d \nmoney = %d\nproduction = %d\nfatctories_work = %d\nfactories_build = %d\nraw = %d\n\n",i,PlayersInfo[i].alive 
                                ,PlayersInfo[i].money,PlayersInfo[i].production,PlayersInfo[i].factories_work,PlayersInfo[i].factories_build,PlayersInfo[i].raw);
                                printf("%s",player);
                                send(sd,player,strlen(player),0);}}
                            }
                            if(strncmp(buffer, "stats" , 5) == 0){
                                char stats[100];
                                printf("Запрос ""stats"" от игрока %d\n",i);
                                for (int i = 0 ; i < maxpl ;i++){
                                    if(PlayersInfo[i].alive!=0){
                                        if(PlayersInfo[i].turn != 0){
                                        snprintf(stats,150,"\n\n======\n---player %d ---\nalive = %d\nmoney = %d \n production = %d \nfactories_work = %d \nfactories_build = %d \nraw = %d \n======\n\n",i,PlayersInfo[i].alive ,PlayersInfo[i].money,PlayersInfo[i].production,PlayersInfo[i].factories_work,PlayersInfo[i].factories_build,PlayersInfo[i].raw);
                                        send(sd,stats,strlen(stats),0);
                                        printf("%s",stats);}}

                                
                                snprintf(stats,40,"turn = %d\nalive_players = %d\n",bank.move_num,bank.alive_players);
                                printf("\n\n======\n%s\n======\n\n",stats);
                                send(sd,stats,strlen(stats),0);}
                     
                            }
                            if(turn_check == bank.alive_players){
                                bank.move_num += 1;
                                turn_check = 0;
                                for(int i = 0 ; i <  bank.alive_players;i++){
                                    PlayersInfo[i].turn = 1;
                                    char info[50];
                                    snprintf(info,50,"\n\n====\nРаунд : %d\n====\n\n",bank.move_num);
                                    if(player_list[i] != 0){
                                    send(player_list[i],info,strlen(info),0);}
                        }
                            for(int i = 0 ; i < maxpl ; i++)
                            {
                                if(production_list[i] != 0){
                                    PlayersInfo[i].production += production_list[i];
                                    PlayersInfo[i].money -= 2000*production_list[i];
                                    char prod_msg[100];
                                    snprintf(prod_msg,100,"\n\n\nБыло создано %d продукции\n\n\n",production_list[i]);
                                    send(player_list[i],prod_msg,strlen(prod_msg),0);
                                }
                            }  
                            if(bank.alive_players == 1){
                                for (int i = 0 ; i < maxpl; i++){
                                    if (PlayersInfo[i].alive != 0 ){
                                        int pob = i;
                                        }
                                    }
                                    for(int i = 0 ; i < maxpl ; i++){
                                        if(player_list[i] != 0)
                                        {
                                            char win_msg[100];
                                            snprintf(win_msg,100,"\n\n\nИгрок --->id:%d ПОБЕДИЛ ! \n\n\n ",i);
                                            send(player_list[i],win_msg,strlen(win_msg),0);                                
                                        }                                  
                                    }
                                    printf("\n\n\nИгрок --->id:%d ПОБЕДИЛ ! \n\n\n ",i);
                                    exit(0);
                                }
                            }
                        }
                    }
                }  
            }  
        } 
    }  

    return 0 ;
}