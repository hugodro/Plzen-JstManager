/**************************************************
* File: indexTable.cc.
* Desc: Implementation of the IndexTable class.
* Module: AkraLog : JSTManager.
* Rev: 26 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "entryTable.h"
#include "indexTable.h"

IndexTable::IndexTable()
{
    indexes= new EntryTable();
    nbrIndexes= 0;
}

void IndexTable::reset(void)
{
    delete indexes;
    indexes= new EntryTable();
    nbrIndexes= 0;
}


bool IndexTable::indexForID(void *anID, unsigned int *dest)
{
    unsigned int i;

    for (i= 0; i < nbrIndexes; i++) {
	if (indexes->elementAt(i) == anID) {
	    *dest= i+1;		// Index 0 is for the universal java.lang 'null' object.
	    return true;
	}
    }
    indexes->addElement(anID);
    nbrIndexes++;
    *dest= nbrIndexes;
    return false;
}

