#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * Soubor:  proj3.c
 * Datum:   2013/12/9
 * Autor:   Jan Jusko, xjusko00@stud.fit.vutbr.cz
 * Projekt: Hledani obrazcu, projekt c. 1 pro predmet IZP
 */

const char *HELPMSG =
  "Program Hledani obrazcu.\n"
  "Autor: Jan Jusko (c) 2013\n"
  "Program hleda v monochromatickem obrazku nejdelsi vodorovne a svisle usecky a nejvetsi ctverce. \n"
  "Obrazek je ulozen ako bitmapa v textovem souboru ve forme matice nul a jednicek.\n"
  "Cilem programu je vypis pocatecni a koncove souradnice\n"
  "nalezene usecky nebo ctverce.\n"
  "\n\n"
  "Pouziti: proj3 --help      Zpusobi, ze program vytiskne napovedu a skonci. \n"
  "         proj3 --test      Pouze zkontroluje, ze soubor dany druhym argumentem\n"
  "                           programu obsahuje radny bitmapovy obrazok.\n"
  "         proj3 --vline     V obrazku nalezne a vytiskne pocatecni a koncovou\n"
  "                           souradnici prvni nejdelsi vertikalni usecky.\n"
  "                           pozici hledaneho znaku v pocitanych slovech. \n"
  "         proj1 --hline     V obrazku nalezne a vytiskne pocatecni a koncovou\n"
  "                           souradnici prvni nejdelsi horizontalni usecky. \n"
  "                           \n";

typedef struct {        //DEFINICE STRUKTURY Bitmap
    int rows;
    int cols;
    char *cells;
}Bitmap;

void alloc2D(Bitmap *a, int rows, int cols) //FUNKCIA NA ALOKACIU PAMETE A INICIALIZACIU STRUKTURY
{
    a->cells=malloc(rows*cols*sizeof(char));
    if (a->cells!=NULL)
    {
        a->rows=rows;
        a->cols=cols;
    }
    return;
}

void free2D(Bitmap *a)              //FUNKCIA NA UVOLNENIE ALOKOVANEJ PAMETE
    {
    	if (a!=NULL && a->cells!=NULL)
    	{
    	free(a->cells);
    	a->cells=NULL;
    	a->rows=0;
    	a->cols=0;
    	}
    }

int test(char *arg)     //FUNKCIA NA TESTOVANIE VALIDITY SUBORU
{
    FILE *soubor;       //OTVORENIE SUBORU
    soubor=fopen(arg,"r");

    int a;
    int b;
    int i=0;    //PRIZNAK INVALID
    char ch;
    int n=0;
    int sum=0;
    char oldch;

    n=fscanf(soubor,"%d %d",&a,&b); //NACITANIE ROZMEROV MATICE
    if (n!=2)
    {
		printf("Invalid\n");
		return -1;
    }

    while ((ch=fgetc(soubor))!=EOF) //NACITAVAJ KYM NENI KONEC SUBORU
    {
        if((oldch=='1' || oldch=='0') && (ch=='1' || ch=='0')) //KDYZ BITY NEJSOU ODDELENY BILYM ZNAKEM
            i=1;

        if (ch>32 && ch!='1' && ch!='0')        //KDYZ JE NACTENY ZNAK NEVALIDNY
        {
            i=1;
        }
        else if (ch=='1' || ch=='0')            //POCITADLO JEDNICEK A NUL
        {
            sum++;
        }
        oldch=ch;
    }

    if ((i==1) || (a*b!=sum))                   //AK JE PRIZNAK INVALID NASTAVENY NA 1 ALEBO AK ROZMER NEODPOVEDA POCTU
    {
        fclose(soubor);
        return 0;                               //VRAT 0 (INVALID)
    }
    else
    {
        fclose(soubor);
        return 1;                               //VRAT 1 (VALID)
    }

}

char getcolor(Bitmap *bitmap, int x, int y)     //MAPOVACIA FUNKCIA MATICE
{
return bitmap->cells[y*bitmap->cols +x];
}

int naplni(char *arg,Bitmap *ax)                // FUNKCIA NA NAPLNENIE POLA CELLS MATICOU ZO SUBORU
{
    FILE *soubor;
    soubor=fopen(arg,"r");

    int a;
    int b;
    char ch;
    int n=0;
    int x=0;

    n=fscanf(soubor,"%d %d",&a,&b);
    if (n!=2)
    {
		return -1;
    }

    while ((ch=fgetc(soubor))!=EOF)
    {
        if (ch=='1' || ch=='0')
        {
            ax->cells[x]=ch;
            x++;
        }
    }

    fclose(soubor);
    return 1;
}

