#!/usr/bin/python

###############################################
#### Projekt ISJ  			   ####
#### Autor: Jan Jusko 			   ####
#### xjusko00 				   ####
###############################################


import urllib.request
import re
import os
import xmlrpc.client as RPCCL
import sys
import zipfile


#funkcia na parsovanie imdbID identifikacne cislo filmu
def parseIMDB(url):
    request = urllib.request.urlopen(url+"/xml").read()
    imdbID = re.search("(?<=MovieIMDBID=..)\d+", str(request))

    if imdbID is None:
        return 0
    return imdbID.group(0)


#stiahnutie ceskych titulkov zadanych v argumente
arglink = sys.argv[1]
number = arglink.split("/")[5]

downloadlink = "http://dl.opensubtitles.org/cs/download/subad/" + number
urllib.request.urlretrieve(downloadlink,"czsubtitles.zip")

#otestovanie, ci sa titulky stiahli
the_zip_file = zipfile.ZipFile("czsubtitles.zip")
ret = the_zip_file.testzip()

if ret is not None:
	print("There was a problem with downloading CZ subtitles.")
	quit(-1)    	
else:
        print("CZ subtitles successfully downloaded!")

#extrahovanie CZ titulkov do adresara cz
czsize = os.path.getsize("czsubtitles.zip")
with zipfile.ZipFile('czsubtitles.zip', "r") as z:
    z.extractall("cz")

#ID filmu zadaneho argumentom
imdbID = parseIMDB(arglink)

#login info
url = "http://api.opensubtitles.org/xml-rpc"
login = ""
password = ""
language = "en"
userAgent = "OS Test User Agent"

#pripojenie na server
conn = RPCCL.ServerProxy(url)
token = conn.LogIn(login, password, language, userAgent)["token"]

#hladanie ID titulkov eng
query = conn.SearchSubtitles(token,[{'sublanguageid': 'eng', 'imdbid': imdbID}])
engIDs = re.findall("(?<='IDSubtitle': ')\d+", str(query))

#zmaz duplicitne zaznamy
engIDs = sorted(set(engIDs))
print(str(len(engIDs)) + " EN subtitles found!")

if len(engIDs) > 0:
	print("Downloading " + str(len(engIDs)) + " subtitles...")
else:
	print("Terminating script")
	quit()

#stahovanie anglickych titulkov a hladanie najlepsej zhody podla velkosti suboru
maximum = 1000000
for sub in engIDs:
	downloadlink = "http://dl.opensubtitles.org/cs/download/subad/" + sub
	urllib.request.urlretrieve(downloadlink,str(sub)+".zip")
	rozdiel = czsize - os.path.getsize(str(sub)+".zip")
	#print(str(abs(rozdiel)) + " " + str(sub))
	if abs(rozdiel) < maximum:
		maximum = abs(rozdiel)	
		bestmatch = sub

print("The best match to CZ subtitles according to file size is file: " + str(bestmatch) + ".zip")

zmaz = input("Do you wish to delete other files? Y/N  ")

if (zmaz == 'Y'):
	print("Deleting " + str(len(engIDs)-1) + " files...")
	for sub in engIDs:
		if sub != bestmatch:
			os.remove(str(sub) + ".zip")
else:
	print("All downloaded subtitles stays in the current directory.")

#extrahovanie anglickych titulkov
with zipfile.ZipFile(str(bestmatch) + ".zip", "r") as z:
    z.extractall("en")


