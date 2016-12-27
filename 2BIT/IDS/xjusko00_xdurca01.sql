DROP TABLE sestra CASCADE CONSTRAINTS;
DROP TABLE oddelenie CASCADE CONSTRAINTS;
DROP TABLE pracuje_na CASCADE CONSTRAINTS;
DROP TABLE hospitalizacia CASCADE CONSTRAINTS;
DROP TABLE vysetrenie CASCADE CONSTRAINTS;
DROP TABLE pacient CASCADE CONSTRAINTS;
DROP TABLE platene CASCADE CONSTRAINTS;
DROP TABLE neplatene CASCADE CONSTRAINTS;
DROP TABLE lekar CASCADE CONSTRAINTS;
DROP TABLE liek CASCADE CONSTRAINTS;
DROP TABLE uziva CASCADE CONSTRAINTS;

CREATE TABLE sestra(
rodne_cislo NUMBER NOT NULL,
meno VARCHAR(20) NOT NULL,
priezvisko VARCHAR(25) NOT NULL,
pohlavie VARCHAR(10)NOT NULL,
datum_narodenia DATE NOT NULL ,
bydlisko VARCHAR(25) NOT NULL,
id_cislo_oddelenie NUMBER NOT NULL CHECK(id_cislo_oddelenie>0),
cislo_sestry NUMBER
);

CREATE TABLE oddelenie(
id_cislo  NUMBER,
nazov VARCHAR(20) NOT NULL,
lozkova_kapacita NUMBER NOT NULL,
primar VARCHAR(30) NOT NULL,
prednosta VARCHAR(30) NOT NULL
);
CREATE TABLE pracuje_na(
uvazok VARCHAR(20) NOT NULL  CHECK  (uvazok='DPP' or uvazok='DPÈ' or uvazok='Trvalý' or uvazok='Polovièný' ),
telefon NUMBER NOT NULL,
rodne_cislo_lekar NUMBER NOT NULL,
id_cislo_oddelenie NUMBER NOT NULL CHECK (id_cislo_oddelenie>0) 
);
CREATE TABLE hospitalizacia(
id_cislo NUMBER NOT NULL CHECK (id_cislo>0),
datum_prijatia DATE NOT NULL,
datum_prepustenia DATE NOT NULL,
rodne_cislo_pacient NUMBER NOT NULL,
id_cislo_oddelenie NUMBER NOT NULL
);
CREATE TABLE vysetrenie(
id_cislo  NUMBER NOT NULL CHECK (id_cislo>0),
datum DATE NOT NULL,
id_cislo_hospitalizacia NUMBER NOT NULL CHECK (id_cislo_hospitalizacia>0)
);
CREATE TABLE pacient(
rodne_cislo NUMBER NOT NULL,
meno VARCHAR(20) NOT NULL,
priezvisko VARCHAR(30) NOT NULL,
datum_narodenia DATE NOT NULL,
pohlavie VARCHAR(10) NOT NULL,
trvale_bydlisko VARCHAR(30) NOT NULL,
alergie VARCHAR(20) ,
rodne_cislo_lekar NUMBER NOT NULL
);
CREATE TABLE platene(
sposob_platby VARCHAR(10) NOT NULL , CHECK  (sposob_platby='hotovost' or sposob_platby='karta'),
id_cislo_vysetrenie NUMBER NOT NULL CHECK(id_cislo_vysetrenie>0)
);
CREATE TABLE neplatene(
cislo_poistovne NUMBER NOT NULL CHECK(LENGTH(cislo_poistovne)=8),
id_cislo_vysetrenie NUMBER NOT NULL CHECK(id_cislo_vysetrenie>0)
);
CREATE TABLE lekar(
rodne_cislo NUMBER NOT NULL,
meno VARCHAR(20) NOT NULL,
priezvisko VARCHAR(30) NOT NULL,
titul VARCHAR(20) NOT NULL,
datum_narodenia DATE NOT NULL,
pohlavie VARCHAR(10) NOT NULL,
trvale_bydlisko VARCHAR(30) NOT NULL,
specializacia VARCHAR(20) NOT NULL
);
CREATE TABLE uziva(
kedy VARCHAR(10) NOT NULL 	  CHECK  (kedy='ráno' or kedy='obed' or kedy='poobede' or kedy='veèer' ),
kolko_krat NUMBER NOT NULL CHECK (kolko_krat>0),
pocet_tabliet NUMBER NOT NULL,
rodne_cislo_pacient NUMBER NOT NULL,
cislo_sarze NUMBER NOT NULL
);
CREATE TABLE liek(
cislo_sarze NUMBER NOT NULL CHECK(LENGTH(cislo_sarze)=7),
nazov VARCHAR(20) NOT NULL,
druh VARCHAR(20) NOT NULL,
ucinna_latka VARCHAR(20) NOT NULL,
mnozstvo_ucinnej_latky NUMBER NOT NULL,
kontraindikacie VARCHAR(20) 
);

CREATE OR REPLACE TRIGGER check_birth_date
  BEFORE INSERT OR UPDATE ON sestra
  FOR EACH ROW
BEGIN
  IF( :new.datum_narodenia > sysdate )
  THEN
    RAISE_APPLICATION_ERROR( 
      -20001, 
      'Nurse not born yet' );
  END IF;
