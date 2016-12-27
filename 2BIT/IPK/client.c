#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <getopt.h>
#define h_addr h_addr_list[0]



void print_usage() {
    fprintf(stderr, "Usage: -p <PORT no.> -h <HOSTNAME> -l/-u <login>/<uid> [-LUGNHS]\n");
}


int main (int argc, char *argv[] )
{
  /* DEKLARACIA/DEFINICIA PREMENNYCH NA SPRACOVANIE ARG. */
  int option = 0;
  int cislo_portu = 0;
  char *hostname;
  char *loginz[100];
  int number_of_loginz = 0;
  char *uidz[100];
  int number_of_uidz = 0;
  int lflag = 0, uflag = 0, pflag = 0, hflag = 0;
  char flagz[6]="______";
  /* DEKLARACIA SIETOVYCH PREMENNYCH */
  int s, n;
  struct sockaddr_in sin; 
  struct hostent *hptr;

  /* 
      SPRACOVANIE PARAMETROV, NAPLNENIE FLAGOV PODLA KTORYCH VYTVARAM SPRAVU PRE SERVER 
  */

  int poradie=0;
  while ((option = getopt(argc, argv,"h:p:l:u:LUGNHS")) != -1) {
      
      switch (option) {
            case 'p':
              cislo_portu = atoi(optarg);   // NACITANIE PORTU NA KTORY SA PRIPAJA
              pflag = 1;
                break;
            
            case 'h':
              hostname = optarg;            // NACITANIE IP/MENO HOSTITELSKEHO SERVERU
              hflag = 1;
                break;
            
            case 'l':
              optind -= 1;
              number_of_loginz = 0;
              for (;optind < argc && *argv[optind] != '-'; optind++){   // NACITANIE VSETKYCH ZADANYCH LOGINOV DO POLA UKAZATELOV
                loginz[number_of_loginz] = argv[optind];                // POLE LOGNIZ SU UKAZATELE NA "STRING-Y"
                number_of_loginz++;
              }
              lflag = 1;
              uflag = 0;
                break;
            
            case 'u':
              optind -= 1;
              number_of_uidz = 0;
              for (;optind < argc && *argv[optind] != '-'; optind++){   // NACITANIE VSETKYCH ZADANYCH UID CISEL DO POLA UKAZATELOV
                uidz[number_of_uidz] = argv[optind];                    // POLE UIDZ SU UKAZATELE NA "STRING-Y"
                number_of_uidz++;
              }
              uflag = 1;
              lflag = 0;
                break;

            /* NAPLNENIE POLA ZNAKOV, PODLA TOHO KTORE PARAMETRE SU ZADANE */
            case 'L': 
              flagz[poradie] = 'L';
              poradie++;
                break;
            case 'U': 
              flagz[poradie] = 'U';
              poradie++;
                break;
            case 'G':
              flagz[poradie] = 'G';
              poradie++;
                break;
            case 'N':
              flagz[poradie] = 'N';
              poradie++;
                break;
            case 'H': 
              flagz[poradie] = 'H';
              poradie++;
                break;
            case 'S': 
              flagz[poradie] = 'S';
              poradie++;
                break;

            /* NEROZPOZNANY PARAMETER, VYPIS NAPOVEDU A SKONCI */
            default:
              print_usage(); 
              exit(EXIT_FAILURE);
      }
  }

  // CHYBA NEJAKY POVINNY PARAMETER
  if((pflag == 0 || hflag == 0) || (lflag == 0 && uflag == 0)){
    fprintf(stderr, "Missing some of the required parameters!\n");
    exit(1);
  }
  // DEKLARACIA A ALOKACIA POLA, KTORE BUDE ODOSLANE SERVERU
  char *to_send = malloc(1000*sizeof(char));
  
  // NAPLNI PRVE 6 ZNAKY POLA ZADANYMI FLAGMI
  for (int i = 0; i < 6; i++){
    to_send[i]=flagz[i];
  }

  /* 
     PODLA TOHO CI JE ZADANY -l/-u ZAPIS 6. PRVOK POLA A ZA TYM PRISLUSNE HODNOTY LOGINOV/UID CISEL 
     UKONCI SPRAVU PRE SERVER ZNAKOM '^' 
  */
  if(lflag){
    to_send[6] = (int)'l';
    for (int i = 0; i < number_of_loginz; i++){
      strcat(to_send, loginz[i]);
      strcat(to_send, "*");
    }
    strcat(to_send, "^");
  } 
  
  if(uflag){
    to_send[6] = (int)'u';
    for (int i = 0; i < number_of_uidz; i++){
      strcat(to_send, uidz[i]);
      strcat(to_send, "*");
    }
    strcat(to_send, "^");
  } 
  
  /* 
     ARGUMENTY SPRACOVANE, SPRAVA PRE SERVER JE PRIPRAVENA NA ODOSLANIE
  */

  
  // VYTVOR SOCKET ALEBO HLAS CHYBU
  if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0) {
    perror("Error on socket: ");
    return -1;
  }
  // PROTOCOL FAMILY JE INTERNET
  sin.sin_family = PF_INET;

  // NASTAV CISLO PORTU
  sin.sin_port = htons(cislo_portu);
  
  // NASTAV HOSTNAME ALEBO HLAS CHYBU
  if ( (hptr =  gethostbyname(hostname) ) == NULL){
    fprintf(stderr, "Gethostname error!\n");
    return -1;
  }
  // SKOPIRUJ DO SIN
  memcpy(&sin.sin_addr, hptr->h_addr, hptr->h_length);

  // SKUS SA PRIPOJIT ALEBO HLAS CHYBU
  if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ){
    perror("Error on connect: ");
    return -1;
  }

  // POSLI SERVERU PRIPRAVENU SPRAVU
  if ( write(s, to_send, strlen(to_send) +1) < 0 ){
    perror("Error on write: ");
    return -1;
  }

  // UVOLNI ALOKOVANE POLE 
  free(to_send);
  // INICIALIZACIA POLI NA PRIJATIE SPRAV OD SERVERA
  char to_receive[10000];
  bzero(to_receive,10000);
  char test[10];
  bzero(test,10);
  
  // 'NEKONECNY' CYKLUS NA PRIJIMANIE SPRAV OD SERVRU
  while(1)
  {  
    int numread;
    // PRECITAJ SPRAVU OD SERVERA
    if ((numread = read(s, to_receive, sizeof(to_receive) - 1)) == -1)
    {
        perror("Error on read: ");
        exit(1);
    }
    // AK SERVER MLCI, STACILO
    if (numread == 0)
       break;
    // UKONCI PRIJATY STRING
    to_receive[numread] = '\0';
    // OSETRENIE PRIJATYCH NEZNAMYCH VYPISOV, IDE TO NA STDERR
    strncpy(test, to_receive,5);

    if(strlen(to_receive) != 0){
      if(strcmp(test, "Chyba") == 0){
        fprintf(stderr, "%s",to_receive);
      }
      else{
        printf("%s", to_receive);
      }
    }
  }
  // ZAVRI SOCKET
  if (close(s) < 0) { 
    perror("Error on close: ");
    return -1;
  } 
  // NAZDAR
  return 0;
}