/****

PROJEKT IMP - rizeni vytahu
autor: Jan Jusko (xjusko00)
rok:   2015

BRNO UNIVERSITY OF TECHNOLOGY

vypracovanie obsahuje prebrane zdrojove kody z demo aplikacie rizeni LED pomoci casovace.

****/

/*******************************************************************************
   main: interrupt service routine (ISR) of timer A0 utilized for LED control
   Copyright (C) 2009 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Josef Strnadel <strnadel AT stud.fit.vutbr.cz>

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$


*******************************************************************************/

#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>
#include <stdio.h>
#include "string.h"

 char last_ch = 0;
 int current = 2;
 int previous = 0;
 int destination;

 int movinup = 0;
 int movindown = 0;
 int pohyb = 1;
 int stoppohyb = 0;
 int direction = 0;
 int otvordvere = 0;
 int zatvordvere = 0;
 int stopflag = 0;
 int reverse = 0;

 int druhy = 0;
 int treti = 0;
 int stvrty = 0;


/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
 * systemoveho helpu
*******************************************************************************/
void print_user_help(void)
{
  term_send_str_crlf("Rizeni vytahu. xjusko00");
}


/*******************************************************************************
 * Dekodovani a vykonani uzivatelskych prikazu
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return (CMD_UNKNOWN);
}


/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized()
{
  term_send_crlf();
  term_send_str_crlf("Aplikace bezi, vytah je ready na 1. poschodi.");
  LCD_init();                          // inicializuj LCD
  LCD_append_string("elevator ready");   // zobraz text na LCD
  LCD_send_cmd(15, 0);
}

// vypis podlazia, je potrebne koli -1 ktora nepatri do ascii tabulky
void printstory(int story){
  if(story == 1){
    term_send_char('-');
    term_send_char('1');
  }
  else
    term_send_char(story + '0' - 1);
}

// funkcia privola analyzuje polohu vytahu a privola ho na pozadovane poschodie
void privolanie(int poschodie){
  if(stopflag == 1)
    return;
  destination = poschodie;

  term_send_str("Vytah bol privolany z ");
  printstory(poschodie);
  term_send_str(". poschodia.");
  term_send_crlf();

  //stoj, nic sa nedeje iba otvor dvere
  if(current == destination){
    movindown = 0;
    movinup = 0;
    otvordvere = 1;
    destination = 0;
  }else if(current < destination){
    movinup = 1;
    pohyb = 2;
  }else if(current > destination){
    movindown = 1;
    pohyb = 2;
  }
}

// funkcia prida poziadavku na pohyb vytahu do fronty
void pridaj_poziadavku(int poschodie){
  if(stopflag == 1){
    return;
  }

  if(druhy == 0){
    term_send_str("Poziadavok 2. poradi z ");
    printstory(poschodie);
    term_send_str(" poschodia.");
    term_send_crlf();
    druhy = poschodie;
  }else if(treti == 0){
    term_send_str("Poziadavok 3. poradi z ");
    printstory(poschodie);
    term_send_str(" poschodia.");
    term_send_crlf();
    treti = poschodie;
  }else if(stvrty == 0){
    term_send_str("Poziadavok 4. poradi z ");
    printstory(poschodie);
    term_send_str(" poschodia.");
    term_send_crlf();
    stvrty = poschodie;
  }
}

// jednoducha pravdivostna funkcia vracia 1 ak su poziadavky vo fronte
int jepoziadavka(){
  if(druhy > 0 || treti > 0 || stvrty > 0)
    return 1;
  else
    return 0;
}

/*******************************************************************************
 * Obsluha klavesnice
*******************************************************************************/
int keyboard_idle()
{
  char ch;
  ch = key_decode(read_word_keyboard_4x4());
  if (ch != last_ch) // stav se zmnenil
  {
    last_ch = ch;
    if (ch != 0) // vylucime pusteni klavesy
    {
      // vytah bol privolany na -1 poschodie
      if(ch == '0' || ch == 'D'){
        //ak sa prave nehybe
        if(movinup != 1 && movindown != 1){
          privolanie(1);
        }
        // prave sa pohybuje sa nadol alebo nahor
        else if(movindown == 1 || movinup == 1){
          pridaj_poziadavku(1);
        }
      }
      else if(ch == '1' || ch == 'C'){
        if(movindown != 1 && movinup != 1){
          privolanie(2);
        }
        else if(movindown == 1 || movinup == 1){
          pridaj_poziadavku(2);
        }
      }
      else if(ch == '2' || ch == 'B'){
        if(movinup != 1 && movindown != 1){
          privolanie(3);
        }        
        else if(movindown == 1 || movinup == 1){
          pridaj_poziadavku(3);
        }
      }
      else if(ch == '3' || ch == 'A'){
        if(movinup != 1 && movindown != 1){
          privolanie(4);
        }
        else if(movindown == 1 || movinup == 1){
          pridaj_poziadavku(4);
        }
      }
      //stop
      else if(ch == '*'){
        term_send_str("STOP.");
        term_send_crlf();

        stopflag = 1;

        druhy = 0;
        treti = 0;
        stvrty = 0;

        if(current < destination) 
          direction = -1;
        else
          direction = 1;

        // OTOC TO
        if(direction == -1 && current < 2){
          direction = 1;
          current--;
          reverse = 1;
        }
        if(direction == 1 && current > 2){
          direction = -1;
          current++;
          reverse = 1;
        }

        if(movinup == 1){
          current++;
        }
        if(movindown == 1){
          current--;
        }

        if(movindown == 0 && movinup == 0)
          current--;

        movinup = 0;
        movindown = 0;
        destination = 0;
        stoppohyb = pohyb;
      }

      else if(ch == '#'){
        if(stoppohyb != 0){

          term_send_str("OPRAVENO.");
          term_send_crlf();
          stopflag = 0;
          previous = current;
          if(reverse == 1){
            pohyb = stoppohyb;
            reverse = 0;
          }
          else{
            pohyb = (7 - stoppohyb + 3);
          }
          if(direction == 1)
            movinup = 1;
          else if(direction == -1)
            movindown = 1;
          destination = 2;
          stoppohyb = 0;
          /*
          term_send_str(" up ");
          term_send_char(movinup + '0');
          term_send_str(" pohyb ");
          term_send_char(stoppohyb + '0');
          term_send_str(" down ");
          term_send_char(movindown + '0');
          term_send_str(" dest ");
          term_send_char(destination + '0');
          term_send_crlf();
          */
        }
      }
    }
  }
  return 0;
}