END;
/
CREATE OR REPLACE TRIGGER check_birth_date_pacient
  BEFORE INSERT OR UPDATE ON pacient
  FOR EACH ROW
BEGIN
  IF( :new.datum_narodenia > sysdate )
  THEN
    RAISE_APPLICATION_ERROR( 
      -20001, 
      'Patient not born yet' );
  END IF;
END;
/
CREATE OR REPLACE TRIGGER check_birth_date_doc
  BEFORE INSERT OR UPDATE ON lekar
  FOR EACH ROW
BEGIN
  IF( :new.datum_narodenia > sysdate )
  THEN
    RAISE_APPLICATION_ERROR( 
      -20001, 
      'Doc not born yet' );
  END IF;
END;
/
CREATE OR REPLACE TRIGGER check_date_vysetrenie
  BEFORE INSERT OR UPDATE ON vysetrenie
  FOR EACH ROW
BEGIN
  IF( :new.datum > sysdate )
  THEN
    RAISE_APPLICATION_ERROR( 
      -20001, 
      'Date of examination must be earlier than today' );
  END IF;
END;
/
CREATE OR REPLACE TRIGGER check_hospitalizacia
  BEFORE INSERT OR UPDATE ON hospitalizacia
  FOR EACH ROW
BEGIN
  IF( :new.datum_prepustenia-:new.datum_prijatia +1 <= 0 )
  THEN
    RAISE_APPLICATION_ERROR( 
      -20001, 
      'Date of Hospitalization must be earlier than today' );
  END IF;
END;
/
CREATE OR REPLACE TRIGGER  RODNE_CISLO_pacient_CHECK
  BEFORE INSERT OR UPDATE ON pacient
  FOR EACH ROW
  DECLARE
  rc pacient.rodne_cislo%TYPE;
  mesiac number(2);
  den number (2);
  rok number (2);
  datum date;
BEGIN
rc:=:new.rodne_cislo;
mesiac:=(rc/1000000)mod 100;
den:= (rc/10000) mod 100;
rok:=rc/100000000;
if(LENGTH(:new.rodne_cislo)=9) then
  begin
    datum:=den||'/'||mesiac||'/'||rok;
  exception
  when others then
  RAISE_APPLICATION_ERROR(-20001,'Wrong date');
  end;
end if;
if(length(:new.rodne_cislo)=10) then
    IF(mesiac<50) then 
    if(:new.pohlavie!='muž') then
    RAISE_APPLICATION_ERROR(-20001,'Wrong sex'); end if; end if;
  IF(mesiac>50) THEN
    IF(:new.pohlavie!='žena') then
    RAISE_APPLICATION_ERROR(-20001,'Wrong sex'); end if; 
    mesiac:=mesiac-50;
  end if; 
  begin
    datum:=den||'/'||mesiac||'/'||rok;
  exception
  when others then
  RAISE_APPLICATION_ERROR(-20001,'Wrong date');
  end;
end if;

 if(length(:new.rodne_cislo)!=10 and length(:new.rodne_cislo)!=9 ) then
  RAISE_APPLICATION_ERROR( 
      -20001, 
      'Doesnt match valid birth code' );
  end if;
END;
/
CREATE OR REPLACE TRIGGER  RODNE_CISLO_lekar_CHECK
  BEFORE INSERT OR UPDATE ON lekar
  FOR EACH ROW
  DECLARE
  rc lekar.rodne_cislo%TYPE;
  mesiac number(2);
  den number (2);
  rok number (2);
  datum date;
BEGIN
rc:=:new.rodne_cislo;
mesiac:=(rc/1000000)mod 100;
den:= (rc/10000) mod 100;
rok:=rc/100000000;
if(LENGTH(:new.rodne_cislo)=9) then
  begin
    datum:=den||'/'||mesiac||'/'||rok;
  exception
  when others then
  RAISE_APPLICATION_ERROR(-20001,'Wrong date');
  end;
end if;


if(length(:new.rodne_cislo)=10) then
    IF(mesiac<50) then 
    if(:new.pohlavie!='muž') then
    RAISE_APPLICATION_ERROR(-20001,'Wrong sex'); end if; end if;
  IF(mesiac>50) THEN
    IF(:new.pohlavie!='žena') then
    RAISE_APPLICATION_ERROR(-20001,'Wrong sex'); end if; 
    mesiac:=mesiac-50;
  end if; 
  begin
    datum:=den||'/'||mesiac||'/'||rok;
  exception
  when others then
  RAISE_APPLICATION_ERROR(-20001,'Wrong date');
  end;
end if;

 if(length(:new.rodne_cislo)!=10 and length(:new.rodne_cislo)!=9 ) then
  RAISE_APPLICATION_ERROR( 
      -20001, 
      'Doesnt match valid birth code' );
  end if;
END;
/
CREATE OR REPLACE TRIGGER  RODNE_CISLO_sestra
  BEFORE INSERT OR UPDATE ON sestra
  FOR EACH ROW
  DECLARE
  rc sestra.rodne_cislo%TYPE;
  mesiac number(2);
  den number (2);
  rok number (2);
  datum date;
BEGIN
rc:=:new.rodne_cislo;
mesiac:=(rc/1000000)mod 100;
den:= (rc/10000) mod 100;
rok:=rc/100000000;
if(LENGTH(:new.rodne_cislo)=9) then
  begin
    datum:=den||'/'||mesiac||'/'||rok;
  exception
  when others then
  RAISE_APPLICATION_ERROR(-20001,'Wrong date');
  end;
