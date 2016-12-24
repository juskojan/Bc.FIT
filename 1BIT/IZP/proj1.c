/*
 * Soubor:  proj1.c
 * Datum:   2013/11/14
 * Autor:   Jan Jusko, xjusko00@stud.fit.vutbr.cz
 * Projekt: Pocitani slov, projekt c. 1 pro predmet IZP
 * Popis:   Program provadi soucet slov ve vstupnom textu ktere obsahuji uzivatelem specifikovany znak,
 *          uzivatel navic muze specifikovat pozici znaku.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//PROTOTYPY FUNKCII PROGRAMU
int params(int argc, char *argv[], char par[]);
void pocitaj(int d, int h, int dbg, int fpozice);
int myatoi(char *p);

//NAPOVEDA KTERA SE ZOBRAZI PO ZADANI --h
const char *HELPMSG =
  "Program Pocitani slov.\n"
  "Autor: Jan Jusko (c) 2013\n"
  "Program provadi soucet slov ktere obsahuji uzivatelem specifikovany znak, \n"
  "uzivatel navic muze specifikovat pozici znaku. \n\n"
  "Pouziti: proj1 --help      Zpusobi, ze program vytiskne napovedu a skonci. \n"
  "         proj1 X           Reprezentuje hledany znak v pocitanych slovech. \n"
  "         proj1 X [N]       Volitelny argument N predstavuje cislo vyjadrujici \n"
  "                           pozici hledaneho znaku v pocitanych slovech. \n"
  "         proj1 X [N] [-d]  Volitelny argument -d zapina debug rezim. \n"
  "                           Prubezne vypisuje kazde nactene slovo. \n\n"
  "Popis parametru: X      Specialni znak : navic symbolizuje libovolne cislo 0-9,\n"
  "                        znak ^ symbolizuje libovolne velke pismeno A-Z\n"
  "                        znak . symbolizuje libovolny znak.\n"
  "                 N      Slovo musi mit hledany znak prave na dane pozici.\n"
  "                        Cislo N je kladne - prvni znak ve slove ma pozici 1.\n\n"
  "Program cte zpracovavany text ze standardniho vstupu (stdin).\n"
  "Veskere informace vypisuje na standardni vystup (stdout).\n"
  "Slovem se v tomto projektu povazuje libovolna posloupnost alfanumerickych znaku\n"
  "bez diakritiky (a-z, A-Z, 0-9) vcetne pomlcky (znamenko -) a podtrzitka _.\n"
  "Vsechny ostatni znaky vcetne ridicich znaku predstavuji oddelovace slov. \n"
  "Ladici rezim pocita s omezenim maximalni delky slova 80 znaku.\n";

//FUNKCIA NAPREVOD STRING TO INTEGER
int myatoi(char *p)
{
  int vysledek = 0;
  while (*p!='\0')
    {
        vysledek = (vysledek*10) + (*p) - '0';
        p++;
    }
  return vysledek;
}

//FUNKCIA NA SPRACOVANIE PARAMETROV FUNKCIE MAIN
int params(int argc, char *argv[], char par[])
{
    int i=0;
    while(i<argc)
    {
        if (strcmp(argv[i],par)==0)
        {
            break;
        }
        else
        {
            i++;
        }
    }
     if (i<argc)
     {
         return 1;
     }
     else
     {
         return 0;
     }
}

//FUNKCIA NA POCITANIE SLOV SPOLU S MOZNOSTOU DEBUG REZIMU A HLADANIM POZICE
void pocitaj(int d, int h, int dbg, int fpozice)
{
    int vyskyt=0;           //PRIZNAK VYSKYTU HLADANEHO PISMENA V SLOVE
    int sucetslov=0;        //POCET SLOV S VYSKYTOM HLEDANEHO ZNAKU
    int c;                  //PREMENNA NA NACITANIE ZNAKU S STDIN
    int bound=0;            //KONTROLA HRANICE 80 PISMEN V DEBUG REZIME
    int medzera=0;          //PRIZNAK MEDZERY
    int aktpozice=0;        //AKTUALNA POZICE NACTENEHO SLOVA
    int pvyskyt=0;          //PRIZNAK VYSKYTU HLADANEHO PISMENA NA HLADANEJ POZICI
    int psucet=0;           //POCET SLOV S VYSKYTOM HLEDANEHO SLOVA NA HLEDANEJ POZICI
    int podmienka=0;        //PODMIENKA HLEDANEHO ZNAKU

    while ((c=getchar())!=EOF)
    {
        if(d==0 && h==0)    //PODMINKA PRO SPECIALNY ZNAK '.'
           {
               if ((c>='0' && c<='9') || (c>='A' && c<='Z') || (c>'a' && c<'z') || (c=='-') || (c=='_'))
               {
                   podmienka=1;
               }
               else
               {
                   podmienka=0;
               }
           }
        else
        {
            if(c>=d && c<=h) //POCITANIE OSTATNYCH ZNAKOV PLUS SPECIALNE ZNAKY : ^
            {
                podmienka=1;
            }
            else
            {
                podmienka=0;
            }
        }
      if (podmienka)    // AK NACITANY ZNAK JE HLADANY ZNAK
      {
          aktpozice++;
          vyskyt++;
          if (fpozice!=0 && aktpozice==fpozice)    // PROBEHNE JENOM KDYZ JE ZADANA NENULOVA HLEDANA POZICE - KDYZ JE ZADAN [N]
          {
              pvyskyt++;
          }
          if (dbg==1 && bound<80)           //PROBEHNE JENOM KDYZ JE ZADANY PARAMETR -d
          {
              putchar(c);
              bound++;
              medzera=0;
          }
      }
      else          // NACTENY ZNAK NENI HLEDANY ZNAK
      {
        if ((c<'0' || c>'9') && (c<'A' || c>'Z') && (c<'a' || c>'z') && (c!='_') && (c!='-'))  // NACTENY ZNAK JE ODDELOVAC
        {
            aktpozice=0;

            if (pvyskyt!=0)     //KDYZ PRIZNAK VYSKYTU JE NENULOVY TAK INKREMENTUJ SUCET A VYNULUJ VYSKYT
            {
                psucet++;
                pvyskyt=0;
            }
            if (vyskyt!=0)      //KDYZ PRIZNAK VYSKYTU JE NENULOVY TAK INKREMENTUJ SUCET A VYNULUJ VYSKYT
            {
            sucetslov++;
            vyskyt=0;
            }
            if(dbg==1 && medzera==0)  //V DEBUG REZIMU ZARIADKUJ A PRIZNAK MEDZERY = 1
               {
                   bound=0;
                   printf("\n");
                   medzera=1;
               }
        }
        else  // NACTENY ZNAK JE PLATNY ZNAK KTERY NENI HLEDANY
        {
            aktpozice++;
            if (dbg==1 && bound<80)   // VYPIS PLATNEHO ZNAKU V DEBUG REZIME
            {

                putchar(c);
                medzera=0;
                bound++;
            }
        }
      }
    }

    if (pvyskyt!=0)  // OSETRENI HLEDANEHO ZNAKU V POSLEDNOM SLOVE
    {
        psucet++;
    }
    if (vyskyt!=0)
    {
        sucetslov++;
    }

    if (fpozice!=0)  //ROZHODOVANI JESTLI VYPSAT S POZICI NEBO NE
    {
    printf("%d\n",psucet);
    }
    else
    {
        printf("%d\n",sucetslov);
    }
}
// MAIN FUNKCIA
int main(int argc, char *argv[])
{
    int pozice=0;
    int d=0;
    if (argc==1)
    {
        printf("Parametry neboli zadany.");
        return 0;
    }

    if (params(argc,argv,"--h"))  //KDYZ JE ZADAN PARAMETR --h TAK SE VYPISE NAPOVEDA A PROGRAM SKONCI
    {
        printf(HELPMSG);
        return 0;
    }
    if (params(argc,argv,"-d"))   //KDYZ JE ZADAN PARAMETR -d TAK PRIZNAK DEBUG REZIMU JE NASTAVEN NA 1
    {
        d=1;
    }

    if (argc==2)   //NACTENI CISLO HLEDANE POZICE DO PROMENNE "pozice"
    {
        pozice=0;  //PARAMETR [N] NENI ZADANY
    }
    if (argc==4)   //JSOU ZADANY VSECHNY 3 PARAMETRY, NACTE SE DRUHY V PORADI
    {
        pozice=(myatoi(argv[2]));
    }
    if ((argc==3) && ((params(argc,argv,"-d"))==0))  //JSOU ZADANY JENOM DVE PARAMETRY, POTREBA OVERIT ZDA 2. PAR. JE PAR. [N]
    {
        pozice=(myatoi(argv[2]));
    }

    switch (argv[1][0])     //ROZHODOVANI O HLEDANEM ZNAKU A VOLANI FUNKCE KT. POCITA S DANYM INTERVALEM VYHOVUJICICH ZNAKU
    {
    case ':':
        pocitaj('0','9',d,pozice);
        return 0;
    case '^':
        pocitaj('A','Z',d,pozice);
        return 0;
    case '.':
        pocitaj(0,0,d,pozice);      // PODMINKA PRO SPECIALNY ZNAK '.' JE OSETRENA VE FUNKCI
        return 0;
    default:
        pocitaj((argv[1][0]),(argv[1][0]),d,pozice);
        return 0;
    }

return 0;
}
