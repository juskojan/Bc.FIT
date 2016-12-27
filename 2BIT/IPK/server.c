#include <unistd.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <regex.h> 



void print_usage() {
    fprintf(stderr, "Usage: -p <PORT no.>\n");
}

/*
  FUNKCIA PRIJIMA LOGIN A VRACIA ODPOVEDAJUCI RIADOK ZO SUBORU PASSWD
*/

char* retlinelogin (char* item){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  
  
  regex_t regex;
  int reti;
  /*COMPILE REGEX*/
  int i;
  for(i = 0; item[i]; i++){
    item[i] = tolower(item[i]);
  }

  reti = regcomp(&regex, item, REG_ICASE);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }

  fp = fopen("/etc/passwd", "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);

  /*NACITAJ PO RIADKU SUBOR, PREMENNA LINE DLZKY READ*/
  while ((read = getline(&line, &len, fp)) != -1) {
    int i=0;
    char auxline[20];
    bzero(auxline,20);
    while(line[i] != ':'){
      auxline[i] = line[i];
      i++;
    }
    
    if (strcmp(item, auxline) == 0){
      return line;
    }
  }
  return NULL;
}

/*
  FUNKCIA PRIJIMA UID A VRACIA ODPOVEDAJUCI RIADOK ZO SUBORU PASSWD
*/
  
char* uidbuf;
char* retlineuid (char* item){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int linecount = 0;
  /* OPEN FILE */
  
  regex_t regex;
  int reti;

  /*COMPILE REGEX */
  reti = regcomp(&regex,item, REG_ICASE);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }

  /* OPEN FILE */
  fp = fopen("/etc/passwd", "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);


  uidbuf = calloc(1000,sizeof(char));
  
  /*NACITAJ PO RIADKU SUBOR, PREMENNA LINE DLZKY READ */
  while ((read = getline(&line, &len, fp)) != -1) {
    int i = 0;
    int counter = 3;
    char auxline[50];
    bzero(auxline,50);
    while(line[i] != '\0'){
      auxline[i] = line[i];
      if(line[i] == ':'){
        counter--;
      }
      if(counter == 0){
        break;
      }
      i++;
    }
    
    /* NAJDI SPRAVNY RIADOK, AK ICH JE VIAC TAK ICH SPOJ DO JEDNEHO */
    reti = regexec(&regex, auxline, 0, NULL, 0);
    if (!reti) {  
      /*printf("%s\n",line );*/
      strcat(uidbuf,line);
      linecount++;
      if(linecount >= 1){
        uidbuf[strlen(uidbuf)-1]=':'; 
      }
    }
  }
  
  if(strlen(uidbuf) != 0){
    return uidbuf;
  }
  else{
    return NULL;
  }
  
}

/*
  FUNKCIA PRIJIMA CELY RIADOK ZO SUBORU PASSWD A VRACIA PODLA FLAGU POZADOVANU HODNOTU
*/
char* get_value(char* line, char flag){
  int i = 0;
  int j = 0;
  int max, min;
  int count = 0;
  char* buffer = calloc(100,sizeof(char));
  bzero(buffer,100);
  
  /* JEDNOTLIVE STLPCE SU OHRANICENE : , PODLA TOHO NASTAV OHRANICENIE KTORE SLEDUJE PREMENNA COUNT */
  switch(flag){
    case 'U':
      min = 2;
      max = 3;
      break;
    case 'L':
      min = 0;
      max = 1;
      break;
    case 'G':
      min = 3;
      max = 4;
      break;
    case 'N':
      min = 4;
      max = 5;
      break;
    case 'H':
      min = 5;
      max = 6;
      break;
    case 'S':
      min = 6;
      max = 7;
      break;
  }

  /* SPRACOVAVAJ CELY RIADOK ZO SUBORU PASSWD AZ PO KONIEC */
  while(line[i] != '\0'){
    /* POCITAJ DVOJTECKY */
    if(line[i] == ':'){
      count++;
      i++;

      if(count!=7)
        continue;
    }

    /* AK NARAZIS NA MAX. DVOJTECKU ALEBO KONIEC TAK VRAT POLE */
    if(count == max || line[i+1] == '\0'){
      if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1] = '\0';
      }
      return buffer;
    }
    /* NACITAVAJ DO POLA IBA OD MIN. DVOJTECKY */
    if(count == min){
      buffer[j] = line[i];
      j++;
    } 
    i++;
  }
}


