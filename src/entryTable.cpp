/**************************************************
* File: entryTable.cc.
* Desc: Implementation of the EntryTable class.
* Module: AkraLog : JSTManager.
* Rev: 26 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "entryTable.h"


/**************************************************
* Implementation: EntryTable.
**************************************************/

unsigned int EntryTable::nbrSubClasses= 1;
ClassSizeArray EntryTable::classSizes[1]= { {1, 255 } };

EntryTable::EntryTable(void)
{
    next= NULL;
    elements= new void*[classSizes[0][1]];
    nbrElements= 0;
}


EntryTable::EntryTable(EntryTable *aClonee)
{
    next= NULL;
    elements= aClonee->elements;
    nbrElements= aClonee->nbrElements;
}



EntryTable::~EntryTable(void)
{
    if (next != NULL) {
	delete next;
    }
    delete elements;
}


void EntryTable::addElement(void *anElement)
{
    if (nbrElements < classSizes[0][1]) {
	elements[nbrElements++]= anElement;
    }
    else {
	EntryTable *newTable;

	if (next != NULL) {
	    newTable= next;
	    while (newTable->next != NULL) {
		newTable->nbrElements+= classSizes[0][1];
		newTable= newTable->next;
	    }
	    newTable->next= new EntryTable(this);
	}
	else {
	    next= new EntryTable(this);
	}
	elements= new void*[classSizes[0][1]];
	nbrElements= 0;
	elements[nbrElements++]= anElement;
    }
}


void *EntryTable::elementAt(unsigned int anIndex)
{
    if (next != NULL) {
	if (anIndex >= next->nbrElements) {
	    anIndex-= next->nbrElements;
	    if (anIndex < nbrElements)
		return elements[anIndex];
	    else
		return NULL;
	}
	else {
	    EntryTable *cursor= next;

	    while (anIndex >= classSizes[0][1]) {
		anIndex-= classSizes[0][1];
		cursor= cursor->next;
	    }
	    return cursor->elements[anIndex];
	}
    }
    else {
	if (anIndex < nbrElements)
	    return elements[anIndex];
	else
	    return NULL;
    }
}