end if;
if(LENGTH(:new.rodne_cislo)=10) then
IF(mesiac<50) then 
    if(:new.pohlavie!='muž') then
    RAISE_APPLICATION_ERROR(-20001,'Wrong sex'); end if; end if;

IF(mesiac>50) THEN
    IF(:new.pohlavie!='žena') then
    RAISE_APPLICATION_ERROR(-20001,'Wrong sex'); end if; 
    mesiac:=mesiac-50;
  end if; 
begin
    datum:=den||'/'||mesiac||'/'||rok;
  exception
  when others then
  RAISE_APPLICATION_ERROR(-20001,'Wrong date');
  end;
end if;
 if(length(:new.rodne_cislo)!=10 and length(:new.rodne_cislo)!=9 ) then
  RAISE_APPLICATION_ERROR( 
      -20001, 
      'Doesnt match valid birth code' );
  end if;
END;
/
ALTER TABLE sestra ADD CONSTRAINT PK_sestra PRIMARY KEY (rodne_cislo);
ALTER TABLE oddelenie ADD CONSTRAINT PK_oddelenie PRIMARY KEY (id_cislo);
ALTER TABLE hospitalizacia ADD CONSTRAINT PK_hospitalizacia PRIMARY KEY (id_cislo);
ALTER TABLE vysetrenie ADD CONSTRAINT PK_vysetrenie PRIMARY KEY (id_cislo);
ALTER TABLE pacient ADD CONSTRAINT PK_pacient PRIMARY KEY (rodne_cislo);
ALTER TABLE lekar ADD CONSTRAINT PK_lekar PRIMARY KEY (rodne_cislo);
ALTER TABLE liek ADD CONSTRAINT PK_liek PRIMARY KEY (cislo_sarze);


ALTER TABLE  sestra ADD CONSTRAINT FK_oddelenie FOREIGN KEY(id_cislo_oddelenie) REFERENCES oddelenie ;
ALTER TABLE  pacient ADD CONSTRAINT FK_lekar FOREIGN KEY(rodne_cislo_lekar) REFERENCES lekar ;
ALTER TABLE  vysetrenie ADD CONSTRAINT FK_hospitalizacia FOREIGN KEY(id_cislo_hospitalizacia) REFERENCES hospitalizacia;

ALTER TABLE  hospitalizacia ADD CONSTRAINT FK_pacient FOREIGN KEY(rodne_cislo_pacient) REFERENCES pacient ;
ALTER TABLE  hospitalizacia ADD CONSTRAINT FK_oddelenie_hospitalizacia FOREIGN KEY(id_cislo_oddelenie) REFERENCES  oddelenie ;


ALTER TABLE  platene ADD CONSTRAINT FK_vysetrenie FOREIGN KEY(id_cislo_vysetrenie) REFERENCES vysetrenie;
ALTER TABLE  neplatene ADD CONSTRAINT FK_vysetrenie_neplatene FOREIGN KEY(id_cislo_vysetrenie) REFERENCES vysetrenie;
ALTER TABLE  pracuje_na ADD CONSTRAINT FK_lekar_pracujena FOREIGN KEY(rodne_cislo_lekar) REFERENCES lekar;
ALTER TABLE  pracuje_na ADD CONSTRAINT FK_oddelenie_pracujena FOREIGN KEY(id_cislo_oddelenie) REFERENCES oddelenie;
ALTER TABLE  uziva ADD CONSTRAINT FK_pacient_uziva FOREIGN KEY(rodne_cislo_pacient) REFERENCES pacient;
ALTER TABLE  uziva ADD CONSTRAINT FK_liek FOREIGN KEY(cislo_sarze) REFERENCES liek;

------ TRIGGER c.2 AUTOINKREMENT HODNOTY cislo_sestry V TABULKE sestra KDYZ JE NULL -------
DROP SEQUENCE seq;
CREATE SEQUENCE seq 
  start with 10000 
  increment by 1;
 
CREATE OR REPLACE TRIGGER trig
BEFORE INSERT OR UPDATE ON sestra
FOR EACH ROW
WHEN (new.cislo_sestry IS NULL)
BEGIN
  SELECT seq.NEXTVAL
  INTO   :new.cislo_sestry
  FROM   dual;
END;
/


INSERT INTO oddelenie (id_cislo, nazov, lozkova_kapacita, primar, prednosta) VALUES (10, 'Interní', 20, 'Michal Mucha', 'Ivan Veselý');
INSERT INTO oddelenie (id_cislo, nazov, lozkova_kapacita, primar, prednosta) VALUES (11, 'Oèní', 3, 'Jan Ostrovid', 'Miloš Okulár');
INSERT INTO oddelenie (id_cislo, nazov, lozkova_kapacita, primar, prednosta) VALUES (12, 'Kardiovaskulární', 12, 'Petr Srdeèný', 'Lucia Silná');

