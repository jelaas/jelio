#!/bin/bash

export SECTION=3
export DATE=$(date "+%B %Y")
export INDENT="&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"

function convertU {
    read LINE
    if [ "$CONVERTS" ]; then
	for CON in $CONVERTS; do
	    SCON="${CON:1:${#CON}-2}"
	    LINE=$(echo "$LINE"|sed "s/$CON/<u>${SCON}<\/u>/g")
	done
    fi
    echo "$LINE"|sed "s/\\ESC/\x5c\x5c/g"
}
function convertRef {
    read LINE
    REF=$(echo "$LINE"|cut -d ' ' -f 1)
    REST=$(echo "$LINE"|cut -d ' ' -sf 2-)
    echo "<a href=\"${REF}.html\">$REF</a>$REST"
}

function convertUs {
    read LINE
    if [ "$CONVERTS" ]; then
	for CON in $CONVERTS; do
	    SCON="${CON:1:${#CON}-2}"
	    LINE=$(echo "$LINE"|sed "s/*$SCON,/*<u>${SCON}<\/u>,/g")
	    LINE=$(echo "$LINE"|sed "s/*$SCON)/*<u>${SCON}<\/u>)/g")
	    LINE=$(echo "$LINE"|sed "s/ $SCON,/ <u>${SCON}<\/u>,/g")
	    LINE=$(echo "$LINE"|sed "s/ $SCON)/ <u>${SCON}<\/u>)/g")
	done
	echo "$LINE"
    else
	echo "$LINE"
    fi
}

function manpage {
    read NAME
    read CONVERTS
    if [ "$CONVERTS" ]; then
	read
    fi
    
    echo "<html><head><title>"
    echo "$NAME $SECTION $DATE jelio function"
    echo "</title></head>"
    echo "<body>"
    echo "<p><h3><b>NAME</b></h3>"
    echo "$INDENT $NAME</p>"
    echo "<p><b><h3>SYNOPSIS</b></h3><table><tr><td>$INDENT</td><td>"
    while read LINE; do
	LINE=$(echo "$LINE"|sed 's/</\&lt;/g')
	echo "<b>$LINE</b><br>"
	if [ -z "$LINE" ]; then
	    break;
	fi
    done
    DECL="$(egrep "[ *]$NAME[(]" jelio.h|head -1)"
    HEAD="$(echo $DECL|cut -d '(' -f 1)"
    TAIL="$(echo $DECL|cut -d '(' -f 2-)"
    echo "<table><tr><td valign=top><b>"
    echo "$HEAD(</b></td><td>"|sed 's/ /\&nbsp;/g'
    echo "<b>$TAIL</b></td></tr></table>"|convertUs
    echo "</td></tr></table></p>"
    echo "<p><h3>DESCRIPTION</h3><table><tr><td>$INDENT</td><td>"
    while read LINE; do
	echo "$LINE"|convertU
	if [ -z "$LINE" ]; then
	    break;
	fi
	echo "<br>"
    done
    echo "</td></tr></table></p>"
    echo "<p><h3>RETURN VALUE</h3><table><tr><td>$INDENT</td><td>"
    while read LINE; do
	echo "$LINE"|convertU
	if [ -z "$LINE" ]; then
	    break;
	fi
    done
    echo "</td></tr></table></p>"
    echo "<p><h3>SEE ALSO</h3><table><tr><td>$INDENT</td><td>"
    while read LINE; do
	echo "$LINE"|convertRef
	if [ -z "$LINE" ]; then
	    break;
	fi
    done
    echo "</td></tr></table></p>"
    echo "</body></html>"
}

echo "<html>" >doc/index.html
for f in *.mn; do
    M=$(basename $f|cut -d . -f 1).html
    echo "Generating $M from $f"
    echo "<a href=\"$M\">$M</a><br>" >> doc/index.html
    cat $f | manpage >doc/$M
done
echo "</html>" >>doc/index.html