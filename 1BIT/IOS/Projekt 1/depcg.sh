#!/bin/sh

pflag=0	# 'deklaracia' premennych
gflag=0
dflag=0
rflag=0

trap "rm $temp" INT TERM HUP 	# zachytenie signalov

temp=`mktemp /tmp/$USER.XXXXXX`	# docastny textovy subor

while getopts gpr:d: name		# spracovanie argumentov	
do
	case $name in
	p)	pflag=1;;
	r)	rflag=1
			rval="$OPTARG";;
	d)	dflag=1
			dval="$OPTARG";;
	g)	gflag=1;;
	esac
done

for filename; do true; done		# nacitanie mena suboru

# nacitanie ID vsetkych funkcii do suboru 
objdump -d -j .text $filename | grep "<.*>:" | awk '{print $2}' | sed 's/>://g' | sed 's/<//g' > $temp

flength=`wc -l < $temp`  		# pocet riadkov resp. funkcii
i=0

# nacitanie vsetkych volani ktore prebiehaju + append do suboru
while read caller 
do	
	((i=i+1))
	objdump -d -j .text $filename | awk "/<$caller>:/{a=1;next}/<.*>:/{a=0}a" | awk '$7~/callq/' | sed 's/>//g' | sed 's/<//g' | sed '/+/d' | awk -v var=$caller '{print var " -> " $9}' | sort | uniq >> $temp
	
	if [ $i -eq $flength ]; then
		break
	fi	

done < $temp
 
# ak je zadany parameter -d a -p
if [ $dflag -eq 1 ] && [ $pflag -eq 1 ]; then
	length=`wc -l < $temp`
	((length=length+1)) 			
	cat $temp | grep -w "$dval" | awk -v vard=$dval '$1==vard' >> $temp
	if [ $gflag -eq 1 ]; then # graph vystup
		echo "digraph CG {"
		sed -e 1,"$length"d $temp | sed 's/$/;/' | sed 's/@plt/_PLT/g'  
		echo "}"
	else 
	sed -e 1,"$length"d $temp
	fi

fi

# ak je zadany parameter -r a -p 
if [ $rflag -eq 1 ] && [ $pflag -eq 1 ]; then
	length=`wc -l < $temp`
	cat $temp | grep -w "$rval" | awk -v varr=$rval '$3==varr' >> $temp
	if [ $gflag -eq 1 ]; then
		echo "digraph CG {"
		sed -e 1,"$length"d $temp | sed 's/$/;/' | sed 's/@plt/_PLT/g'  
		echo "}"
	else 
	sed -e 1,"$length"d $temp
	fi
fi 

# ak je zadany iba parameter -r
if [ $rflag -eq 1 ] && [ $pflag -eq 0 ]; then
	length=`wc -l < $temp`
	cat $temp | grep -w "$rval" | sed '/@plt$/d' $temp | awk -v varr=$rval '$3==varr' >> $temp
	if [ $gflag -eq 1 ]; then
		echo "digraph CG {"
		sed -e 1,"$length"d $temp | sed 's/$/;/' | sed 's/@plt/_PLT/g'  
		echo "}"
	else 
	sed -e 1,"$length"d $temp
	fi
fi

# ak je zadany iba parameter -d
if [ $dflag -eq 1 ] && [ $pflag -eq 0 ]; then
	length=`wc -l < $temp`
	((length=length+1))
	cat $temp | grep -w "$dval" | sed '/@plt$/d' $temp | awk -v varr=$dval '$1==varr' >> $temp
	if [ $gflag -eq 1 ]; then
		echo "digraph CG {"
		sed -e 1,"$length"d $temp | sed 's/$/;/' | sed 's/@plt/_PLT/g'  
		echo "}"
	else 
	sed -e 1,"$length"d $temp
	fi
fi

# ak nieje zadany -d ani -r
if [ $dflag -eq 0 ] && [ $rflag -eq 0 ] && [ $pflag -eq 1 ]; then
	if [ $gflag -eq 1 ]; then
		echo "digraph CG {"
		sed -e 1,"$flength"d $temp | sed 's/$/;/' | sed 's/@plt/_PLT/g' 
		echo "}"
	else
	sed -e 1,"$flength"d $temp
	fi
fi

#ak nieje zadany -d ani -r ani -p
if [ $dflag -eq 0 ] && [ $rflag -eq 0 ] && [ $pflag -eq 0 ]; then
	if [ $gflag -eq 1 ]; then
		echo "digraph CG {"
		sed -e 1,"$flength"d $temp | sed '/@plt$/d' | sed 's/$/;/' 
		echo "}"
	else
	sed -e 1,"$flength"d $temp | sed '/@plt$/d'
	fi
fi


rm $temp