--INSERT INTO sestra (rodne_cislo, meno, priezvisko,pohlavie, datum_narodenia, bydlisko, id_cislo_oddelenie,cislo_sestry) VALUES (8356020420, 'Jolana', 'Mrzká','žena', '06/02/1983', 'Kounicova 54, Brno', 10, 5);
--INSERT INTO sestra (rodne_cislo, meno, priezvisko,pohlavie, datum_narodenia, bydlisko, id_cislo_oddelenie) VALUES (7953120428, 'Iveta', 'Èokoládová','žena', '03/12/1979', 'Purkyòova 2, Brno', 10);
INSERT INTO sestra (rodne_cislo, meno, priezvisko, pohlavie,datum_narodenia, bydlisko, id_cislo_oddelenie) VALUES (8161270425, 'Eliška', 'Rychlá','žena', '27/01/1981', 'Hlavná 125, Olomouc', 11);
INSERT INTO sestra (rodne_cislo, meno, priezvisko,pohlavie, datum_narodenia, bydlisko, id_cislo_oddelenie) VALUES (6957220138, 'Eliška', 'Rychlá','žena', '22/07/1969', 'Nerudova 17, Bruntál', 12);
INSERT INTO sestra (rodne_cislo, meno, priezvisko,pohlavie, datum_narodenia, bydlisko, id_cislo_oddelenie) VALUES (7455140055, 'Alena', 'Nováková','žena', '14/05/1974', 'Kolejní 2, Praha', 12);


INSERT INTO lekar(rodne_cislo, meno, priezvisko, titul, datum_narodenia, pohlavie, trvale_bydlisko, specializacia) VALUES (9406150017, 'Michal', 'Mucha', 'MuDr', '15/06/1994', 'muž', 'Mlyny 12, Kladno', 'Endokrynológia');
INSERT INTO lekar(rodne_cislo, meno, priezvisko, titul, datum_narodenia, pohlavie, trvale_bydlisko, specializacia) VALUES (8001100206, 'Ivan', 'Veselý', 'MuDr', '01/10/1980', 'muž', 'Pánska 1202, Plzeò', 'Chirurgia');
INSERT INTO lekar(rodne_cislo, meno, priezvisko, titul, datum_narodenia, pohlavie, trvale_bydlisko, specializacia) VALUES (6805190546, 'Jan', 'Ostrovid', 'MuDr', '19/05/1968', 'muž', 'Na kameòi 45, Plzeò', 'ORL');
INSERT INTO lekar(rodne_cislo, meno, priezvisko, titul, datum_narodenia, pohlavie, trvale_bydlisko, specializacia) VALUES (6407300526, 'Miloš', 'Okulár', 'MuDr', '30/07/1964', 'muž', 'Pod mostem 7, Brno', 'Oèní');
INSERT INTO lekar(rodne_cislo, meno, priezvisko, titul, datum_narodenia, pohlavie, trvale_bydlisko, specializacia) VALUES (8510220851, 'Petr', 'Srdeèný', 'MuDr', '22/10/1985', 'muž', 'Klusáèkova 46, Brno', 'Kardiológia');
INSERT INTO lekar(rodne_cislo, meno, priezvisko, titul, datum_narodenia, pohlavie, trvale_bydlisko, specializacia) VALUES (8955050203, 'Lucia', 'Silná', 'MuDr', '05/05/1989', 'žena', 'Kounicova 400, Brno', 'Kardiochirurgia');


INSERT INTO pracuje_na (uvazok, telefon, rodne_cislo_lekar, id_cislo_oddelenie) VALUES ('DPP', 0949191735, 9406150017, 10);
INSERT INTO pracuje_na (uvazok, telefon, rodne_cislo_lekar, id_cislo_oddelenie) VALUES ('Trvalý', 0907156221, 8001100206, 10);
INSERT INTO pracuje_na (uvazok, telefon, rodne_cislo_lekar, id_cislo_oddelenie) VALUES ('Trvalý', 0908236548, 6805190546, 11);
INSERT INTO pracuje_na (uvazok, telefon, rodne_cislo_lekar, id_cislo_oddelenie) VALUES ('Polovièný', 0905622548, 6407300526, 11);
INSERT INTO pracuje_na (uvazok, telefon, rodne_cislo_lekar, id_cislo_oddelenie) VALUES ('DPÈ', 0905622545, 8510220851, 12);
INSERT INTO pracuje_na (uvazok, telefon, rodne_cislo_lekar, id_cislo_oddelenie) VALUES ('DPP', 0908462545, 8955050203, 12);

