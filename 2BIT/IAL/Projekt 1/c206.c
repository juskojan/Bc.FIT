
/* c206.c **********************************************************}
{* T�ma: Dvousm�rn� v�zan� line�rn� seznam
**
**                   N�vrh a referen�n� implementace: Bohuslav K�ena, ��jen 2001
**                            P�epracovan� do jazyka C: Martin Tu�ek, ��jen 2004
**                                            �pravy: Bohuslav K�ena, ��jen 2014
**
** Implementujte abstraktn� datov� typ dvousm�rn� v�zan� line�rn� seznam.
** U�ite�n�m obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prom�nnou
** typu tDLList (DL znamen� Double-Linked a slou�� pro odli�en�
** jmen konstant, typ� a funkc� od jmen u jednosm�rn� v�zan�ho line�rn�ho
** seznamu). Definici konstant a typ� naleznete v hlavi�kov�m souboru c206.h.
**
** Va��m �kolem je implementovat n�sleduj�c� operace, kter� spolu
** s v��e uvedenou datovou ��st� abstrakce tvo�� abstraktn� datov� typ
** obousm�rn� v�zan� line�rn� seznam:
**
**      DLInitList ...... inicializace seznamu p�ed prvn�m pou�it�m,
**      DLDisposeList ... zru�en� v�ech prvk� seznamu,
**      DLInsertFirst ... vlo�en� prvku na za��tek seznamu,
**      DLInsertLast .... vlo�en� prvku na konec seznamu,
**      DLFirst ......... nastaven� aktivity na prvn� prvek,
**      DLLast .......... nastaven� aktivity na posledn� prvek,
**      DLCopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DLCopyLast ...... vrac� hodnotu posledn�ho prvku,
**      DLDeleteFirst ... zru�� prvn� prvek seznamu,
**      DLDeleteLast .... zru�� posledn� prvek seznamu,
**      DLPostDelete .... ru�� prvek za aktivn�m prvkem,
**      DLPreDelete ..... ru�� prvek p�ed aktivn�m prvkem,
**      DLPostInsert .... vlo�� nov� prvek za aktivn� prvek seznamu,
**      DLPreInsert ..... vlo�� nov� prvek p�ed aktivn� prvek seznamu,
**      DLCopy .......... vrac� hodnotu aktivn�ho prvku,
**      DLActualize ..... p�ep��e obsah aktivn�ho prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal�� prvek seznamu,
**      DLPred .......... posune aktivitu na p�edchoz� prvek seznamu,
**      DLActive ........ zji��uje aktivitu seznamu.
**
** P�i implementaci jednotliv�ch funkc� nevolejte ��dnou z funkc�
** implementovan�ch v r�mci tohoto p��kladu, nen�-li u funkce
** explicitn� uvedeno n�co jin�ho.
**
** Nemus�te o�et�ovat situaci, kdy m�sto leg�ln�ho ukazatele na seznam
** p�ed� n�kdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodn� komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou��v� pojem procedura.
** Proto zde pou��v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozorn�n� na to, �e do�lo k chyb�.
** Tato funkce bude vol�na z n�kter�ch d�le implementovan�ch operac�.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* glob�ln� prom�nn� -- p��znak o�et�en� chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L p�ed jeho prvn�m pou�it�m (tzn. ��dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�d�t nad ji� inicializovan�m
** seznamem, a proto tuto mo�nost neo�et�ujte. V�dy p�edpokl�dejte,
** �e neinicializovan� prom�nn� maj� nedefinovanou hodnotu.
**/

    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;


 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLDisposeList (tDLList *L) {
/*
** Zru�� v�echny prvky seznamu L a uvede seznam do stavu, v jak�m
** se nach�zel po inicializaci. Ru�en� prvky seznamu budou korektn�
** uvoln�ny vol�n�m operace free.
**/

	tDLElemPtr pomUk = NULL;
    while(L->First != NULL)
    {
        pomUk = L->First;
        L->First = pomUk->rptr;
        free(pomUk);
    }

    L->Act = NULL;
    L->Last = NULL;

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vlo�� nov� prvek na za��tek seznamu L.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/

    //alokacia prvku
    struct tDLElem *tmpEl;
    tmpEl = malloc(sizeof(struct tDLElem));
    if (tmpEl == NULL)
    {
        DLError();
        return;
    }

    //naplnit data
    tmpEl->data=val;
    tmpEl->lptr=NULL;
    tmpEl->rptr=NULL;

    if(L->First == NULL)    //ak je prazdny zoznam
    {
        L->Last = tmpEl;
        L->First = tmpEl;
        return;
    }
    else                    //ak nieje prazdny
    {
        tmpEl->rptr = L->First;
        L->First->lptr = tmpEl;
        L->First = tmpEl;
    }

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vlo�� nov� prvek na konec seznamu L (symetrick� operace k DLInsertFirst).
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/

    //alokacia prvku
    struct tDLElem *tmpEl;
    tmpEl = malloc(sizeof(struct tDLElem));
    if (tmpEl == NULL)
    {
        DLError();
        return;
    }

    //naplnit data
    tmpEl->data=val;
    tmpEl->lptr=NULL;
    tmpEl->rptr=NULL;

    if(L->First == NULL)    //ak je prazdny zoznam
    {
        L->Last = tmpEl;
        L->First = tmpEl;
        return;
    }
    else                    //ak nieje prazdny
    {
        tmpEl->lptr = L->Last;
        L->Last->rptr = tmpEl;
        L->Last = tmpEl;
    }


// solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLFirst (tDLList *L) {
/*
** Nastav� aktivitu na prvn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/

    L->Act = L->First;

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLLast (tDLList *L) {
/*
** Nastav� aktivitu na posledn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/

    L->Act = L->Last;

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/

    if (L->First == NULL)
    {
        DLError();
        return;
    }
    else
    {
        *val = L->First->data;
    }

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu posledn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/

    if (L->First == NULL)
    {
        DLError();
        return;
    }
    else
    {
        *val = L->Last->data;
    }

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLDeleteFirst (tDLList *L) {
/*
** Zru�� prvn� prvek seznamu L. Pokud byl prvn� prvek aktivn�, aktivita
** se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
**/

    if(L->First == NULL)
        return;

    tDLElemPtr tmpPtr = L->First;

    if (L->First == L->Last)        //je v zozname jeden prvok?
    {
        L->Last = NULL;
    }

    if(L->Act == L->First)          //je aktivny prvy?
    {
        L->Act = NULL;
    }

    L->First = tmpPtr->rptr;

    if(L->First != NULL)
    {
        L->First->lptr = NULL;
    }

    free(tmpPtr);


 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLDeleteLast (tDLList *L) {
/*
** Zru�� posledn� prvek seznamu L. Pokud byl posledn� prvek aktivn�,
** aktivita seznamu se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
**/

    if(L->First == NULL)
        return;

    tDLElemPtr tmpPtr = L->Last;

    if (L->First == L->Last)        //je v zozname jeden prvok?
    {
        L->First = NULL;
    }

    if(L->Act == L->Last)          //je aktivny posledny?
    {
        L->Act = NULL;
    }

    L->Last = tmpPtr->lptr;

    if(L->Last != NULL)
    {
        L->Last->rptr = NULL;
    }

    free(tmpPtr);



 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLPostDelete (tDLList *L) {
/*
** Zru�� prvek seznamu L za aktivn�m prvkem.
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** posledn�m prvkem seznamu, nic se ned�je.
**/

    tDLElemPtr tmpPtr = NULL;

    if(L->Act == NULL || L->Last == L->Act)
    {
        return;
    }

    tmpPtr = L->Act->rptr;

    if(tmpPtr->rptr != NULL) // za rusenym je este prvok
    {
        L->Act->rptr = tmpPtr->rptr;
        tmpPtr->rptr->lptr = L->Act;
    }
    else                    //ruseny bol posledny
    {
        L->Act->rptr = NULL;
        L->Last = L->Act;
    }

    free(tmpPtr);

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLPreDelete (tDLList *L) {
/*
** Zru�� prvek p�ed aktivn�m prvkem seznamu L .
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** prvn�m prvkem seznamu, nic se ned�je.
**/

    tDLElemPtr tmpPtr = NULL;

    if(L->Act == NULL || L->First == L->Act)
    {
        return;
    }

    tmpPtr = L->Act->lptr;

    if(tmpPtr->lptr != NULL) // pred rusenym je este prvok
    {
        L->Act->lptr = tmpPtr->lptr;
        tmpPtr->lptr->rptr = L->Act;
    }
    else                    //ruseny bol posledny
    {
        L->Act->lptr = NULL;
        L->First = L->Act;
    }

    free(tmpPtr);


 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo�� prvek za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/

    if(L->Act == NULL)
        return;

    //alokacia
    struct tDLElem *tmpEl;
    tmpEl = malloc(sizeof(struct tDLElem));
    if (tmpEl == NULL)
    {
        DLError();
        return;
    }

    //naplnit data
    tmpEl->data=val;
    tmpEl->lptr=NULL;
    tmpEl->rptr=NULL;

    if(L->Act->rptr != NULL)    //za aktivnym nieco je
    {
        tmpEl->rptr = L->Act->rptr;
        L->Act->rptr->lptr = tmpEl;
    }
    else
    {
        L->Last = tmpEl;
    }

    //previazat novy a aktivny
    L->Act->rptr = tmpEl;
    tmpEl->lptr = L->Act;


 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vlo�� prvek p�ed aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/

    if(L->Act == NULL)
        return;

    //alokacia
    struct tDLElem *tmpEl;
    tmpEl = malloc(sizeof(struct tDLElem));
    if (tmpEl == NULL)
    {
        DLError();
        return;
    }

    //naplnit data
    tmpEl->data=val;
    tmpEl->lptr=NULL;
    tmpEl->rptr=NULL;

    if(L->Act->lptr != NULL)    //pred aktivnym nieco je
    {
        tmpEl->lptr = L->Act->lptr;
        L->Act->lptr->rptr = tmpEl;
    }
    else
    {
        L->First = tmpEl;
    }

    //previazat novy a aktivny
    L->Act->lptr = tmpEl;
    tmpEl->rptr = L->Act;

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLCopy (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, vol� funkci DLError ().
**/

    if(L->Act == NULL)
        DLError();
    else
        *val = L->Act->data;

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLActualize (tDLList *L, int val) {
/*
** P�ep��e obsah aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, ned�l� nic.
**/

    if(L->Act == NULL)
        return;
    else
        L->Act->data = val;

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na posledn�m prvku se seznam stane neaktivn�m.
**/

    if(L->Act == NULL)
        return;
    else
        L->Act = L->Act->rptr;


 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na p�edchoz� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na prvn�m prvku se seznam stane neaktivn�m.
**/

    if(L->Act == NULL)
        return;
    else
        L->Act = L->Act->lptr;


 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

int DLActive (tDLList *L) {
/*
** Je-li seznam aktivn�, vrac� true. V opa�n�m p��pad� vrac� false.
** Funkci implementujte jako jedin� p��kaz.
**/

 return (L->Act != NULL);

 //solved = FALSE;                   /* V p��pad� �e�en�, sma�te tento ��dek! */
}

/* Konec c206.c*/
