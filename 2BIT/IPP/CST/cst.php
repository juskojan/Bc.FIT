<?php

#CST:xjusko00
$longopts  = array(
    "help::",     // Required value
    "input:",
    "nosubdir",
    "output:"
);

//nacitanie informacii o parametroch
$argArray = getopt("koicpw:s",$longopts);
$argCount = count($argArray);


//zadane nezname argumenty alebo ich je vela
if ($argc-1 != $argCount){
	fwrite(STDERR, "Nespravne zadane argumenty!\n");
	exit(1);
}

//vzajomne vylucenie argumentov c k w i o a nacitanie flagov
$argStr='';
$pflag=FALSE;
$nosubdir=FALSE;
$output=FALSE;
$sflag=FALSE;

foreach($argArray as $key=>$value){
	if($key == "c" || $key == "k" || $key == "w" || $key == "i" || $key == "o"){
		$argStr = $argStr.$key;
	}
	if(strlen($argStr)>1){
		fwrite(STDERR, "Nespravne zadane argumenty!\n");
		exit(1);
	}

	//overenie ci su zadane argumenty nosubdir, output, -p --- flags
	if($key == "nosubdir"){
		$nosubdir = TRUE;
	}
	
	if($key == "p"){
		$pflag = TRUE;
	}

	if($key == "output"){
		$output = TRUE;
	}
	
	if($key == "s"){
		$sflag = TRUE;
	}
}