INSERT INTO pacient(rodne_cislo, meno, priezvisko, datum_narodenia, pohlavie, trvale_bydlisko, alergie, rodne_cislo_lekar) VALUES (9656120507, 'Simona', 'Palláriková', '12/06/1996', 'žena', 'Víïenská 21, Brno', 'arašidy, mlieko', 8955050203);
INSERT INTO pacient(rodne_cislo, meno, priezvisko, datum_narodenia, pohlavie, trvale_bydlisko, alergie, rodne_cislo_lekar) VALUES (8758191959, 'Katarína', 'Sabolová', '19/08/1987', 'žena', 'Kounicova 412, Brno-Cejl', 'maso', 6407300526);
INSERT INTO pacient(rodne_cislo, meno, priezvisko, datum_narodenia, pohlavie, trvale_bydlisko, alergie, rodne_cislo_lekar) VALUES (9405094424, 'Adam', 'Dudek', '09/05/1994', 'muž', 'Kounicova 412, Polièka', '', 8510220851);
INSERT INTO pacient(rodne_cislo, meno, priezvisko, datum_narodenia, pohlavie, trvale_bydlisko, alergie, rodne_cislo_lekar) VALUES (9609269945, 'Adam', 'Pokorný', '26/09/1996', 'muž', 'Pivní 69, Polièka', 'pivo', 8510220851);
INSERT INTO pacient(rodne_cislo, meno, priezvisko, datum_narodenia, pohlavie, trvale_bydlisko, alergie, rodne_cislo_lekar) VALUES (9204127647, 'Ivan', 'Drzý', '12/04/1992', 'muž', 'Tovární 45, Kladno', 'lepok', 6805190546);
INSERT INTO pacient(rodne_cislo, meno, priezvisko, datum_narodenia, pohlavie, trvale_bydlisko, alergie, rodne_cislo_lekar) VALUES (8805122755, 'Norbert', 'Novák', '12/05/1988', 'muž', 'Sportovní 4, Košice', 'med', 8001100206);
INSERT INTO pacient(rodne_cislo, meno, priezvisko, datum_narodenia, pohlavie, trvale_bydlisko, alergie, rodne_cislo_lekar) VALUES (8706127881, 'Jordán', 'Slaný', '12/06/1987', 'muž', 'Sladkého 81, Bratislava', 'vodka', 8510220851);

INSERT INTO hospitalizacia (id_cislo, datum_prijatia, datum_prepustenia, rodne_cislo_pacient, id_cislo_oddelenie) VALUES (36,'15/02/2015' ,'20/02/2015' ,9656120507,10);
INSERT INTO hospitalizacia (id_cislo, datum_prijatia, datum_prepustenia, rodne_cislo_pacient, id_cislo_oddelenie) VALUES (23,'12/12/2014' ,'13/12/2014' ,8758191959,10);
INSERT INTO hospitalizacia (id_cislo, datum_prijatia, datum_prepustenia, rodne_cislo_pacient, id_cislo_oddelenie) VALUES (42,'30/05/2014' ,'02/06/2014' ,9405094424,12);
INSERT INTO hospitalizacia (id_cislo, datum_prijatia, datum_prepustenia, rodne_cislo_pacient, id_cislo_oddelenie) VALUES (14,'01/05/2014' ,'03/05/2014' ,9609269945,12);
INSERT INTO hospitalizacia (id_cislo, datum_prijatia, datum_prepustenia, rodne_cislo_pacient, id_cislo_oddelenie) VALUES (28,'25/03/2015' ,'29/03/2015' ,9204127647,11);
INSERT INTO hospitalizacia (id_cislo, datum_prijatia, datum_prepustenia, rodne_cislo_pacient, id_cislo_oddelenie) VALUES (43,'13/01/2015' ,'15/02/2015' ,8805122755,12);
INSERT INTO hospitalizacia (id_cislo, datum_prijatia, datum_prepustenia, rodne_cislo_pacient, id_cislo_oddelenie) VALUES (49,'23/02/2015' ,'28/02/2015' ,8706127881,11);

INSERT INTO vysetrenie(id_cislo, datum, id_cislo_hospitalizacia) VALUES (464,'16/02/2015',36);
INSERT INTO vysetrenie(id_cislo, datum, id_cislo_hospitalizacia) VALUES (522,'13/12/2014',23);
INSERT INTO vysetrenie(id_cislo, datum, id_cislo_hospitalizacia) VALUES (462,'31/05/2014',42);
INSERT INTO vysetrenie(id_cislo, datum, id_cislo_hospitalizacia) VALUES (995,'02/05/2014',14);
INSERT INTO vysetrenie(id_cislo, datum, id_cislo_hospitalizacia) VALUES (505,'26/03/2015',28);
INSERT INTO vysetrenie(id_cislo, datum, id_cislo_hospitalizacia) VALUES (463,'26/03/2015',28);
INSERT INTO vysetrenie(id_cislo, datum, id_cislo_hospitalizacia) VALUES (465,'26/03/2015',28);

INSERT INTO liek(cislo_sarze, nazov, druh, ucinna_latka, mnozstvo_ucinnej_latky, kontraindikacie) VALUES (1221325, 'paralen', 'tablety', 'paracetamolum', 500, '');
INSERT INTO liek(cislo_sarze, nazov, druh, ucinna_latka, mnozstvo_ucinnej_latky, kontraindikacie) VALUES (8452321, 'ibalgin', 'tablety', 'ibubrofen', 250, 'trufen');
INSERT INTO liek(cislo_sarze, nazov, druh, ucinna_latka, mnozstvo_ucinnej_latky, kontraindikacie) VALUES (6546842, 'panadol forte', 'roztok', 'panadolium', 5000, 'cuprumium');
INSERT INTO liek(cislo_sarze, nazov, druh, ucinna_latka, mnozstvo_ucinnej_latky, kontraindikacie) VALUES (6842225, 'flediol', 'kapsule', 'flediolum', 2, 'paralen, ibalgin');


