#!/usr/bin/env python3

#XTD:xjusko00
import sys
import argparse
from argparse import RawDescriptionHelpFormatter
from xml.dom.minidom import parse
import os
import os.path
# NEJAKE GLOBALNE KONSTANTY
BIT = 1
INT = 2
FLOAT = 3
STRING = 4
NVARCHAR = 5
NTEXT = 6
ELEMENT = 1
TEXT = 3
# spracuj argumenty a skontroluj ich spravnost!
def spracovanie_argumentov():
	parser = argparse.ArgumentParser(description="IPP proj. 2 (XTD) 2015L \nSkript na zaklade vstupniho XML suboru s daty vytvori sadu SQL prikazu \ngenerujicich prislusnou strukturu tabulek v SQL databazi. \nAutor: Jan Jusko, xjusko00", add_help = False, formatter_class=RawDescriptionHelpFormatter)
	parser.add_argument('--help', action='count', default= 0, help='Vypise napovedu k spravnimu spusteni. ')
	parser.add_argument('--input', dest="in_file", action="append", default= [], help='XML vstupni subor/stdin. ')
	parser.add_argument('--output', dest="out_file", action="append", default= [], help='XML vystupni soubor/stdout. ')
	parser.add_argument('--header', dest="header", action="append", default= [], help='Vlozi zakomentovanej HEADER na zacatek vystupu. ')
	parser.add_argument('--etc', dest="etc_n", action="append", default= [], help='Maximalni pocet sloupcu vzniklych ze stejnojmennych podelementu. ')
	parser.add_argument('-a', action="count", default= 0, help='Nebudou se generovat sloupce z atributu. ')
	parser.add_argument('-b', action="count", default= 0, help='Obsahuje-li element vice podelementu stejneho nazvu, bude se uvazovat pouze jeden z nich. Nekombinovat s --etc. ')
	parser.add_argument('-g', action="count", default= 0, help='Lze jej uplatnit s jakymkoliv jinym prepinacem vyjma --help, generuje XML s tabulkami i vztahy mezi nima. ')
	parser.add_argument('--isvalid', dest="isvalid", action="append", default= [], help='Vstupni soubor k overeni ve formatu XML. ')
	
	try:
		args = parser.parse_args()
	except:
		print('Chyba pri spusteni! Pre napovedu pouzite --help.', file=sys.stderr)
		sys.exit(1)
		
	if (len(args.out_file) > 1):
		print('Chyba: vicenasobny vystupni soubor!', file=sys.stderr)
		sys.exit(1)
	if (len(args.header) > 1):
		print('Chyba: vicekrat zadany --header!', file=sys.stderr)
		sys.exit(1)
	if (len(args.etc_n) > 1):
		print('Chyba: vicekrat zadano etc!', file=sys.stderr)
		sys.exit(1)
	if (len(args.isvalid) > 1):
		print('Chyba: vickrat --validate!', file=sys.stderr)
		sys.exit(1)
	if ((args.a > 1) or (args.b > 1) or (args.g > 1)):
		print('Chyba: vicenasobny parametr -a/-b/-g!', file=sys.stderr)
		sys.exit(1)
	if ((args.b == 1 ) and (len(args.etc_n) == 1)):
		print('Chyba: --etc a -b zadany naraz!', file=sys.stderr)
		sys.exit(1)
	if (args.help == 1):
		if(len(sys.argv) == 2):
			parser.print_help()
			sys.exit(0)
		else:
			print('Chyba: --help a jiny argument!', file=sys.stderr)
			sys.exit(1)
	if (len(args.in_file) > 1):
		print('Chyba: vicenasobny stupni soubor!')
		sys.exit(1)
	return args

# POMOCNA FUNKCIA VYHODNOCUJE ROVNOST UZLOV
def equal_nodes(node1, node2):
	if node1 == node2:
		return True
	else:
		return False