//je zadany help? tak vypis napovedu
if (array_key_exists('help',$argArray)){
	if ($argc > 2){
		fwrite(STDERR, "Nespravne zadane argumenty!\n");
		exit(1);
	}
	exit ("Skript CST:	Analyzator zdrojovych souboru jazyka C.
Autor:		Jan Jusko, xjusko00
		FIT VUT BRNO 2015
POUZITI:\n
--help			Vypise napovedu k pouziti skriptu.\n
--input=fileordir	Zadany vstupni soubor nebo adresar se zdrojovym 
			kodem v jazyce C. Pri zadani konkretniho souboru
			se automaticky analyzuje. Pri zadani adresare se
			analyzujou vsechny zdrojove soubory s priponou
			.c nebo .h v zadanem adresari i ve vsech jeho
			podadresarich.\n
--nosubdir		Prohledavani bude provedeno pouze v zadanem
			adresari. Nesmi se kombinovat se zadanym konkretnim
			souborem.\n
--output=filename	Zadani vystupniho textoveho souboru, kdyz neni
			zadany, vypisuje se na STDOUT.\n
-k			Vypise pocet klicovych slov (mimo poznamky a retezce)
			na jednotlive soubory a celkem.\n
-o			Vypise pocet jednoduchych operatoru mimo poznamky,
			znakove literaly, a retezce na jednotlive soubory 
			a celkem.\n
-i			Vypise pocet vyskytu identifikatoru na jednotlive
			soubory a celkem.\n
-w=pattern		Vyhleda presny textovy retezec pattern ve vsech
			zdrojovych souborech a vypise pocet neprekryvajicich
			se vyskytu na soubor i celkem.\n
-c			Vypise pocet znaku komentaru vcetne uvozujicich 
			znaku //, /* a */ na jednotlive soubory a celkem.\n
-p			V kombinaci s predoslymi (kromne --help) zpusobi, ze
			soubory se budou vypisovat bez uplne cesty.
			T.j. pouze jmena souboru.\n
Parametry -k -o -i -w -c nelze mezi sebou kombinovat a pokud neni uveden
parametr --help tak se pozaduje zadani prave jednoho z techto parametru.
Parametr -p je naopak mozne kombinovat s uvedenymi dle libosti.\n");
}

//nieje zadany ziaadny jednopismenny parameter
if(strlen($argStr)==0){
	fwrite(STDERR, "Nespravne zadane argumenty!\n");
	exit(1);
}

//--------------------input parameter????... spracovanie potrebnych suborov do pola filenames : name -> text---------------
//umele vlozenie null, pre pripad adresara bez suborov
	$filenames['null']='null';
	$path_filenames['null']='null';

if (array_key_exists('input',$argArray)){	
	//-----------------je zadany input parameter--------------------

	if (is_file($argArray["input"])){
		//------------------------1 je zadany file------OK/------------------------
		//skontroluj ci nieje zadany nosubdir
		if($nosubdir == TRUE){
			fwrite(STDERR, "Nespravne zadane argumenty!\n");
			exit(1);
		}
		//nacitaj subor do premennej file ak sa da precitat!!!!
		if(is_readable($argArray["input"])){
			$text = file_get_contents($argArray["input"]);
		}
		else{
			fwrite(STDERR, "File ".realpath($argArray["input"])." is not readable!\n");
			exit(2);
		}
		
		$bn = pathinfo($argArray["input"]);
	
		//naplnenie filenames premennej podla pflagu
		if($pflag == TRUE){
			$filenames[$bn['basename']] = $text;
		}
		else{
			$filenames[realpath($argArray["input"])] = $text;
		}
 	}
	elseif (is_dir($argArray["input"])){
		//2 ----------------------je zadany nejaky directory------------------
		if($nosubdir == TRUE){
			//-----------prehladavam iba zadany priecinok-----------
			$files = scandir($argArray["input"]);
			//kontrolujem ktore subory maju vhodnu priponu c alebo h
			foreach($files as $key => $value){
				$celynazov = $argArray["input"]."/".$value;
				if(is_file($celynazov)){
					$ext = pathinfo($celynazov, PATHINFO_EXTENSION);
					if ($ext == "h" || $ext == "c"){
						if(is_readable($celynazov)){
							$file = file_get_contents($celynazov);
							//podla pflagu naplnim filenames na spracovanie
							if($pflag == FALSE){	
								$filenames[realpath($celynazov)] = $file;
							}
							else{
								$filenames[$value] = $file;
							}
						}
						else{
							fwrite(STDERR, "File ".realpath($celynazov)." is not readable!\n");
							exit(21);
						}
					}
				}
			}
		}
		//---------------prehladavam aj podadresare lebo nosubdir neni------------------ 
		elseif($nosubdir == FALSE){
			if($pflag==TRUE){
				$filenames = get_filenames($argArray["input"],0);
			}
			elseif($pflag == FALSE){
				$filenames = get_filenames($argArray["input"],1);
			}
		}
	}
	else{
		//vstup neni ani file ani directory.. wtf?
		fwrite(STDERR, "File ".realpath($argArray["input"])." is not readable!\n");
		exit(2);
	}

}
else{
	//---------------3 nieje zadany input parameter---------
	if($nosubdir == TRUE){
		//---------prehladavam iba cwd-------------
		$filearray = scandir(getcwd());
		//iteruj nad subormi a spracuj vhodne subory
		foreach ($filearray as $key => $value){
			//ak najdem vhodny C subor s priponou
			if(is_file($value)){
				$ext = pathinfo($value, PATHINFO_EXTENSION);
				if ($ext == "h" || $ext == "c"){
					if(is_readable($value)){
						$file = file_get_contents($value);
						if($pflag == FALSE){
							$filenames[realpath($value)] = $file;
						}
						else{
							$filenames[$value] = $file;
						}
					}
					else{
						fwrite(STDERR, "File ".realpath($value)." is not readable!\n");
						exit(21);
					}
				}
			}
		}
	}
	//-----------prehladavam aj podpriecinky----------------
	elseif ($nosubdir == FALSE){
		if($pflag==TRUE){
			$filenames = get_filenames(getcwd(),0);
		}
		elseif($pflag == FALSE){
			$filenames = get_filenames(getcwd(),1);
		}
	}
}

//---------SORT------------!!!
ksort($filenames);

//----------- funkcia vracia pole nazov -> string vsetkych suborov v priecinku aj podpriecinkoch rekurzivne-----------------
function get_filenames($cesta,$path_flag)
{
	$filenames['null']='null';
	$path_filenames['null']='null';
	$i= 0;
	//rekurzivne prehladava priecinky od cesty
	$it = new RecursiveDirectoryIterator($cesta);
	foreach(new RecursiveIteratorIterator($it) as $file){
		//len s priponou c, h + musi byt readable
		$ext = pathinfo($file, PATHINFO_EXTENSION);
		if ($ext == 'c' || $ext == 'h'){
			if(is_readable($file)){
				$text = file_get_contents($file);
				$path_filenames[realpath((string)$file)] = $text;
				//ak existuje uz file s rovnakym nazvon tak ho uloz s priponou (
				if(array_key_exists(pathinfo($file,PATHINFO_FILENAME).'.'.pathinfo($file, PATHINFO_EXTENSION),$filenames)){
					$filenames[pathinfo($file,PATHINFO_FILENAME).'.'.pathinfo($file, PATHINFO_EXTENSION).$i.'usof'] = $text;
					$i = $i +1;

				}
				else{
					$filenames[pathinfo($file,PATHINFO_FILENAME).'.'.pathinfo($file, PATHINFO_EXTENSION)] = $text;
				}
			}
			else{
				fwrite(STDERR, "File ".realpath($file)." is not readable!\n");
				exit(21);
			}	
		}
	}
	//vrat podla flagu
	if($path_flag == 1){
	//	var_dump($path_filenames);
		return $path_filenames;
		}
	elseif($path_flag == 0){
	//	var_dump($filenames);
		return $filenames;
	}
}


//----------------- funkcia pocita pocet vyskytov klucovych slov jazyka C ----------------OK
function keywords($text)
{
	//odstran makra!
	$text = preg_replace("/#define(?=\s)(?>.*\\\\\R)*.*\R?/u",'',$text);
	$text = preg_replace('!#include(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	$text = preg_replace('!#ifndef(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	$text = preg_replace('!#endif(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	//odstranenie zlozenych komentarov
	$text = preg_replace('!/\*.*?\*/!s', '', $text);
	//odstranenie jednoduchych komentarov
	$text = preg_replace("!//(?>.*\\\\\R)*.*\R?!u", "", $text);
	//odstranenie stringov v ""
	$text = preg_replace('!".*?"!s', '',$text);
	//odstranenie stringov v ''
	$text = preg_replace('!\'.*?\'!s', '',$text);
	//subor je hotovy na analyzu

	//spocitaj vyskyty klucovych slov z arrayu	
	$array = array('auto','break','case','char','const','continue','default','do','double','else','enum','extern','float','for','goto','if','int','inline','long','register','restrict','return','short','signed','sizeof','static','struct','switch','typedef','union','unsigned','void','volatile','while','_Bool','_Complex','_Imaginary');
	$total = 0;
	
	foreach($array as $key){	
		//matchni, spocitaj, vymaz
		$kolko = preg_match_all("/\b$key\b/",$text,$matches);
		$text = preg_replace("/\b$key\b/",'',$text);
		$total = $total + $kolko; 
	}
	
	return $total;
}


//-----------------------funkcia pocita celkovy pocet znakov v komentaroch------------------ OK
function comments($text,$sflag)
{
	//odstran \r
	$text = str_replace("\r", "", $text);

	//odstran makra aj ich komentarovu cast ak nieje zadany -s operator, inak pocitaj aj v makraaach ROZSIRENIE
	if($sflag != TRUE){
		$text = preg_replace("/#define(?=\s)(?>.*\\\\\R)*.*\R?/u",'',$text);
		$text = preg_replace('!#include(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
		$text = preg_replace('!#ifndef(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
		$text = preg_replace('!#endif(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	}
	//najdi vsetky BLOKOVE komentare a daj ich do pola OK
	preg_match_all("!/\*.*?\*/!s",$text,$zlozene);
	
	$comchars1 = 0; // "inicializacia premennej"
	//cyklus na pocitanie dlzok
	foreach($zlozene[0] as $str){
		$comchars1 = $comchars1 + strlen($str);
	}
	//vymaz blokove komentare.. netreba ich...
	$text = preg_replace("!/\*.*?\*/!s",' ',$text);
	//najdi vsetky RIADKOVE komentare a daj ich do pola
	preg_match_all('!//(?>.*\\\\\R)*.*\R?!u',$text,$riadkove);

	$comchars2 = 0;
	foreach($riadkove[0] as $str){
		$comchars2 = $comchars2 + strlen($str);
	}

	$total = $comchars1 + $comchars2;
	return $total;
}

//----------------------funkcia pocita operatory----------------------OK
function operators($text,$sflag)
{
	//odstran makra!
	$text = preg_replace("/#define(?=\s)(?>.*\\\\\R)*.*\R?/u",'',$text);
	$text = preg_replace("!#include(?=\s)(?>.*\\\\\R)*.*\R?!u",'',$text);
	$text = preg_replace('!#ifndef(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	$text = preg_replace('!#endif(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	//odstranenie zlozenych komentarov
	$text = preg_replace('!/\*.*?\*/!s', '', $text);
	//odstranenie jednoduchych komentarov
	$text = preg_replace("!//(?>.*\\\\\R)*.*\R?!u","", $text);
	//odstranenie stringov v ""
	$text = preg_replace('!".*?"!s', '',$text);
	//odstranenie stringov v ''
	$text = preg_replace('!\'.*?\'!s', '',$text);
	//odstran vararg ...
	$text = preg_replace('/\.\.\./', '', $text);
	//odstran deklaracie ukazatelov
	$text = preg_replace('/char\s*\*+/','',$text);
	$text = preg_replace('/short\s*\*+/','',$text);
	$text = preg_replace('/int\s*\*+/','',$text);
	$text = preg_replace('/unsigned\s*\*+/','',$text);
	$text = preg_replace('/char\s*\*+/','',$text);
	$text = preg_replace('/long\s*\*+/','',$text);
	$text = preg_replace('/float\s*\*+/','',$text);
	$text = preg_replace('/double\s*\*+/','',$text);
	//subor je hotovy na analyzu
		
	//pole vsetkych moznych operatorov usporiadanych od najdlhsich po najkratsie (velmi dolezite)
	$array = array('<<=','>>=','++','--','==','!=','<=','>=','&&','||','<<','>>','+=','-=','*=','%=','/=','&=','|=','^=','->','=','+','-','*','/','%','<','>','!','~','&','|','^','.');
	
	$total = 0;
	foreach($array as $key){
		$kolko = substr_count($text,$key);
		$total = $total + $kolko; 
		//postupne mazanie spracovanych operatorov (iterujeme od najdlhsich cize zabranime viacnasobnemu zapocitavaniu operatorov)
		$text = str_replace($key,'x',$text);
	}	
	// toto je rozsirenie IND
	$index = substr_count($text,"[");

	// rozsirenie indexacia
	if($sflag == TRUE){
		$total = $total + $index;
	}
	
	return $total;
}

//---------------------------funkcia vracia pocet vyskytov pattern-------------------------------OK
function pattern($text,$vzor)
{
	$total = substr_count($text,$vzor);
	return $total;
}

//------------------------------funkcia na identifikatory------------------------------asi OK
function identifiers($text)
{
	//odstran makra!
	$text = preg_replace("/#define(?=\s)(?>.*\\\\\R)*.*\R?/u",'',$text);
	$text = preg_replace('!#include(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	$text = preg_replace('!#ifndef(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	$text = preg_replace('!#endif(?=\s)(?>.*\\\\\R)*.*\R?!u','',$text);
	//odstranenie zlozenych komentarov
	$text = preg_replace('!/\*.*?\*/!s', '', $text);
	//odstranenie jednoduchych komentarov
	$text = preg_replace("!//(?>.*\\\\\R)*.*\R?!u","", $text);
	//odstranenie stringov v ""
	$text = preg_replace('!".*?"!s', ' ',$text);
	//odstranenie stringov v ''
	$text = preg_replace('!\'.*?\'!s', ' ',$text);
	//subor je hotovy na analyzu

	$array = array('auto','break','case','char','const','continue','default','do','double','else','enum','extern','float','for','goto','if','int','inline','long','register','restrict','return','short','signed','sizeof','static','struct','switch','typedef','union','unsigned','void','volatile','while','_Bool','_Complex','_Imaginary');
	$total = 0;
	
	//odstran keywords
	foreach($array as $key){
		$text = preg_replace("/\b$key\b/",' ',$text); 
	}

	$array = array('<<=','>>=','++','--','==','!=','<=','>=','&&','||','<<','>>','+=','-=','*=','%=','/=','&=','|=','^=','->','=','+','-','*','/','%','<','>','!','~','&','|','^','.',',',')','(','{','}','[',']',';','?',':');
	
	foreach($array as $key){
		//postupne mazanie spracovanych operatorov (iterujeme od najdlhsich cize zabranime viacnasobnemu zapocitavaniu operatorov)
		$text = str_replace($key,' ',$text);
	}
	//odstran dristy... cisla a podciarkovnikz
	$text = preg_replace("/\d*/",'',$text);
	$text = preg_replace("/_/",'a',$text);
	
	//echo $text;
	
	return (str_word_count($text));
}

//funkcia analyzuje co chceme spocitat a spocita to, vracia pole  name -> count
// parameter vzor (pattern) musi byt vzdy pristupny
function analyze($fileArray,$flag,$vzor,$sflag)
{
	$analArray['null']='null';
	foreach($fileArray as $filename => $text){
		if($filename != 'null'){
			switch($flag){
				case "c":
					$analArray[$filename] = comments($text,$sflag);
					break;
				case "k":
					$analArray[$filename] = keywords($text);
					break; 
				case "i":
					$analArray[$filename] = identifiers($text);
					break;
				case "w":
					$analArray[$filename] = pattern($text,$vzor);
					break;
				case "o":
					$analArray[$filename] = operators($text,$sflag);
					break;
			}
		}
	}

	return $analArray;
}

//potreba osetrit nezadany -w a prazdny pattern :(
if($argStr != "w"){
	$argArray["w"] = "random s#!t";
}

// vystupny subor
if($output == TRUE){
	$outfile = $argArray["output"]; 
}
else{
	$outfile = "nope";
}

// analyza filenames ... 
$filenames = analyze($filenames,$argStr,$argArray["w"],$sflag);

//funkcia sa stara o vypis!!!
function vypis($analArray,$out)
{
	$max_name = 0;
	$max_int = 0;
	$celkem = 0;
	//hladanie maximalnych dlziek nazvu a cisla
	foreach($analArray as $filename => $count){
		if($filename != 'null'){		

			if(preg_match("/\d*usof/",$filename)){
				$filename = preg_replace("/\d*usof/",'',$filename);
			}
		
			if(strlen($filename) > $max_name){
				$max_name = strlen($filename);
			}
	
			//echo $count,'-',strlen((string)$count),"\n";	
	
			if(strlen((string)$count) > $max_int){
				$max_int = strlen($count);
			}
			//sum ... celkem
			$celkem = $celkem + $count;	
		}
	}
	
	// pridanie polozky CELKEM na koniec anal. zoznamu...
	$analArray["CELKEM:"] = $celkem;
	// co ak je slovo CELKEM dlhsie ako vsetky cesty? alebo celkovy pocet dlhsi ako ostatne????
	if(strlen("CELKEM:") > $max_name){
		$max_name = strlen("CELKEM:");
	}
	if(strlen((string)$celkem) > $max_int){
		$max_int = strlen((string)$celkem);
	}
	//vytvor subor alebo ho vytvor
	if($out != "nope"){
		$handle = fopen($out,"w") or die(3);
	}

	$vypis='';

	foreach($analArray as $filename => $count){
		if($filename != 'null'){
			if(preg_match("/\d*usof/",$filename)){
				$filename = preg_replace("/\d*usof/",'',$filename);
			}
			$pocet_medzier = (($max_name - strlen($filename)) + 1 + ($max_int - strlen($count)));
			$medzery = '';
			for($i = 0; $i < $pocet_medzier; $i++){
				$medzery = $medzery . ' ';
			}
			//dal som tam nasilu polozku null aby nehadzalo chyby undefined
			$vypis = $filename.$medzery.$count."\n";
			if($out == "nope"){
				echo $vypis;
			}
			else{
				file_put_contents($out,$vypis,FILE_APPEND);
			}
		}
	}
}

// no hotovo!!!
vypis($filenames,$outfile);
?>