int find_hline(Bitmap *a, int *x1, int *y1, int *x2, int *y2)   //HLADANIE NAJDLHSEJ HORIZONTALNEJ CIARY
{
    int dlzka,maxdlzka=0;
    int tx1,tx2,ty1,ty2;
    int occ;
    char ch;
    int test=0;

    for(int j=0;j<a->rows;j++)  //FUNKCIA PRECHADZA MATICU PO RIADKOCH ZLAVA DOPRAVA
    {
        dlzka=0;        //NULOVANIE PRIZNAKU occ A DOCASTNYCH SURADNIC NA KONCI RIADKA
        tx1=0;
        tx2=0;
        ty1=0;
        ty2=0;
        occ=0;

        for (int i=0;i<(a->cols)-1;i++)
        {
            if ((ch=getcolor(a,i,j))=='1' && (ch=getcolor(a,i+1,j))=='1') //AK SU DVE SUSEDNE BITY JEDNICKY
            {
                test++;                 //NAVRATOVA HODNOTA, PRIZNAK NAJDENIA CIARY
                if(occ==0)              //PRIZNAK, ZE PRAVE PREBIEHA CIARA (ABY SA NEPREPISOVALA ZACIATOCNA SURADNICA)
                {
                    tx1=j;              //ZAPIS ZACIATOCNU SURADNICU
                    ty1=i;
                    occ=1;              //NASTAV PRIZNAK ZE PRAVE PREBEIHA CIARA
                }
                tx2=j;                  //ZAPIS KONCOVU SURADNICU
                ty2=i+1;
                dlzka++;                //INKREMENTUJ DLZKU

                if (dlzka>maxdlzka)     //ZAPIS SURADNIC NAJDLHSEJ CIARY
                {
                    maxdlzka=dlzka;
                    *x1=tx1;
                    *x2=tx2;
                    *y1=ty1;
                    *y2=ty2;
                }
            }
            else
            {
                occ=0;                  //NULOVANIE PRIZNAKU
            }
        }
    }

    return test;
}

int find_vline(Bitmap *a, int *x1, int *y1, int *x2, int *y2)   //HLADANIE NAJDLHSEJ VERTIKALNEJ CIARY
{
    int dlzka,maxdlzka=0;
    int tx1,tx2,ty1,ty2;
    int occ;
    int test=0;
    char ch;

    for(int j=0;j<(a->cols)-1;j++)  //FUNKCIA PRECHADZA MATICU PO STLPCOCH, ZHORA DOLE
    {
        dlzka=0;                    //NULOVANIE PRIZNAKU occ A DOCASTNYCH SURADNIC NA KONCI STLPCA
        tx1=0;
        tx2=0;
        ty1=0;
        ty2=0;
        occ=0;

        for (int i=0;i<(a->rows)-1;i++)
        {

            if ((ch=getcolor(a,j,i))=='1' && (ch=getcolor(a,j,i+1))=='1')   //DVE SUSEDNE BITY SU JEDNICKA
            {
                test++;             //NAVRATOVA HODNOTA
                if(occ==0)          //PRIZNAK, ZE PRAVE PREBIEHA CIARA (ABY SA NEPREPISOVALA ZACIATOCNA SURADNICA)
                {
                    tx1=i;          //ZAPIS ZACIATOCNEJ SURADNICE
                    ty1=j;
                    occ=1;
                }
                tx2=i+1;            //ZAPIS KONCOVEJ SURADNICE A INKREMENTACIA DLZKY
                ty2=j;
                dlzka++;

                if(dlzka==maxdlzka && tx1<*x1)  //AK JE VIACERO CIAR ROVNAKO DLHYCH, ZAPISE SA TA, KTORA MA MENSIU SURADNICU RIADKU
                {
                    *x1=tx1;
                    *x2=tx2;
                    *y1=ty1;
                    *y2=ty2;
                }

                if (dlzka>maxdlzka)     //ZAPIS SURADNIC NAJDLHSEJ CIARY
                {
                    maxdlzka=dlzka;
                    *x1=tx1;
                    *x2=tx2;
                    *y1=ty1;
                    *y2=ty2;
                }
            }
            else
            {
                occ=0;      //VYNULOVANIE PRIZNAKU PRIEBEHU CIARY
            }
        }
    }

    return test;
}