INSERT INTO uziva(kedy, kolko_krat, pocet_tabliet, rodne_cislo_pacient, cislo_sarze) VALUES ('ráno', 3, 2, 9656120507, 1221325);
INSERT INTO uziva(kedy, kolko_krat, pocet_tabliet, rodne_cislo_pacient, cislo_sarze) VALUES ('veèer', 5, 4, 9656120507, 8452321);
INSERT INTO uziva(kedy, kolko_krat, pocet_tabliet, rodne_cislo_pacient, cislo_sarze) VALUES ('obed', 12, 2, 9405094424, 6842225);
INSERT INTO uziva(kedy, kolko_krat, pocet_tabliet, rodne_cislo_pacient, cislo_sarze) VALUES ('veèer', 1, 1, 9204127647, 6546842);
INSERT INTO uziva(kedy, kolko_krat, pocet_tabliet, rodne_cislo_pacient, cislo_sarze) VALUES ('ráno', 6, 3, 8805122755, 1221325);
INSERT INTO uziva(kedy, kolko_krat, pocet_tabliet, rodne_cislo_pacient, cislo_sarze) VALUES ('obed', 9, 1, 8706127881, 1221325);

INSERT INTO platene (sposob_platby, id_cislo_vysetrenie) VALUES ('hotovost', 464);
INSERT INTO platene (sposob_platby, id_cislo_vysetrenie) VALUES ('karta', 522);
INSERT INTO platene (sposob_platby, id_cislo_vysetrenie) VALUES ('hotovost', 462);
INSERT INTO platene (sposob_platby, id_cislo_vysetrenie) VALUES ('karta', 463);
INSERT INTO platene (sposob_platby, id_cislo_vysetrenie) VALUES ('karta', 465);

INSERT INTO neplatene(cislo_poistovne, id_cislo_vysetrenie) VALUES (41456879, 995);
INSERT INTO neplatene(cislo_poistovne, id_cislo_vysetrenie) VALUES (43565472, 505);

----------------------------------------------- 3. CAST PROJEKTU PRIKAZY SELECT -----------------------------------------------


-- 1. SPOJENI 2 TABULEK
-- zobraz sestry (meno, priezvisko) ktore pracuju na oddeleni Interní
SELECT s.meno, s.priezvisko
FROM sestra s , oddelenie o
WHERE s.ID_CISLO_ODDELENIE = o.ID_CISLO AND o.NAZOV = 'Interní';

-- 2. SPOJENI 2 TABULEK
-- zobraz pacientov, ktorych osetruje lekar, ktory sa narodil pred rokom 1980
SELECT p.meno, p.priezvisko
FROM pacient p, lekar l
WHERE p.rodne_cislo_lekar = l.rodne_cislo AND l.datum_narodenia < '1/1/1980';

-- SPOJENI TRI TABULEK
-- zobraz pacientov, ktory uzivaju paralen viac ako 5x denne
SELECT p.meno, p.priezvisko
FROM pacient p, uziva u, liek l
WHERE u.cislo_sarze = l.cislo_sarze AND u.rodne_cislo_pacient = p.rodne_cislo AND u.kolko_krat > 5 AND l.nazov = 'paralen';

-- POUZITI PREDIKATU IN
-- zobraz pocet pacientov, ktory uzivaju liek vo forme tabliet
select COUNT(*) pocet_pacientov
from pacient p
where p.rodne_cislo in (select rodne_cislo_pacient from uziva  where uziva.cislo_sarze in (select cislo_sarze from liek where liek.druh='tablety'));

-- POUZITI PREDIKATU EXISTS
-- zobraz pacientov, ktory behom hospitalizacie podstupili vysetrenie
select DISTINCT p.meno, p.priezvisko, p.RODNE_CISLO
from pacient p, vysetrenie v, hospitalizacia h
where h.ID_CISLO = v.ID_CISLO_HOSPITALIZACIA AND h.RODNE_CISLO_PACIENT = p.RODNE_CISLO AND EXISTS (SELECT id_cislo_hospitalizacia
                                                                                                    FROM vysetrenie
                                                                                                    WHERE h.ID_CISLO = v.ID_CISLO_HOSPITALIZACIA);
-- 1. POUZITI PREDIKATU GROUP BY                                                                                                    
--Vypis nazvy oddeleni a pocet hospitalizacii,ktore maju datum prepustenia do konca roku 2014
select o.nazov,COUNT(*) pocet_hospitalizacii
from oddelenie o,hospitalizacia h
where o.id_cislo=h.id_cislo_oddelenie and h.datum_prepustenia<='31/12/2014'
group by o.nazov;

-- 2. POUZITI PREDIKATU GROUP BY
-- vypis vsetky datumy a pocty, kedy za vysetrenie platil pacient
select v.datum,count(*)
from vysetrenie v , platene p
where p.id_cislo_vysetrenie=v.id_cislo
group by v.datum;


-- priklad pre trigger c.2 implementovany vyssie:
INSERT INTO sestra (rodne_cislo, meno, priezvisko,pohlavie, datum_narodenia, bydlisko, id_cislo_oddelenie,cislo_sestry) VALUES (8356020420, 'Jolana', 'Mrzká','žena', '06/02/1983', 'Kounicova 54, Brno', 10, 5);
INSERT INTO sestra (rodne_cislo, meno, priezvisko,pohlavie, datum_narodenia, bydlisko, id_cislo_oddelenie) VALUES (7953120428, 'Iveta', 'Èokoládová','žena', '03/12/1979', 'Purkyòova 2, Brno', 10);

--select * from sestra;


--XJUSKO00_XDURCA01
------------------------------4. cast projektu -------------------------------------------