# CREATE TABLES OF ELEMENTS PLS
def parse_me_gently(tab_elements, act_node, rootName, wanna_attributes):
	# POMOCNE POLE NA POCITANIE FKeys
	foreign_keys = {}

	for node in act_node.childNodes:
		parent = node.parentNode.nodeName.lower()
		# JE TO ELEMENT?
		if node.nodeType == ELEMENT:
			# EXISTUJU UZ ZAZNAMY O ELEMENTE?
			aktualny = node.nodeName.lower()
			if(aktualny in tab_elements.keys()):
				pass
			else:
				tab_elements[aktualny] = {}
				# VNORENY SLOVNIK PRE ATRIBUTY
				tab_elements[aktualny]["attrs"] = {}

			# SPRACUJ ATRIBUTY DO TABULKY
			if (wanna_attributes):
				attr_count = len(node.attributes)
				#print ("count> ",attr_count)
				#tab_elements[aktualny][]
				for i in range(attr_count):
					#print (i, attr_count)
					# IMPLICITNE TAM BUDE NVARCHAR, MOZE SA NIZSIE ZMENIT
					tab_elements[aktualny]["attrs"][node.attributes.item(i).name.lower()] = NVARCHAR
					# ALEBO BIT?
					hodnota = node.attributes.item(i).value

					if (hodnota == '1') or (hodnota == '0') or (hodnota == 'True') or (hodnota == 'False') or (hodnota == ''):
						tab_elements[aktualny]["attrs"][node.attributes.item(i).name.lower()] = BIT
						continue
					# CO AK INT?
					try:
						int(hodnota)
						tab_elements[aktualny]["attrs"][node.attributes.item(i).name.lower()] = INT
						continue
					except:
						pass
					# ZEBY NAKONIEC FLOAT?
					try:
						float(hodnota)
						tab_elements[aktualny]["attrs"][node.attributes.item(i).name.lower()] = FLOAT
						continue
					except:
						pass
					pass
			# SPRACUJ PODELEMENTY - CUZI KLICE
			if not equal_nodes(act_node.nodeName, rootName):
				if aktualny in foreign_keys.keys():					
					foreign_keys[aktualny] = foreign_keys[aktualny] + 1
				else:
					foreign_keys[aktualny] = 1
			# REKURZIA DO NEKONECNA A ESTE DALEJ!!
			parse_me_gently(tab_elements, node, rootName, wanna_attributes)
		# JEDNA SA O TEXTOVY ATRIBUT
		elif node.nodeType == TEXT:
			# print(node.data.strip())
			# AK SA NEJEDNA O PRAZDNY RETAZEC
			if len(node.data.strip()) != 0:
				if not equal_nodes(act_node.nodeName, rootName):
					hodnota = node.data
					# CO TO JE?????
					assign = NTEXT

					if (hodnota == '1') or (hodnota == '0') or (hodnota == 'True') or (hodnota == 'False') or (hodnota == ''):
						assign = BIT
					# CO AK INT?
					try:
						int(hodnota)
						assign = INT
					except:
						pass
					# ZEBY NAKONIEC FLOAT?
					try:
						float(hodnota)
						assign = FLOAT
					except:
						pass
					# VLOZ TO TAM
					if "value" not in tab_elements[parent]["attrs"]:
						tab_elements[parent]["attrs"]["value"] = assign

					if "value" in tab_elements[parent]["attrs"]:
						if tab_elements[parent]["attrs"]["value"] < assign:
							tab_elements[parent]["attrs"]["value"] = assign
		# SPOCTY VYSKYTOV FKs
		if not equal_nodes(act_node.nodeName, rootName):
			for fk in foreign_keys:
				if fk in tab_elements[parent]:
					if tab_elements[parent][fk] < foreign_keys[fk]:
						tab_elements[parent][fk] = foreign_keys[fk]					
				else:
					tab_elements[parent][fk] = foreign_keys[fk]
					


def create_tables(vystup, tab_elements, b_param):
	for element in tab_elements:
		#vystup.write(element)
		vystup.write("CREATE TABLE " + element + "(\n")
		vystup.write("   prk_" + element + "_id INT PRIMARY KEY")
		#print(tab_elements["library"]["book"])
		for FK in tab_elements[element]:
			#print("iterujem",FK)
			if(FK != "attrs"):
				if(b_param):
					if(tab_elements[element][FK] > 1):
						tab_elements[element][FK] = 1

				if(tab_elements[element][FK] > 1):
					for i in range(1,tab_elements[element][FK]+1):
						vystup.write(",\n   " + FK + str(i) + "_id INT")
				else:
					vystup.write(",\n   " + FK + "_id INT")
				#if(tab_elements[element][FK] > 1):
				#	print(tab_elements[element], tab_elements[element][FK])
				#vystup.write(",\n   " + FK + "_ID INT")

		for ATTR in tab_elements[element]["attrs"]:
			typ_atributu = tab_elements[element]["attrs"][ATTR]
			if typ_atributu == BIT:
				print_me = " BIT"
			elif typ_atributu == FLOAT:
				print_me = " FLOAT"
			elif typ_atributu == INT:
				print_me = " INT"
			elif typ_atributu == NTEXT:
				print_me = " NTEXT"
			elif typ_atributu == NVARCHAR:
				print_me = " NVARCHAR"

			vystup.write(",\n   " + ATTR + print_me)

		vystup.write("\n);\n\n")

