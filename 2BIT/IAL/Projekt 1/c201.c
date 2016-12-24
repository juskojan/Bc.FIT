
/* c201.c *********************************************************************}
{* Téma: Jednosmìrný lineární seznam
**
**                     Návrh a referenèní implementace: Petr Pøikryl, øíjen 1994
**                                          Úpravy: Andrea Nìmcová listopad 1996
**                                                   Petr Pøikryl, listopad 1997
**                                Pøepracované zadání: Petr Pøikryl, bøezen 1998
**                                  Pøepis do jazyka C: Martin Tuèek, øíjen 2004
**	                                      Úpravy: Bohuslav Køena, øíjen 2014
**
** Implementujte abstraktní datový typ jednosmìrný lineární seznam.
** U¾iteèným obsahem prvku seznamu je celé èíslo typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou typu tList.
** Definici konstant a typù naleznete v hlavièkovém souboru c201.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu s vý¹e
** uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DisposeList ... zru¹ení v¹ech prvkù seznamu,
**      InsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zru¹í první prvek seznamu,
**      PostDelete .... ru¹í prvek za aktivním prvkem,
**      PostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na dal¹í prvek seznamu,
**      Active ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci funkcí nevolejte ¾ádnou z funkcí implementovaných v rámci
** tohoto pøíkladu, není-li u dané funkce explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int solved;
int errflg;

void Error() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální promìnná -- pøíznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
	L->Act = NULL;
	L->First = NULL;

 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void DisposeList (tList *L) {
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Ve¹kerá pamì» pou¾ívaná prvky seznamu L bude korektnì
** uvolnìna voláním operace free.
***/
    tElemPtr pomUk = NULL;
    while(L->First != NULL)
    {
        pomUk = L->First;
        L->First = pomUk->ptr;
        free(pomUk);
    }

    L->Act = NULL;


 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void InsertFirst (tList *L, int val) {
/*
** Vlo¾í prvek s hodnotou val na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci Error().
**/
    struct tElem *tmpEl;
    tmpEl = malloc(sizeof(struct tElem));
    if (tmpEl == NULL)
    {
        Error();
        return;
    }
    else
    {
        tmpEl->data = val;
        tmpEl->ptr = L->First;
        L->First = tmpEl;
    }

 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný pøíkaz, ani¾ byste testovali,
** zda je seznam L prázdný.
**/
    L->Act = L->First;

 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void CopyFirst (tList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
    if(L->First != NULL)
    {
        *val = L->First->data;
    }
    else if(L->First == NULL)
    {
        Error();
        return;
    }


 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void DeleteFirst (tList *L) {
/*
** Zru¹í první prvek seznamu L a uvolní jím pou¾ívanou pamì».
** Pokud byl ru¹ený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se nedìje.
**/

    if(L->First == L->Act)
    {
        L->Act = NULL;
    }

    if(L->First == NULL)
    {
        return;
    }


    if(L->First != NULL)
    {
        tElemPtr pomUk = L->First;
        L->First = L->First->ptr;
        free(pomUk);
    }

 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void PostDelete (tList *L) {
/*
** Zru¹í prvek seznamu L za aktivním prvkem a uvolní jím pou¾ívanou pamì».
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se nedìje.
**/
	if(L->First == NULL || L->Act->ptr == NULL)
    {
        return;
    }

    tElemPtr pomUk = L->Act->ptr;
    L->Act->ptr = pomUk->ptr;
    free(pomUk);

 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void PostInsert (tList *L, int val) {
/*
** Vlo¾í prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje!
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** zavolá funkci Error().
**/
    if(L->First == NULL)
    {
        return;
    }

    struct tElem *tmpEl;
    tmpEl = malloc(sizeof(struct tElem));
    if(tmpEl == NULL)
    {
        Error();
        return;
    }

    tmpEl->data = val;
    tmpEl->ptr = L->Act->ptr;
    L->Act->ptr = tmpEl;


 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void Copy (tList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
	if(L->Act == NULL)
    {
        Error();
        return;
    }
    else
    {
        *val = L->Act->data;
    }

//solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void Actualize (tList *L, int val) {
/*
** Pøepí¹e data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedìlá nic!
**/

	if(L->Act == NULL)
    {
        return;
    }
    else
    {
        L->Act->data = val;
    }

//solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** V¹imnìte si, ¾e touto operací se mù¾e aktivní seznam stát neaktivním.
** Pokud seznam L není aktivní, nedìlá nic.
**/
	if(L->Act == NULL)
    {
        return;
    }
    else
    {
        L->Act = L->Act->ptr;
    }

 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

int Active (tList *L) {
/*
** Je-li seznam L aktivní, vrací TRUE. V opaèném pøípadì vrací FALSE.
** Tuto funkci implementujte jako jediný pøíkaz return.
**/
	return L->Act != NULL;

 //solved = FALSE;                   /* V pøípadì øe¹ení, sma¾te tento øádek! */
}

/* Konec c201.c */