--Predvedenie vysie implementovanych triggerov (Trigger na overenie rodneho cisla) 
/
--1.Trigger
--Triger na rodne cislo 
--snazime sa pridat do tabulky lekarku s rodnym cislom muža
INSERT INTO lekar(rodne_cislo, meno, priezvisko, titul, datum_narodenia, pohlavie, trvale_bydlisko, specializacia) VALUES (9107100013, 'Lucia', 'Slabá', 'MuDr', '10/07/1991', 'žena', 'Kounicova 400, Brno', 'Kardiochirurgia');
/
DROP PROCEDURE POCET_LOZOK;
/
------ TRIGGER c.2 AUTOINKREMENT HODNOTY cislo_sestry V TABULKE sestra KDYZ JE NULL -------
--nazov triggeru je trig
----Vsetky sestry co mali v stlpci NULL sa pridala hodnota +auto increment
---- Tie co mali nastavenu zostala im.
SELECT * from sestra;
------------------------------------
-------------------------------PROCEDURY------------------------------------------------------
--1.Procedura
-- zmeni lozkovu kapacitu na oddeleni....  implementacia + predvedenie nizsie 
create or replace procedure pocet_lozok(nazov VARCHAR,pocet NUMBER) as
begin
  Update oddelenie
  SET lozkova_kapacita=pocet_lozok.pocet
  where oddelenie.nazov=pocet_lozok.nazov;
end;
/
execute pocet_lozok('Interní',500);
select * from oddelenie;

--2.Procedura 
--vyuzitie kurzoru
---Vypise aky uvazok ma dany lekar na danom oddeleni
---Procedura prijima rodne cislo lekara a id oddelenia
SET SERVEROUTPUT ON
create or replace procedure uvazok (lekar number,oddelenie number)
is
cursor c1 is select * from pracuje_na;
rec1 c1%rowtype;
begin
open c1;
loop
fetch c1 into rec1;
exit when c1%notfound;
  if rec1.rodne_cislo_lekar=lekar and rec1.id_cislo_oddelenie=oddelenie then
  dbms_output.put_line(rec1.uvazok);
  end if;
end loop;
close c1;
  end;
  /
--select uvazok from pracuje_na where rodne_cislo_lekar=9406150017 and id_cislo_oddelenie=10;  
execute uvazok(9406150017,10);


--------------------GRANT PRE DRUHEHO CLENA TYMU-------------------------------------

grant ALL on oddelenie to XDURCA01;
grant ALL on pracuje_na to xdurca01;
grant all on sestra to xdurca01;
grant ALL on hospitalizacia to xdurca01;
grant ALL on vysetrenie to xdurca01;
grant ALL on pacient to xdurca01;
grant ALL on platene to xdurca01;
grant ALL on neplatene to xdurca01;
grant ALL on lekar to xdurca01;
grant ALL on uziva to xdurca01;
grant ALL on liek to xdurca01;

-----------------------
--EXPLAIN PLAN
-----------------------
----Vypis nam kolko pacientov ma kazdy lekar 
/
DROP INDEX INDEXIDS;
DROP INDEX INDEXIDSS;
/
------explain plan bez vytvorenia indexov 
EXPLAIN PLAN FOR SELECT lekar.priezvisko,count(*) FROM PACIENT INNER JOIN LEKAR ON PACIENT.RODNE_CISLO_LEKAR=LEKAR.RODNE_CISLO group by lekar.rodne_cislo,lekar.priezvisko;
SELECT PLAN_TABLE_OUTPUT FROM TABLE(dbms_xplan.display());
---------------------
create  INDEX INDEXIDS ON PACIENT (RODNE_CISLO_LEKAR);
create  INDEX INDEXIDSS ON LEKAR (PRIEZVISKO,RODNE_CISLO);

------explain plan s vytvorenymi indexami
------Pridanim indexov vidime vyrazne nizsie vytazenie CPU a nemusime prehladavat
EXPLAIN PLAN FOR SELECT lekar.priezvisko,count(*) FROM PACIENT INNER JOIN LEKAR ON PACIENT.RODNE_CISLO_LEKAR=LEKAR.RODNE_CISLO group by lekar.rodne_cislo,lekar.priezvisko;
SELECT PLAN_TABLE_OUTPUT FROM TABLE(dbms_xplan.display());
SELECT lekar.priezvisko,count(*) FROM PACIENT INNER JOIN LEKAR ON PACIENT.RODNE_CISLO_LEKAR=LEKAR.RODNE_CISLO group by lekar.rodne_cislo,lekar.priezvisko;

---Optimalizacia pri zlozitejsom SELECT

-----------------------------------------------
----Optimalizacia pouzitie tych istych + chceme vediet specializaciu a kde pracuje lekar 
--Neoptimalizovane pouzitie
--Priezvisko lekar, pocet pacientov a kde pracuje
--Uloha  
----Spustit bez indexov a nizsie s nimi 
---Table access full a podobne ktore vytazuju  CPU
SELECT lekar.priezvisko, oddelenie.nazov,COUNT(*)  FROM PACIENT, LEKAR, ODDELENIE,PRACUJE_NA WHERE PACIENT.RODNE_CISLO_LEKAR=LEKAR.RODNE_CISLO AND LEKAR.RODNE_CISLO=PRACUJE_NA.RODNE_CISLO_LEKAR AND ODDELENIE.ID_CISLO=PRACUJE_NA.ID_CISLO_ODDELENIE
GROUP BY LEKAR.RODNE_CISLO,LEKAR.PRIEZVISKO,ODDELENIE.NAZOV;
-----
DROP INDEX INDEXIDSX;
DROP INDEX INDEXIDSXX;