// funkcia analyzuje stav vytahu a vypisuje do terminalu a LCD aktualny stav / zmeny stavu
void getstatus(){
  if(movindown == 1 && movinup == 0){
    term_send_str("Vytah ide dole na ");
    printstory(destination);
    term_send_str(". poschodie. ");

    //vypisuj zaregistrovane poziadavky ak su
    if(jepoziadavka()){
      term_send_str("Dalsie poziadavky: ");

      printstory(druhy);
      
      if(treti > 0){
        term_send_char(',');
        printstory(treti);
      }

      if(stvrty > 0){
        term_send_char(',');
        printstory(stvrty);
      }
    }
    term_send_crlf();
    //lcd output
    switch(current){
      case 1:
        LCD_clear();
        LCD_write_string("-1 v");
        break;
      case 2:
        LCD_clear();
        LCD_write_string("1 v");
        break;
      case 3:
        LCD_clear();
        LCD_write_string("2 v");
        break;
      case 4:
        LCD_clear();
        LCD_write_string("3 v");
        break;
    }

    //presiel som jedno poschodie nadol
    if((pohyb % 7) == 0){
      pohyb = 1;
      current--;
      if(current == destination){
        term_send_str(" *vytah dorazil do ciela*");
        term_send_crlf();
        otvordvere = 1;
        destination = 0;
        movindown = 0;
      }
    }
  }

  else if(movinup == 1 && movindown == 0){
    // TERMINAL PART OUTPUT
      term_send_str("Vytah ide hore na ");
      printstory(destination);
      term_send_str(". poschodie. ");

      if(jepoziadavka()){
        term_send_str("Dalsie poziadavky: ");
        printstory(druhy);
        
        if(treti > 0){
          term_send_char(',');
          printstory(treti);
        }

        if(stvrty > 0){
          term_send_char(',');
          printstory(stvrty);
        }
      }
      term_send_crlf();

        // LCD PART
      switch(current){
      case 1:
        LCD_clear();
        LCD_write_string("-1 ^");
        break;
      case 2:
        LCD_clear();
        LCD_write_string("1 ^");
        break;
      case 3:
        LCD_clear();
        LCD_write_string("2 ^");
        break;
      case 4:
        LCD_clear();
        LCD_write_string("3 ^");
        break;
    }
     

    //presiel som poschodie nahoru
    if((pohyb % 7) == 0){
      pohyb = 1;
      current++;
      if(current == destination){
        term_send_str(" *vytah dorazil do ciela*");
        term_send_crlf();
        otvordvere = 1;
        destination = 0;
        movinup = 0;
      }
    }
  }
  else if(movindown == 0 && movinup == 0){
    //vytah stoji!!!
  }

  // kazde zavolanie getstatus 
  pohyb++;

  // rotuj pohyb
  if(pohyb%8 == 0){
    pohyb = 1;
  }
}

