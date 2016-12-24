#!/bin/bash


# docastny subor
temp=`mktemp /tmp/$USER.XXXXXX` 

trap "rm $temp" INT TERM HUP		# zachytenie signalov
 
dflag=0
rflag=0
gflag=0

while getopts gr:d: name		# spracovanie argumentov	
do
	case $name in
	r)	rflag=1
			rval="$OPTARG";;
	d)	dflag=1
			dval="$OPTARG";;
	g)	gflag=1;;
	esac
done

# nacitanie suborov do premennej zadanych ako argument
for subor in $*
do
if [ -f $subor ]; then
objects="$objects $subor"
fi
done

# nacitanie definovanych objektov
for subor in $*
do

if [ -f $subor ]; then
	nm $subor | awk '$3~/.+/' | awk -v var=$subor '{print $3 " D " var}' >> $temp
fi
done

linecount=`wc -l < $temp`


# vypis zavislosti
while read line			 # pre kazdy riadok symbol + jeho def. modul
do
symbol=$(echo $line | awk '{print $1}')
dmodul=$(echo $line | awk '{print $3}')

for module in ${objects[@]}; do	# iteruj nad vypisom kazdeho modulu
	flag=$(nm $module | awk '$1~/U/' | grep -c -w "$symbol") 
	if [ $flag -eq 1 ];then
	echo $module "->" $dmodul "("$symbol")" | cat >> $temp
	fi
done
done < $temp
newlinecount=`wc -l < $temp`


# zadany d parameter
if [ $dflag -eq 1 ] && [ $gflag -eq 0 ]; then
cat $temp | sed -e 1,"$linecount"d | sort | uniq | awk -v darg=$dval '$1==darg'
fi

#zadany -g a -d
if [ $gflag -eq 1 ] && [ $dflag -eq 1 ]; then
	echo "digraph GSYM {"
	cat $temp | sed -e 1,"$linecount"d | sort | uniq | awk -v darg=$dval '$1==darg' | sed 's/-/_/g' | sed 's/\./D/g' | sed 's/+/P/g' | sed 's/(//g' | sed 's/)//g' | awk -v sipka=" -> " -v lab=" [label=\"" -v end="\"];" '{print $1 sipka $3 lab $4 end}' >> $temp

for objekt in ${objects[@]}; do

exists=$(cat $temp | sed -e 1,"$newlinecount"d | grep -c -w "$objekt")  
if [ $exists -ge 1 ];then
echo $objekt | sed 's/\./D/g' | awk -v huh=$objekt -v label=" [label=\"" -v end="\"];" -v Z="Z" '{print Z $1 label huh end}' >> $temp
fi

done
cat $temp | sed -e 1,"$newlinecount"d | sort | uniq | sed 's/^Z//g'
echo "}"


fi

# zadany r parameter
if [ $rflag -eq 1 ] && [ $gflag -eq 0 ]; then
cat $temp | sed -e 1,"$linecount"d | sort | uniq | awk -v rarg=$rval '$3==rarg'
fi

#zadany -g a -r
if [ $gflag -eq 1 ] && [ $rflag -eq 1 ]; then
echo "digraph GSYM {"
	cat $temp | sed -e 1,"$linecount"d | sort | uniq | awk -v rarg=$rval '$3==rarg' | sed 's/-/_/g' | sed 's/\./D/g' | sed 's/+/P/g' | sed 's/(//g' | sed 's/)//g' | awk -v sipka=" -> " -v lab=" [label=\"" -v end="\"];" '{print $1 sipka $3 lab $4 end}' >> $temp

for objekt in ${objects[@]}; do

exists=$(cat $temp | sed -e 1,"$newlinecount"d | grep -c -w "$objekt")  
if [ $exists -ge 1 ];then
echo $objekt | sed 's/\./D/g' | awk -v huh=$objekt -v label=" [label=\"" -v end="\"];" -v Z="Z" '{print Z $1 label huh end}' >> $temp
fi

done
cat $temp | sed -e 1,"$newlinecount"d | sort | uniq | sed 's/^Z//g'
echo "}"


fi

# nezadany ani -d ani -r ani -g
if [ $dflag -eq 0 ] && [ $rflag -eq 0 ] && [ $gflag -eq 0 ]; then
cat $temp | sed -e 1,"$linecount"d | sort | uniq 
fi 

#zadany iba -g
if [ $dflag -eq 0 ] && [ $rflag -eq 0 ] && [ $gflag -eq 1 ]; then
echo "digraph GSYM {"
cat $temp | sed -e 1,"$linecount"d | sort | uniq |  sed 's/-/_/g' | sed 's/\./D/g' | sed 's/+/P/g' | sed 's/(//g' | sed 's/)//g' | awk -v sipka=" -> " -v lab=" [label=\"" -v end="\"];" '{print $1 sipka $3 lab $4 end}' >> $temp


for objekt in ${objects[@]}; do

exists=$(cat $temp | sed -e 1,"$newlinecount"d | grep -c -w "$objekt")  
if [ $exists -ge 1 ];then
echo $objekt | sed 's/\./D/g' | awk -v huh=$objekt -v label=" [label=\"" -v end="\"];" -v Z="Z" '{print Z $1 label huh end}' >> $temp
fi

done
cat $temp | sed -e 1,"$newlinecount"d | sort | uniq | sed 's/^Z//g'
echo "}"

fi


rm $temp