EXPLAIN PLAN FOR
SELECT  lekar.priezvisko, oddelenie.nazov,COUNT(*)  FROM PACIENT, LEKAR, ODDELENIE,PRACUJE_NA WHERE PACIENT.RODNE_CISLO_LEKAR=LEKAR.RODNE_CISLO AND LEKAR.RODNE_CISLO=PRACUJE_NA.RODNE_CISLO_LEKAR AND ODDELENIE.ID_CISLO=PRACUJE_NA.ID_CISLO_ODDELENIE
GROUP BY LEKAR.RODNE_CISLO,LEKAR.PRIEZVISKO,ODDELENIE.NAZOV;
SELECT PLAN_TABLE_OUTPUT FROM TABLE(dbms_xplan.display());
/

-----Optimalizovane
-----Pridane dalsie indexy
----Zlepsenie a zrychlenie vyhladavania
CREATE INDEX INDEXIDSX ON PRACUJE_NA(RODNE_CISLO_LEKAR,ID_CISLO_ODDELENIE); 
CREATE INDEX INDEXIDSXX ON ODDELENIE(ID_CISLO,NAZOV);

EXPLAIN PLAN FOR
SELECT/*+INDEX( ODDELENIE INDEXIDSXX) INDEX( LEKAR INDEXIDSS)*/ lekar.priezvisko, oddelenie.nazov,COUNT(*)  FROM PACIENT, LEKAR, ODDELENIE,PRACUJE_NA WHERE PACIENT.RODNE_CISLO_LEKAR=LEKAR.RODNE_CISLO AND LEKAR.RODNE_CISLO=PRACUJE_NA.RODNE_CISLO_LEKAR AND ODDELENIE.ID_CISLO=PRACUJE_NA.ID_CISLO_ODDELENIE
GROUP BY LEKAR.RODNE_CISLO,LEKAR.PRIEZVISKO,ODDELENIE.NAZOV;
SELECT PLAN_TABLE_OUTPUT FROM TABLE(dbms_xplan.display());
/

-------------------------------------TATO CAST SA SPUSTA U DRUHEHO CLENA. VYTVORENIE MATERIALIZOVANEHO POHLADU-------------------------------------
-----XDURCA01
--TEST ze dane tabulky su pristupne pre uzivatela XDURCA01
select * from  xjusko00.sestra  ;
select * from  xjusko00.oddelenie  ;
select * from  xjusko00.pracuje_na;
select * from  xjusko00.hospitalizacia;
select * from  xjusko00.vysetrenie  ;
select * from  xjusko00.pacient  ;
select * from  xjusko00.platene  ;
select * from  xjusko00.neplatene  ;
select * from  xjusko00.lekar  ;
select * from  xjusko00.liek  ;
select * from  xjusko00.uziva  ;

---Vytvorenie materializovaneho pohladu,ktory patri XDURCA01 a pouziva tabulky XJUSKO00
drop materialized view pacienthosp;

--Materializovany pohlad na vypis pacientov ich hospitalizacia a poctu vysetreni
create materialized view pacienthosp
nologging
cache
build immediate
enable query rewrite
as
select xjusko00.pacient.rodne_cislo,xjusko00.pacient.Priezvisko,xjusko00.hospitalizacia.datum_prijatia,xjusko00.hospitalizacia.datum_prepustenia,count(*) pocet_vysetreni
from xjusko00.pacient, xjusko00.hospitalizacia, xjusko00.vysetrenie where xjusko00.pacient.rodne_cislo=xjusko00.hospitalizacia.rodne_cislo_pacient and  xjusko00.vysetrenie.id_cislo_hospitalizacia=xjusko00.hospitalizacia.id_cislo
group by xjusko00.pacient.rodne_cislo,xjusko00.pacient.Priezvisko,xjusko00.hospitalizacia.datum_prijatia,xjusko00.hospitalizacia.datum_prepustenia
;
grant all on pacienthosp to xjusko00;
/
--select xjusko00.pacient.rodne_cislo,xjusko00.pacient.Priezvisko,xjusko00.hospitalizacia.datum_prijatia,xjusko00.hospitalizacia.datum_prepustenia,count(*) pocet_vysetreni
--from xjusko00.pacient, xjusko00.hospitalizacia, xjusko00.vysetrenie where xjusko00.pacient.rodne_cislo=xjusko00.hospitalizacia.rodne_cislo_pacient and  xjusko00.vysetrenie.id_cislo_hospitalizacia=xjusko00.hospitalizacia.id_cislo
--group by xjusko00.pacient.rodne_cislo,xjusko00.pacient.Priezvisko,xjusko00.hospitalizacia.datum_prijatia,xjusko00.hospitalizacia.datum_prepustenia
--;
------------------------------


-------------------------------SPUTENIE U PRVEHO CLENA XJUSKO00-----------------------------
select Priezvisko,rodne_cislo,datum_prijatia from xdurca01.pacienthosp;