/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
  initialize_hardware();
  WDG_stop();                               // zastav watchdog
  keyboard_init();

  set_led_d5(1);

  CCTL0 = CCIE;                             // povol preruseni pro casovac (rezim vystupni komparace) 
  CCR0 = 0x4000;                            // nastav po kolika ticich (16384 = 0x4000, tj. za 1/2 s) ma dojit k preruseni
  TACTL = TASSEL_1 + MC_2;                  // ACLK (f_tiku = 32768 Hz = 0x8000 Hz), nepretrzity rezim

  while (1) {
     terminal_idle();                       // obsluha terminalu
     keyboard_idle();
  }
  return 0; 
}
/*******************************************************************************
 * Obsluha preruseni casovace timer A0
*******************************************************************************/
interrupt (TIMERA0_VECTOR) Timer_A (void)
{
  flip_led_d5();     //invertuj bit
  CCR0 += 0x8000;    // nastav po kolika ticich (16384 = 0x4000, tj. za 1/2 s) ma dojit k dalsimu preruseni

  // vypis aktualne poschodie len pri zmene
  if(previous != current && stopflag != 1){
    term_send_str(">Aktualne sme na: ");
    printstory(current);
    term_send_crlf();

    switch(current){
      case 1:
        LCD_clear();
        LCD_write_string("-1");
        break;
      case 2:
        LCD_clear();
        LCD_write_string("1");
        break;
      case 3:
        LCD_clear();
        LCD_write_string("2");
        break;
      case 4:
        LCD_clear();
        LCD_write_string("3");
        break;
    }
    previous = current;
  }
  // otvaranie + zatvaranie dveri (ludia nastupuju ked vytah dorazi do cieloveho poschodia)
  if(otvordvere >= 1){
    // vypis iba raz
    if(otvordvere == 1){
      term_send_str(" -otvaram dvere. ");
      term_send_crlf();
    }
    otvordvere++;
    if(otvordvere == 4){
      term_send_str(" -dvere otvorene. ");
      term_send_crlf();      
    }
    if(otvordvere == 9){
      term_send_str(" -zatvaram dvere. ");
      term_send_crlf();
    }
    if(otvordvere == 12){
      term_send_str(" -dvere zatvorene. ");
      term_send_crlf();
    } 
    if(otvordvere == 13){
      otvordvere = 0;
      // ak je dalsia poziadavka tak ju pories a posun poziadavky podla priority      
      if(jepoziadavka() == 1 && destination == 0){
        privolanie(druhy);
        druhy = treti;
        treti = stvrty;
        stvrty = 0;
      }
    }
  }
  else{
    getstatus();
  }
}
