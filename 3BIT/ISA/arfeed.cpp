
/* DISCLAIMER: I AM USING EITHER ENGLISH OR CZECH LANGUAGE IN COMMENTS, IF SOMEONE IS READING THIS, DONT GET CONFUSED */
/* libraries */
#include <cstring>
#include <string>
#include <getopt.h>
#include <unistd.h>
#include <fstream>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <ctype.h>
#include <iostream>
#include <libxml/tree.h>
#include <libxml/parser.h>
/* macros */
#define HTTPS 443
#define HTTP 80
#define HOST 36
#define RQST 63
#define ASKPORT 666
#define COUNT 58
#define FILL 85
#define FEED 33
#define ENTRY 73
#define LOCATION_LENGTH 10
/* c++ namespace */
using namespace std;
/* defined structure for one entry */
typedef struct Entry {
   int type;
   string title="";
   string url="";
   string update="";
   string author="";
}Entry;
/* function headers */
string parseURL(string filename, int flag);
string connectoSSL(string host, string request);
string connectoNoSSL(string host, string request);
string cut_header(string feed, int* flags[]);
int myxmlParser(xmlNodePtr root, Entry *Entries, int flag, int *index, xmlDoc *doc);
void vypis(Entry *Entries, int * flags[]);
void processURL(string url, int *flags[]);
string eraseTags(string ok);
int* params(int argc, char*argv[]);
/* global strings (sorry) */
char * cvalue = NULL;
char const *Cvalue = "/etc/ssl/certs";
string oh_my_filename = "";
/* help string */
string HELP = "\narfeed application: \n\
App downloads and prints out informations obtained from atom feeds.\n\n\
              USAGE:   ./arfeed url/to/atom | -f feedfile [-c certfile] [-C certaddr] [-l] [-T] [-a] [-u]\n\
              feedfile - text file consisting of several links to atom files\n\
              certfile - certificate file (.pem)\n\
              certaddr - folder containing certificates\n\
              -l       - print out only the latest entry\n\
              -T       - print update timestamp for every entry\n\
              -a       - print name of author for every entry\n\
              -u       - print url link for every entry\n\n\
Author: Jan Jusko\n\
Brno University of Technology, Faculty of Information Technology\n\
@2015\n";

/************** FUNCTIONS DEFINITION **************/

/* FUNCTION PARSES URL AND RETURNS HOST/REQUEST/PORT ACCORDING TO FLAG */
string parseURL(string filename, int flag){
  int protokol;
  string file="";
  string port="";
  string server="";
  unsigned int position;
  string empty="ERROR";
  char* tmp;
  /* sprav si lokalnu kopiu celeho URL s ktorou sa bude pracovat */
  string localfilename = filename;
  
  /* vyhladaj protokol, implicitny protokol je HTTP, potom ho odrez z URL */
  if(localfilename.find("https://") == 0){
  	protokol = HTTPS;
  	localfilename.erase(localfilename.begin(), localfilename.begin()+8);
  }
  else if(localfilename.find("http://")==0){
  	protokol = HTTP;
  	localfilename.erase(localfilename.begin(), localfilename.begin()+7);
  }
  else{
  	protokol = HTTP;
  }
  /* ZISTI NAZOV DOMENY(domain) A SUBORU(file) */
  /* AK NAJDES ZNAK / TO znamena ze je definovany atom subor, ak neni tak chyba, vrat prazdny string ktory sa potom testuje */
  position=localfilename.find("/");

  if(localfilename.find("/") != string::npos){
    position++;
    if(position != localfilename.length())
      file=localfilename.substr(position);
      localfilename.erase(localfilename.begin()+position-1,localfilename.end());
  }
  else{
    fprintf(stderr, "Input error: File %s to download is not specified in given source URL.\n", localfilename.c_str());
    return empty;
  }
  /* THERE IS :PORT SPECIFIED IN URL, GET IT! */
  position=localfilename.find(":");

  if(localfilename.find(":") != string::npos){
    if(position+1 != localfilename.length())
      port=localfilename.substr(position+1);
    else{
      fprintf(stderr, "Input error: There was ':' character in URL but not in right position.\n");
      return empty;
    }
    /* CHECK IS PORT IS VALID NUMERO */
    strtol(port.c_str(), &tmp, 10);
    if (*tmp){
        printf("Input error: Port was not composed of digits!\n");
        return empty;
    }
    localfilename.erase(localfilename.begin() + position,localfilename.end());
  }
  /* PORT WAS NOT EXPLICITLY SPECIFIED, HAVE TO BE ASSIGNED ACCORDING TO USED PROTOCOL */
  else{
    if(protokol == HTTPS){
      port="443";
    }
    else if(protokol == HTTP){
      port="80";
    }
    else{
      fprintf(stderr, "Unexpected error: Protocol has not been parsed correctly.\n" );
      return empty;
    }
  }
  
  /* last part is server address because we cut everything else */
  if(localfilename != ""){
    server=localfilename;    
  }
  else{
    fprintf(stderr, "Input error: Server address missing.\n" );
    return empty;
  }
  
  /* CREATE OUTPUT  */
  string host = server + ":" + port;
  string rqst = "GET /" + file + " HTTP/1.1\r\nHost: " + server + ":" + port + "\r\nUser-Agent: Jusko\r\nAccept: application/xml;charset=UTF-8\r\nAccept-Charset: UTF-8\r\nAccept-Language: en-US,en;q=0.5\r\nConnection: Close\r\n\r\n"; 

 /* RETURN BASED ON RECEIVED FLAG */
  if(flag == HOST){
    return(host);
  }
  else if(flag == RQST){
    return(rqst);
  }
  else if(flag == ASKPORT){
    return(port);
  }
  else{
    fprintf(stderr, "Unexpected error: Function parseURL received wrong flag parameter.\n");
    return empty;
  }
}