/*
  FUNKCIA VRACIA JEDNOTLIVE ZANALYZOVANE RIADKY VYSLEDKOV
  PRE KAZDY FLAG V SPRAVE VOLA FUNKCIU GET VALUE KTORA VYPARSUJE PASSWD SUBOR A SPOJI ICH PODLA PORADIA DO JEDNEHO RIADKU
*/
char* writeback(char* line, char* params,char* match){
  
  char* buffer = calloc(500,sizeof(char));
  int i = 0;
  char *str2;
  int counter=0;

  if(line == NULL){
    if(params[6]=='l'){
      str2 = "Chyba: neznamy login ";
    }
    if(params[6]=='u'){
      str2 = "Chyba: nezname UID ";
    }
    strcat(buffer, str2);
    strcat(buffer, match);
    strcat(buffer, "\n");
    return buffer;
  }
  
  line[strlen(line)]=':';
  
  int x=1;
  int oddelovace[10];
  oddelovace[0]=0;
  int f=0;
  /* 
    SPOCITAJ KOLKO JE V PRIJATOM RIADKU : A PODLA TOHO ZISTI KOLKO ZAZNAMOV V TOM JE,
    ULOZ DO POLA INDEXY, KDE RIADKY ROZDELIT  
  */
  for(f=0; i<strlen(line) ;i++){
    if(line[i] == ':')
      counter++;
    if (counter%7 == 0 && counter != 0){
      counter=0;
      oddelovace[x++]=i+1;
    }
  }
  
  /* ROZDEL CELY RIADOK NA JEDNOTLIVE RIADKY... AK SU NAJDENE MULTIPLE ZAZNAMY */ 
  char templine[1000];
  bzero(templine,1000);
  int j;
  x--;
  for(j=0; j < x; j++){
    int y;
    int z=0;
    /* VYTVOR JEDNOTLIVY RIADOK A SPRACUJ HO */
    for(y=oddelovace[j]; y < (oddelovace[j+1]); y++){
      templine[z++] = line[y];
    }
    
    i=0;
    while(params[i] != '_' && i < 6){
      switch(params[i]){
        case 'U':
          buffer = strcat(buffer,get_value(templine,'U'));
          buffer = strcat(buffer," ");
          break;
        case 'L':
          buffer = strcat(buffer,get_value(templine,'L'));
          buffer = strcat(buffer," ");
          break;
        case 'G':
          buffer = strcat(buffer,get_value(templine,'G'));
          buffer = strcat(buffer," ");
          break;
        case 'N':
          buffer = strcat(buffer,get_value(templine,'N'));
          buffer = strcat(buffer," ");
          break;
        case 'H':
          buffer = strcat(buffer,get_value(templine,'H'));
          buffer = strcat(buffer," ");
          break;
        case 'S':
          buffer = strcat(buffer,get_value(templine,'S'));
          buffer = strcat(buffer," ");
          if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1] = '\0';
          }
          break;
      }
      i++;
    }
    buffer[strlen(buffer)-1] = '\n';
  } 


  return buffer;
}

/*
  PRVOTNA FUNKCIA KTORA SA VOLA Z PROCESU JEDNOTLIVYCH KLIENTOV
  postupne parsovanie loginov alebo uid cisel
*/
void analyze(char* received, int socket){
  int i = 7;
  int j = 0;
  char match[100];
  bzero(match,100);
  
  /* SPRAVA JE UKONCENA ZNAKOM ^, DOVTEDY NACITAJ */
  while(received[i] != '^'){
    /* BOL NACITANY LOGIN/UID, PODLA TOHO ZAPIS NA SOCKET POTREBNU SPRAVU */
    if(received[i] == '*'){
      /* VOLAJ FUNKCIU NA SPRACOVANIE LOGINOV*/
      if(received[6] == 'l'){
        int n = write(socket,writeback(retlinelogin(match), received, match) ,strlen(writeback(retlinelogin(match), received, match)));
   
            if (n < 0)
            {
              perror("ERROR writing to socket");
              exit(1);
            }
      }
      /* VOLAJ FUNKCIU NA SPRACOVANIE UID A ZAPIS */
      else if(received[6] == 'u'){
        int n = write(socket,writeback(retlineuid(match), received, match) ,strlen(writeback(retlineuid(match), received, match)));
   
            if (n < 0)
            {
              perror("ERROR writing to socket");
              exit(1);
            }
      }
      /* POLE MATCH OBSAHUJE LOGIN / UID AKTUALNE SPRACOVANE.. POSTUPNE SA NAPLNA ZNAKMI AZ PO * */
      bzero(match,100);
      j = 0;
      i++;
    }
    else{
      match[j] = received[i];
      j++;
      i++;
    }
  }
}


int main(int argc, char *argv[]) {
    
    int option = 0;
    int cislo_portu = 0;

    /* NACITAJ CISLO PORTU */

    while ((option = getopt(argc, argv,"p:")) != -1) {
        switch (option) {
             case 'p' : cislo_portu = atoi(optarg); 
                 break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    } 

    int sockfd, newsockfd, portno, clilen;
    portno=cislo_portu;
   
   struct sockaddr_in serv_addr, cli_addr;
   
   
   /* VYTVOR SOCKET */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   int yes = 1;
   /* OSETRENIE BIND ALREADY IN USE CHYBY PRI OPATOVNOM SPUSTANI */
   if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    perror("setsockopt");
    exit(1);
    }

   if (sockfd < 0)
      {
      perror("ERROR opening socket");
      exit(1);
      }
   
   /* INICIALIZUJ SOCKET STRUKTURU */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* BIND NA SOCKET */
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }
   
   /* PRIJMI SPRAVY OD KLIENTA */
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1)
   {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0)
         {
         perror("ERROR on accept");
         exit(1);
         }
      signal(SIGCHLD,SIG_IGN);
      /* VYTVOR PROCES POTOMKA */
      int pid = fork();
      if (pid < 0)
         {
         perror("ERROR on fork");
         exit(1);
         }
      
      if (pid == 0)
         {
           /* KLIENTSKY PROCES */
           close(sockfd);
           int n;
           char buffer[256];      
           bzero(buffer,256);
           /* CITAJ SPRAVU OD KLIENTA */
           n = read(newsockfd,buffer,255);
     
           if (n < 0){
              perror("ERROR reading from socket");
              exit(1);
            }
           /* ANALYZUJ SPRAVU OD KLIENTA */
           analyze(buffer, newsockfd);

          exit(0);
          }

      else{
        close(newsockfd);
      }
   }

return 0;
}