def etc(etcn, tab_elements):
	to_delete=[]
	for element in tab_elements:
		for i in tab_elements[element]:
			if (type(tab_elements[element][i]) != type(dict())):

				#print(tab_elements[element][i])
				if(tab_elements[element][i] > int(etcn)):
					tab_elements[i][element] = -1
					to_delete.append(i)
		for item in to_delete:
			del tab_elements[element][item]
		to_delete = []

def konflikt(tab_elements):
	for element in tab_elements:
		for i in tab_elements[element]:
			if i != "attrs":
				if "prk_" + element + "_id" == i+"_id":
					#print(element, i)
					print('Konflikt primarniho klice a ciziho klice', file=sys.stderr)
					exit(90)
			
			for x in tab_elements[element]["attrs"]:
				#print(element,i, x)
				if("prk_" + element + "_id" == x):
					print('Konflikt primarniho klice a atributu', file=sys.stderr)
					exit(90)

				if(i+"_id" == x):
					print('Konflikt ciziho klice a atributu', file=sys.stderr)
					exit(90)
				#print(element,x)	


def validate(tab_elements, val_tab_elements):
	for element in val_tab_elements:
		#print(element)
		# su tam FK?
		for FK in val_tab_elements[element]:
			if FK != "attrs":
				if FK not in tab_elements[element]:
					return False
		# su tam PK?			
		if element not in tab_elements.keys():
			return False
		

		#porovnaj atributy
		for elem in val_tab_elements[element]["attrs"]:
			#print (elem)
			if elem not in tab_elements[element]["attrs"]:
				return False
			for attr in val_tab_elements[element]["attrs"]:
				if attr not in tab_elements[element]["attrs"] or val_tab_elements[element]["attrs"][attr] > tab_elements[element]["attrs"][attr]:
					return False
	
	return True
	


def main():
	# SPRACOVANIE ARGUMENTOV
	params = spracovanie_argumentov()
	#print(BIT, NVARCHAR)
	# NACITANIE VSTUPNEHO SUBORU
	if (len(params.in_file) == 0):
		vstup = sys.stdin
	elif (len(params.in_file) == 1):
		try:
			vstup = open(params.in_file[0],'r')
		except IOError:
			print('Vadny vstupny subor!', file=sys.stderr)
			sys.exit(2)

	# NACITANIE VYSTUPNEHO SUBORU
	if (len(params.out_file) == 1):
		try:
			main_vystup = open(params.out_file[0],'w')
		except IOError:
			print('Vadny vystupny subor!', file=sys.stderr)
			sys.exit(2)
	else:
		main_vystup = sys.stdout
					 

	# VYTVOR PRAZDNE TABULKU ATRIBUTOV A ELEMENTOV
	tab_elements = {}
	# VYTVOR TRIEDU ROZPARSOVANEHO XML SUBORU
	p_class = parse(vstup)
	# NACITAJ PRVY UZOL A JEHO NAZOV
	starting_point = p_class.firstChild
	rootName = starting_point.nodeName.lower()
	# CHCES GENEROVAT ATRIBUTY?
	if(params.a == 0):
		wanna_attributes = True
	else:
		wanna_attributes = False
	# TAK POJD!
	parse_me_gently(tab_elements, starting_point, rootName, wanna_attributes)
	# NAPLNENE SLOVNIKY
	#print(tab_elements,"\n\n\n",tab_attributes)

	#print(tab_elements,"\n\n\n")
	#exit(1)
	konflikt(tab_elements)
	

	if(params.b == 1):
		b = True
	else:
		b = False

	#print(params.etc_n[0])	
	if (len(params.etc_n) > 0):
		etc(params.etc_n[0], tab_elements)

	if(params.g == 1):
		pass


	# TREBA VALIDOVAT?
	val_tab_elements = {}
	if(len(params.isvalid) == 1):
		if os.path.isfile(params.isvalid[0]) and os.access(params.isvalid[0], os.R_OK):
			#print "File exists and is readable"
			val_class= parse(params.isvalid[0])
		else:
			print('Vadny vstupny subor v parametre --isvalid!', file=sys.stderr)
			sys.exit(2)
		
		val_starting_point = val_class.firstChild
		val_rootName = val_starting_point.nodeName.lower()

		parse_me_gently(val_tab_elements, val_starting_point, val_rootName, wanna_attributes)
		konflikt(val_tab_elements)

		if(validate(tab_elements, val_tab_elements)==False):
			print('Nesu validne!', file=sys.stderr)
			exit(91)

	# SPRAV VYPIS
	if(len(params.header) == 1):
		wanna_header = True
		main_vystup.write("--" + params.header[0] + "\n\n")		
	create_tables(main_vystup, tab_elements, b)



if __name__ == "__main__":
	main()