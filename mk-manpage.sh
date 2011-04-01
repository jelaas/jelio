#!/bin/bash

export SECTION=3
export DATE=$(date "+%B %Y")

function convertU {
    read LINE
    if [ "$CONVERTS" ]; then
	for CON in $CONVERTS; do
	    SCON="${CON:1:${#CON}-2}"
	    LINE=$(echo "$LINE"|sed "s/$CON/\\\fI${SCON}\\\fP/g")
	done
    fi
    echo "$LINE"|sed "s/\\ESC/\\x5c\\x5c\\x5c\\x5c/g"
}

function convertUs {
    read LINE
    if [ "$CONVERTS" ]; then
	for CON in $CONVERTS; do
	    SCON="${CON:1:${#CON}-2}"
	    LINE=$(echo "$LINE"|sed "s/*$SCON,/*\\\fI${SCON}\\\fP,/g")
	    LINE=$(echo "$LINE"|sed "s/*$SCON)/*\\\fI${SCON}\\\fP)/g")
	    LINE=$(echo "$LINE"|sed "s/ $SCON,/ \\\fI${SCON}\\\fP,/g")
	    LINE=$(echo "$LINE"|sed "s/ $SCON)/ \\\fI${SCON}\\\fP)/g")
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
    
    echo ".TH \"$NAME\" \"$SECTION\" \"$DATE\" \"jelio function\""
    echo ".SH NAME"
    echo "$NAME"
    echo ".SH SYNOPSIS"
    while read LINE; do
	echo ".BI \"$LINE\""
	if [ -z "$LINE" ]; then
	    break;
	fi
	echo ".br"
    done
    echo ".sp"
    echo ".B $(egrep "[ *]$NAME[(]" jelio.h|head -1)"|convertUs
    echo ".SH DESCRIPTION"
    while read LINE; do
	echo "$LINE"|convertU
	if [ -z "$LINE" ]; then
	    break;
	fi
	echo ".br"
    done
    echo ".SH RETURN VALUE"
    while read LINE; do
	echo "$LINE"|convertU
	if [ -z "$LINE" ]; then
	    break;
	fi
    done
    echo ".SH SEE ALSO"
    while read LINE; do
	echo ".BR $LINE"|convertU
	if [ -z "$LINE" ]; then
	    break;
	fi
    done
}

for f in *.mn; do
    M=$(basename $f|cut -d . -f 1).$SECTION
    echo "Generating $M from $f"
    cat $f | manpage >man/$M
done