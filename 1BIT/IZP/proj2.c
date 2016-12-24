/*
 * Soubor:  proj2.c
 * Datum:   2013/11/26
 * Autor:   Jan Jusko, xjusko00@stud.fit.vutbr.cz
 * Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP
 * Popis:   Program provadi vypocet a vypis uhlov obecneho trojuholnika. Trojuholnik je dany tromi vrcholmi.
 *          Program dalej pocita aj odmocninu zo zadaneho cisla a hodnotu arkus sinus zo zadanej hodnoty.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NAN 0.0/0.0   //KONSTANTA NAN (NOT A NUMBER)
//PROTOTYPY FUNKCII
double myabs(double x);
double taylor_asin(double x);
int params(int argc, char *argv[], char par[]);
double mypow (double x);
double my_sqrt(double x);
double taylor_asin(double x);

//KONSTANTY
const double PIPOL = 1.57079632679489661923;
const char *HELPMSG=
"Program proj2.c\n"
"Autor: Jan Jusko\n"
"Program pocita arcsin nebo odmocninu ze zadaneho cisla a velikosti uhlu\n"
"zadaneho obecneho trojuhelnika.\n\n"
"Pouziti: ./proj2 --h\n"
"         ./proj2 --sqrt [N]\n"
"         ./proj2 --asin [N]\n"
"         ./proj2 --triangle AX AY BX BY CX CY\n\n"
"Popis parametru:\n"
"  --h   		        Vypise obrazovku s napovedou a program skonci.\n"
"  --asin [N]    	        Provede vypocet arcsin se zadaneho cisla N.\n"
"  --sqrt [N]     	        Provede vypocet odmocniny ze zadaneho cisla N.\n"
"  --triangle AX AY BX BY CX CY  Vypocita uhly v obecnem trojuhelniku se zadanych"
"                                sourednic.\n\n"
"Odmocnina je definovana jenom pro nezaporna realna cisla (N>=0).\n"
"ArcSinus je definovan jenom pro realna cisla v intervalu <-1,1>.\n\n";

//FUNKCIA NA POCITANIE ODMOCNINY POMOCI NEWTONOVY METODY
double my_sqrt(double x)
{
    double starey=1.0;
    double novey;
    double eps1=1.0e-11;    //EPSILON PRESNOST

    if (x==0)               //ODMOCNINA Z NULY JE NULA
        return 0;
    if (x<0)                //ODMOCNINA JE DEFINOVANA JENOM Z NEZAPORNYCH CISEL
        return NAN;
    if (x>0)
    {
        novey=(((x/starey)+starey)/2);              //ALGORITMUS NEWTONOVY METODY
        while (myabs(novey-starey)>=(eps1*novey))   //RELATIVNI PRESNOST
        {
            starey=novey;
            novey=(((x/starey)+starey)/2);
        }
        return novey;
    }
    return -1;      //CHYBA
}

//FUNKCIA ABSOLUTNA HODNOTA ZO ZADANEHO CISLA
double myabs(double x)
{
    if (x>=0)
    {
        return x;
    }
    else
    {
        return -x;
    }
}
//VYPOCET ARCUS SINUS POMOCI TAYLOROVY RADY
double taylor_asin(double x)
{
	double eps=1.0e-11;  //EPSILON PRESNOST
	double rozdiel=1.0;
	double novyvys=x;
	double staryvys=1.0;
	double zlomok1=1.0;
	double zlomok2=x;
	int n=0.0;

	if (x==1)               //ARCSIN(1) == PI/2
        return PIPOL;
    if (x==-1)
        return -PIPOL;      //ARCSIN(-1) == -PI/2
    if(x<1 && x>-1)
    {                       //ALGORITMUS APROXIMACI ARCSINUSU (TAYLOROVA RADA)
		while(rozdiel>eps)  //ABSOLUTNI PRESNOST
		{
			rozdiel = myabs(novyvys-staryvys);
			staryvys=novyvys;
			zlomok1 = zlomok1*((1.0+n)/(2.0+n));
			zlomok2 = zlomok2*x*x;
			novyvys = novyvys + (zlomok1*(zlomok2/(3+n)));
			n=n+2;
		}
	}
	else
	{
		novyvys = NAN;            //ARCSIN JE DEFINOVAN JENOM V INTERVALU <-1,1>
	}

	return novyvys;
}

double my_asin(double x)        //FUNKCE KTERA JE VOLANA PRO VYPOCTY, ZOHLEDNUJE HRANICNI HODNOTY A OPTIMALIZUJE VYPOCET
{
    double vys;

    if ((x>=0.75 && x<1) || (x>-1 && x<=-0.75))     //OPTIMALIZACE VYPOCTU PRO HODNOTY BLIZKE |1|
    {
        vys=PIPOL-(taylor_asin(my_sqrt(1-mypow(x))));
    }
    else if ((x<0.75 && x>-0.75) || (x==1) || (x==-1))                    //PRO OSTATNE HODNOTY POUZIJ JENOM TAYLOROVU RADU
         {
            vys=taylor_asin(x);
         }
         else
         {
            vys=NAN;                                //ARKUS SINUS JE DEFINOVAN JENOM V INTERVALU <-1,1>
         }

    return vys;
}

//FUNKCIA NA VYHLEDAVANI RETEZCE MEZI PARAMETRY
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

//FUNKCIA UMOCNENIE NA DRUHU
double mypow (double x)
{
    return x*x;
}

//MAIN
int main(int argc,char *argv[])
{
    char *test;

    if(argc==1)         //CHYBOVY STAV - NEZADANY PARAMETER
    {
        fprintf(stderr,"Nezadali ste parametre, pre napovedu zadajte parameter --h\n");
        return -1;
    }
//NAPOVEDA
    if (params(argc,argv,"--h"))
    {
        printf("%s",HELPMSG);
        return 0;
    }
//ODMOCNINA
    if (params(argc,argv,"--sqrt"))
    {
        double sqrt;            //PROMENNA NA NACITANI ZADANE HODNOTY
        sqrt=(strtod(argv[2],&test));
        if (*test!='\0')        //OSETRENI SPATNE ZADANEHO CISLA
        {
            fprintf(stderr,"Nespravny format cisla! Pro napovedu zadejte argument --h.");
            return -1;
        }
        printf("%.10e",my_sqrt(sqrt));
    }
//ARCUS SINUS
    if (params(argc,argv,"--asin"))
    {
        double arc;                 //PROMENNA NA NACITANI ZADANE HODNOTY Z PARAMETRU
        arc=(strtod(argv[2],&test));
        if (*test!='\0')            //OSETRENI SPATNE ZADANEHO CISLA
        {
            fprintf(stderr,"Nespravny format cisla! Pro napovedu zadejte argument --h.");
            return -1;
        }
        printf("%.10e",my_asin(arc));
    }
//TROJUHOLNIK
    if(params(argc,argv,"--triangle"))
    {
        double pole[6];

        if(argc>7)          //NACITANIE SURADNIC DO POLA
        {
            for (int i=0;i<6;i++)
            {
                double crdnt;                      //POMOCNA PREMENNA NA NACITANIE Z PARAMETRU
                crdnt=(strtod(argv[i+2],&test));
                if (*test!='\0')                    //OSETRENI SPATNE ZADANE SURADNICE
                {
                    fprintf(stderr,"Nespravny format cisla! Pro napovedu zadejte argument --h.");
                    return -1;
                }
                pole[i]=crdnt;
            }
        }
                                        //VYPOCET DLZOK STRAN TROJUHOLNIKA
        double c = my_sqrt((mypow(pole[0]-pole[2]))+(mypow(pole[1]-pole[3])));
        double b = my_sqrt((mypow(pole[0]-pole[4]))+(mypow(pole[1]-pole[5])));
        double a = my_sqrt((mypow(pole[2]-pole[4]))+(mypow(pole[3]-pole[5])));
                                        //VYPOCET KOSINU UHLOV ALFA, BETA, GAMA
        double cosC = ((a*a)+(b*b)-(c*c))/(2*a*b);
        double cosB = ((a*a)+(c*c)-(b*b))/(2*a*c);
        double cosA = ((b*b)+(c*c)-(a*a))/(2*b*c);
                                        //VYPOCET UHLOV V RADIANOCH POMOCOU VZTAHU MEDZI ARCSIN A ARCCOS
        double uholC=(PIPOL)-(my_asin(cosC));
        double uholA=(PIPOL)-(my_asin(cosA));
        double uholB=(PIPOL)-(my_asin(cosB));
                                        // OSETRENI TROJUHOLNIKOVE NEROVNOSTI A OSETRENI BODY LEZICI NA PRIMCE
        if ((a+b<c)||(b+c<a)||(a+c<b)||(uholA==0.0)||(uholB==0.0)||(uholC==0.0))
        {
            uholA=NAN;
            uholB=NAN;
            uholC=NAN;
        }
        printf("%.10e\n%.10e\n%.10e",uholA,uholB,uholC); //VYPIS
    }
return 0;
}