int find_square(Bitmap *a, int *x1, int *y1, int *x2, int *y2)  //HLADANIE NAJVACSIEHO CTVERCA
{
    int stvorec=0;
    int maxstvorec=0;
    char ch;
    int n=0;


    for (int r=0;r<a->rows-1;r++)       //FUNKCIA PRECHADZA KAZDY PRVOK MATICE PO RIADKOCH ZLAVA DOPRAVA
    {
       stvorec=0;

        for (int s=0;s<a->cols-1;s++)
        {
            stvorec=0;

            for (int p=0;p<a->rows+1;p++) //KAZDEMU BITU TESTUJE VSETKY PRIPUSTNE BITY PO DIAGONALE, S KTORYM BY MOHLI TVORIT CTVEREC
            {

                if((ch=getcolor(a,s,r))=='1' && (ch=getcolor(a,s+p,r+p))=='1')  //AK SU OBE JEDNICKY BUDE TESTOVAT CI NAOZAJ TVORIA CTVEREC
                {

                    for (int t=1;t<p+1;t++) //TEST
                    {

                        if ((ch=getcolor(a,s+t,r))=='1' && (ch=getcolor(a,s,r+t)=='1') && (ch=getcolor(a,s+p-t,r+p))=='1' &&(ch=getcolor(a,s+p,r+p-t)=='1'))
                        {
                            stvorec++; //POCITADLO VELKOSTI CTVERCA
                        }
                        else
                        {
                            break; //NENI TO CTVEREC
                        }
                    }

                    if (stvorec>maxstvorec) //ZAPIS SURADNIC NEJVECSIHO CTVERCE
                    {
                        *x1=r;
                        *y1=s;
                        *x2=r+p;
                        *y2=s+p;
                        n=1;        //PRIZNAK ZE NEJAKY CTVEREC SE NASEL -- NAVRATOVA HODNOTA
                        maxstvorec=stvorec;
                    }
                    stvorec=0;
                }
            }
        }
    }

return n;
}

int main(int argc, char *argv[])
{
    Bitmap b;
    Bitmap *a;
    a=&b;
    int x,y;
    int x1=0;
    int y1=0;
    int x2=0;
    int y2=0;
    int n;

    if (argc==1)            //OSETRENIE NEZADANYCH PARAMETROV
    {
        fprintf(stderr,"Nezadane parametry\n");
        return 0;
    }

    if ((strcmp(argv[1],"--help"))==0)  //VYPIS NAPOVEDY
    {
        printf("%s",HELPMSG);
        return 0;
    }

    FILE *soubor;                       //OTVORENIE SUBORU NA NACITANIE ROZMEROV MATICE
    soubor=fopen(argv[2],"r");
    if (soubor==NULL)
    {
        fprintf(stderr,"Nepodarilo sa otvorit subor\n");
        return 0;
    }
    fscanf(soubor,"%d%d",&x,&y);
    fclose(soubor);

    alloc2D(a,x,y);                     //ALOKACIA PAMETE PRE POLE CELLS

    if (strcmp(argv[1],"--test")==0)    //AK JE PARAMETER --test
    {
        if ((test(argv[2]))==0)
        {
            printf("Invalid\n");
        }
        else printf("Valid\n");
    }

    naplni(argv[2],a);                  //NAPLNI POLE CELLS CISLAMI MATICE SO SUBORU

    if (strcmp(argv[1],"--hline")==0)   //HLADANIE HLINE
    {
        if ((test(argv[2]))==0)         //TESTOVANIE VALIDNOSTI MATICE
        {
            fprintf(stderr,"Invalid\n");
            return 0;
        }
        n = find_hline(a,&x1,&y1,&x2,&y2);
        if(n!=0)
        {
            printf("%d %d %d %d\n",x1,y1,x2,y2);
        }
        else fprintf(stderr,"Not found\n");//OBRAZEC SA NENASIEL
    }

    if (strcmp(argv[1],"--vline")==0)   //HLADANIE VLINE
    {
        if ((test(argv[2]))==0)         //TESTOVANIE VALIDNOSTI MATICE
        {
            fprintf(stderr,"Invalid\n");
            return 0;
        }
        n = find_vline(a,&x1,&y1,&x2,&y2);
        if(n!=0)
        {
            printf("%d %d %d %d\n",x1,y1,x2,y2);
        }
        else fprintf(stderr,"Not found\n");   //OBRAZEC SA NENASIEL
    }

    if (strcmp(argv[1],"--square")==0)  //HLADANIE CTVERCA
    {
        if ((test(argv[2]))==0)         //TESTOVANIE VALIDNOSTI MATICE
        {
            fprintf(stderr,"Invalid\n");
            return 0;
        }
        n=find_square(a,&x1,&y1,&x2,&y2);
        if(n!=0)
        {
            printf("%d %d %d %d\n",x1,y1,x2,y2);
        }
        else fprintf(stderr,"Not found\n");   //OBRAZEC SA NENASIEL

    }

    free2D(a);                          //UVOLNENIE ALOKOVANEJ PAMETE
    return 0;
}