/* FUNCTION CREATES SSL CONNECTION AND RETURNS RESPONSE FROM SERVER IN STRING */
/*  CODE TAKEN FROM IBM TUTORIAL, EDITED FOR MY NEEDS 
    SEE LICENSE: http://www.ibm.com/developerworks/apps/download/index.jsp?contentid=11410&filename=intro-openssl.zip&method=http&locale=
*/
string connectoSSL(string host, string request){
  /* SSL variables and pointers */
  BIO * bio;
  SSL * ssl;
  SSL_CTX * ctx;
  int p;
  char r[1024];
  string response="";
  string error = "ERROR";

  /* Load SSL libraries */
  SSL_library_init (); 
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();
  

  /* Set up the SSL context */
  ctx = SSL_CTX_new(SSLv23_client_method());
  
  /* Load the trust store */
  if(! SSL_CTX_load_verify_locations(ctx, cvalue, Cvalue))
    {
        fprintf(stderr, "Connection error(SSL): Error loading trust store.\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return error;
    }

  /* Setup the connection */
  bio = BIO_new_ssl_connect(ctx);

  /* Set the SSL_MODE_AUTO_RETRY flag */
  BIO_get_ssl(bio, & ssl);  
  SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
  
  /* Create and setup the connection */
  BIO_set_conn_hostname(bio, host.c_str());

  if(BIO_do_connect(bio) <= 0)
    {
        fprintf(stderr, "Connection error(SSL): Error attempting to connect.\n");
        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return error;
    }
  SSL_do_handshake(ssl); 

  /* Check the certificate */
  if(SSL_get_verify_result(ssl) != X509_V_OK)
    {
        fprintf(stderr, "Chyba: nepodařilo se ověřit platnost certifikátu serveru: %s\n", host.c_str());
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return error;
    }

  /* Send the request */
  BIO_write(bio, request.c_str(), strlen(request.c_str()));
  
  /* Read in the response */
  for(;;)
  {
      p = BIO_read(bio, r, 1023);
      if(p <= 0)
        break;
      r[p] = 0;
      response+=string(r);
  }

  /* Close the connection and free the context */
  BIO_free_all(bio);
  SSL_CTX_free(ctx);

  return response;
}

/* FUNCTION CREATES TCP CONNECTION AND RETURNS RESPONSE FROM SERVER IN STRING */
/*  CODE TAKEN FROM IBM TUTORIAL, EDITED FOR MY NEEDS 
    SEE LICENSE: http://www.ibm.com/developerworks/apps/download/index.jsp?contentid=11410&filename=intro-openssl.zip&method=http&locale=
*/
string connectoNoSSL(string host, string request){
  BIO * bio;
  int p;
  char r[1024];
  char char_host[host.length()];
  unsigned int i=0;
  string response = "";
  string error = "ERROR";
  
  /* Set up the library */
  ERR_load_BIO_strings();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();

  /* Create and setup the connection */  
  /* ssl connect function accepts char arrays only, i have to copy received string to char array */
  for(i=0; i<host.length(); i++){
    char_host[i]=host.at(i);
  }
  char_host[i]='\0';

  bio = BIO_new_connect(char_host);

  if(bio == NULL){
    fprintf(stderr, "Connection error(TCP): BIO connection failed: BIO is null.\n");
    return error;
  }

  if(BIO_do_connect(bio) <= 0){
      fprintf(stderr, "Connection error(TCP): BIO connection failed:\n");
      ERR_print_errors_fp(stderr);
      BIO_free_all(bio);
      return error;
  }

  /* Send the request */
  BIO_write(bio, request.c_str(), strlen(request.c_str()));

  
  /* Read in the response */
  for(;;)
  {
      p = BIO_read(bio, r, 1023);
      if(p <= 0) 
        break;
      r[p] = 0;
      response+=string(r);
  }

  /* Close the connection and free the context */

  BIO_free_all(bio);
  return response;

}

/*  FUNCTION RECEIVES RECEIVED FEED, ANALYZES THE HEADER, PERFORMS NEEDED OPERATIONS AND IF EVERYTHING 
    IS OK THEN IT CUTS THE HEADER AND RETURNS CLEAN XML STRING  */
string cut_header(string feed, int* flags[]){
  string newLocation = "";
  string processed = "PROCESSED";
  string text="";

  /* find and store header */
  int pos = feed.find("\r\n\r\n");
  string header=feed.substr(0,pos);
  /* cut header from feed */
  feed=feed.substr(pos+4);
  /* HAVE TO REMOVE CHUNK HEX NUMEROS */
  if(header.find("Transfer-Encoding: chunked") != string::npos){
    string number;
    while(1){
      number=feed.substr(0,feed.find("\r\n"));
      unsigned int x = strtoul(number.c_str(), NULL, 16);
      if(x == 0)
        break;
      feed=feed.substr(feed.find("\r\n")+2);
      text+=feed.substr(0,x);
      feed=feed.substr(x+2);
    }
    feed=text;
  }
  
  /* IS EVERYTHING OK JEFE? */
  if(header.find("200 OK") == string::npos){
    /* something is not alright, gotta check for possible errors */
    if(header.find("301 Moved Permanently") != string::npos){      
      /* get new location */
      if((pos = header.find("Location: ")) != string::npos){ 
        newLocation=header.substr(header.find("Location: ") + LOCATION_LENGTH);
        newLocation=newLocation.substr(0,newLocation.find('\r'));
      }
      else{
        fprintf(stderr, "Server error: Location has been moved and new location is unknown.\n");
        return processed;
      }
      /*  call to process new location, return string PROCESSED which is controlled in parseURL function
          so it doesnt get processed twice
      */
      processURL(newLocation, flags);
      /* oh god this is so clever */
      return processed;
    }
  }
  
  return feed;
}

/* FUNCTION TAKES XML STRING AND PARSES IT, FILLS THE ENTRY STRUCTURES... MUCH CLEVER */
int myxmlParser(xmlNodePtr root, Entry *Entries, int flag, int *index, xmlDoc *doc){
  xmlNodePtr cur_root;
  xmlChar* tmp_str;
  int i=*index;
  
  /* counting mode, count all entries */
  if(flag == COUNT){
    
    for(cur_root = root->children; cur_root != NULL; cur_root = cur_root->next){ 
      if (!(xmlStrcmp(cur_root->name, (const xmlChar*) "entry")))  
        (*index)++;

      if(cur_root->type==XML_ELEMENT_NODE){
        myxmlParser(cur_root, Entries, flag, index, doc);
      }
    }
  }
  else if(flag == FILL){
    for(cur_root = root->children; cur_root != NULL; cur_root = cur_root->next){
      if(cur_root->type == XML_ELEMENT_NODE)
      {
        /* get entry and process it */
      if (!(xmlStrcmp(cur_root->name, (const xmlChar*) "entry"))){
        (*index)++;
      }
      /* get title */
      if (!(xmlStrcmp(cur_root->name, (const xmlChar*) "title"))){ 
        if(xmlIsBlankNode(cur_root->xmlChildrenNode)){
          continue;
        }
        tmp_str = xmlNodeListGetString(doc, cur_root->xmlChildrenNode, 1);
        Entries[i].title = (char*)tmp_str;        
        xmlFree(tmp_str);
      }
      /* get link (href) */
      i=*index;
      if (!(xmlStrcmp(cur_root->name, (const xmlChar*) "link" ))){
        if(xmlHasProp(cur_root,(const xmlChar*)"href")==NULL){
          return 90;
        }

        tmp_str = xmlGetProp(cur_root, (const xmlChar*) "href");
        Entries[i].url = (char*)tmp_str;
        xmlFree(tmp_str);
      }
      /* get last update timestamp */
      if (!(xmlStrcmp (cur_root->name, (const xmlChar*) "updated"))){ 
        if(xmlIsBlankNode(cur_root->xmlChildrenNode)){
          continue;
        }
        tmp_str = xmlNodeListGetString(doc, cur_root->xmlChildrenNode, 1);
        Entries[i].update = (char*)tmp_str;
        xmlFree(tmp_str);
      }
      /* get author */
      if (!(xmlStrcmp(cur_root->name,(const xmlChar*) "name"))){ 
        if(!(xmlStrcmp (root->name, (const xmlChar*) "author"))){
          if(xmlIsBlankNode(cur_root->xmlChildrenNode)){
            continue;
          }
          tmp_str = xmlNodeListGetString(doc, cur_root->xmlChildrenNode, 1);
          Entries[i].author = (char*)tmp_str;        
          xmlFree(tmp_str);
        }
      }

      /* feed structure is always first */
      if((*index)==0)
        Entries[i].type=FEED;
      else
        Entries[i].type=ENTRY;
      /* recursion */
      myxmlParser(cur_root, Entries, flag, index, doc);
      }
    }
  }
  return 0;
}

/* function erases html tags from string, usually called for individual strings when writing out */
string eraseTags(string ok){
  unsigned int start_pos=0;
  unsigned int end_pos=0;
  unsigned int i=0;
  int count=0;
  /* first count how many tags there are */
  for(i=0; i < ok.size(); i++){
    if(ok[i] == '<')
      count++;
  }
  /* delete this number of tags */
  for(i=0; i < count; i++){
    start_pos = ok.find("<");
    end_pos = ok.find(">");
    if(start_pos < end_pos){
      ok.replace(start_pos, end_pos-start_pos+1, "");
    }
  }
  /* thank mr skeltal! */
  return ok;
}

/* make output for one single source, not war */
void vypis(Entry *nove, int* flags[]){
  int i=0;
  Entry emptyEntry;
  /* iterate over entries, find feed entry and print title of the source (feed) */
  while(nove[i].type == FEED || nove[i].type == ENTRY){
    if(nove[i].type == FEED){
      if(nove[i].title != ""){
        printf("*** %s ***", eraseTags(nove[i].title).c_str());
      }
    }
    i++;
  }

  /* -l FLAG ACTIVE, have to delete all entries except the latest (first in our array) */
  if(*flags[3] == 1){
    int oldEntries = 2;
    while(nove[oldEntries].type == FEED || nove[oldEntries].type == ENTRY ){
      nove[oldEntries++] = emptyEntry;
    }
  }
  /* here comes lots of if conditions, I spent lot of time formatting it to desired output so be considerate */
  /* default, always print at least title */
  i=0;
  while(nove[i].type == FEED || nove[i].type == ENTRY){
    if(nove[i].type == ENTRY){
      if(i == 1)
        printf("\n%s", eraseTags(nove[i].title).c_str());
      else{
        if( (*flags[0] == 1 && nove[i].author != "") || (*flags[1] == 1 && nove[i].url != "") || (*flags[2] == 1 && nove[i].update != "") ){
          printf("\n\n%s", eraseTags(nove[i].title).c_str());
        }
        else
          printf("\n%s", eraseTags(nove[i].title).c_str());
      }
      /* url flag active */
      if(*flags[1] == 1){
        if(nove[i].url != ""){
          printf("\nURL: %s", eraseTags(nove[i].url).c_str());
        }
      }
      // if flag -T active check if loaded string and print out
      if(*flags[2] == 1){
        if(nove[i].update != ""){
          printf("\nAktualizace: %s", eraseTags(nove[i].update).c_str());
        }
      }
      // if flag -a active
      if(*flags[0] == 1){
        if(nove[i].author != ""){
          printf("\nAutor: %s", eraseTags(nove[i].author).c_str() );
        }
      }
    }
    i++;
  }
}

/* function processes one source of feed, also makes output */
void processURL(string url, int *flags[]){  
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  int index=0;
  string resp="";
  
  /* LOAD INFO AND RETURN IF FAILED */
  string port = parseURL(url, ASKPORT);
  string host = parseURL(url,HOST);
  string rqsr = parseURL(url,RQST);
  if(port == "ERROR" || host == "ERROR" || rqsr == "ERROR"){
    return;
  }
  /* ASK WHAT CONNECTION TO USE AND THEN FETCH FEED FILE AS A RESPONSE FROM SERVER */  
  if(port.compare("443") == 0){
    /* lets use SSL */
    resp = connectoSSL(host, rqsr);
  }
  else if(port.compare("80") == 0){
    /* use TCP */
    resp = connectoNoSSL(host, rqsr);
  }
  /* catch error */
  if(resp == "ERROR"){
    return;
  }
  /* ANALYZE AND CUT HEADER */
  resp = cut_header(resp, flags);
  /* moved location has been processed already, can skip this */
  if(resp=="PROCESSED"){
    return;
  }
  
  /* PARSING XML OBJECT USING LIBXML2 */
  doc = xmlReadMemory(resp.c_str(),resp.length(),NULL,"UTF-8",1);
  if (doc == NULL) {
    fprintf(stderr, "Error: Downloaded atom feed can't be parsed.\n");
    return;
  }

  /* GET ROOT ELEMENT TO START */
  root_element = xmlDocGetRootElement(doc);
  if(root_element == NULL){
    fprintf(stderr, "Error: Cannot find root element. Probably empty atom file.\n");
    return;
  }

  /* get number of entries to pointed index */
  int chck = myxmlParser(root_element,NULL,COUNT,&index, doc);
  if(chck != 0){
    return;
  }
  
  /* create array of entries, add 1 for feed entry which is always there but doesnt get counted */
  Entry nove[index+1];
  
  index = 0;
  /* lets fill the structures */
  chck = myxmlParser(root_element, nove, FILL, &index, doc);
  if(chck != 0){
    return;
  }
  /* print the entries according to desired format (another sorcery) */
  vypis(nove, flags);
  if(*flags[6] == 1){
    printf("\n\n");
  }else
    printf("\n");
  
  return;
}
/* FUNCTION PARSES PARAMETERS FROM COMMAND LINE AND RETURNS ARRAY OF FLAGS, ALSO SETS GLOBAL STRING VARIABLES OF FILENAME/URL/CERTIFICATES */
int* params(int argc, char*argv[]){
  int aflag = 0;
  int uflag = 0;
  int Tflag = 0;
  int lflag = 0;
  int cflag = 0;
  int Cflag = 0;
  int fflag = 0;
  int urlflag = 1;
  int index;
  /* oh god.. */
  if(argc == 1){
    fprintf(stderr, "Arguments not inserted. \n" );
    exit(99);
  }

  opterr = 0;
  /* search for -f parameter */
  for(index = 0; index<argc; index++){
    if (strcmp(argv[index], "-f") == 0){
      urlflag = 0;
    }
  }    
  /* parsing arguments using getopt */  
  int c = 0;
  while ((c = getopt (argc, argv, "hauTlc:C:f:")) != -1)
    switch (c)
      {
      case 'h':
        printf("%s\n",HELP.c_str());
        exit(5);
      case 'a':
        aflag = 1;
        break;
      case 'u':
        uflag = 1;
        break;
      case 'T':
        Tflag = 1;
        break;
      case 'l':
        lflag = 1;
        break;
      case 'c':
        cvalue = optarg;
        cflag = 1;        
        break;
      case 'C':
        Cvalue = optarg;
        Cflag = 1;        
        break;
      case 'f':
        oh_my_filename = optarg;
        fflag = 1;
        break;
      case '?':
        if (optopt == 'C'){
          fprintf (stderr, "Option -%c requires an argument.\nRun ./arfeed -h for help.\n", optopt);
          exit(99);
        }
        else if (optopt == 'c'){
          fprintf (stderr, "Option -%c requires an argument.\nRun ./arfeed -h for help.\n", optopt);
          exit(99);
        }
        else if (optopt == 'f'){
          fprintf (stderr, "Option -%c requires an argument.\nRun ./arfeed -h for help.\n", optopt);
          exit(99);
        }
        else if (isprint (optopt)){
          fprintf (stderr, "Unknown option `-%c'.\nRun ./arfeed -h for help.\n", optopt);
          exit(99);
        }
        else{
          fprintf (stderr, "Unknown option character `\\x%x'.\nRun ./arfeed -h for help.\n", optopt);
          exit(99);
        }
        return NULL;
      default:
        abort ();
      }
  /* there is url selected */
  if(urlflag == 1){
    oh_my_filename = argv[optind];
    optind++;
  }
  /* check for shady arguments */
  for (index = optind; index < argc; index++){
    printf ("Non-option argument %s\nRun ./arfeed -h for help.\n", argv[index]);
    exit(99);
  }
  /* fill array to return */
  int static FLAG[] = {aflag, uflag, Tflag, lflag, cflag, Cflag, fflag, urlflag};

  /* --------- OSETRENIE CHYBOVYCH VSTUPOV-------- */
  if(fflag == 1 && urlflag == 1){
    fprintf(stderr, "Zadane vstupne URL a subor zaroven! \nRun ./arfeed -h for help.\n");
    exit(99);
  }

  // setting cert flags according to settings???
  if(cflag == 1){
    if(Cflag == 0){
      Cvalue = NULL;  
    }
    /* checking cert file validity */
    FILE *pem = fopen(cvalue, "r");
    if(pem == NULL){
      fprintf(stderr, "Input error: Certificate file is not valid.\n");
      cvalue=NULL;
    }
  }
  /* thanks */
  return FLAG;
}

/*HERE WE GO*/
int main(int argc, char *argv[])
{
  /* parse params and make local copy to work with */
  int* flgstmp = params(argc, argv);
  int* flgs[] = {&flgstmp[0], &flgstmp[1], &flgstmp[2], &flgstmp[3], &flgstmp[4], &flgstmp[5], &flgstmp[6]};
  
  /* JE ZADANE JEDNO KONKRETNE URL */
  if(flgstmp[7] == 1){
    processURL(oh_my_filename, flgs);  
  }
  /* JE ZADANY FEEDFILE */
  else if(flgstmp[6] == 1 && flgstmp[7] == 0){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    /* Open file for reading */
    fp = fopen(oh_my_filename.c_str(),"r");
    if( fp == NULL )
     {
        fprintf(stderr, "Error while opening the file. \n");      
        exit(10);
     }
    /* Read line by line from file */
    while ((read = getline(&line, &len, fp)) != -1) {
      line[read-1] = 0; //cut the linefeed
      if (read == 1 || line[0]=='#')  //empty line or comment line
        continue;
      /* process url link */
      processURL(string(line), flgs);
    }
     free(line);
  }
  /* end, probably needs lot of frees */
 return 1;
}