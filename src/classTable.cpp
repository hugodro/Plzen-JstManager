/**************************************************
* File: classTable.cc.
* Desc: Implementation of the ClassTable class.
* Module: AkraLog : JstManager.
* Rev: 25 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <RunTime/classFile.h>
#include "entryTable.h"
#include "archiver.h"	// TMPTMP: For the ClassFile resolution.
#include "classTable.h"


/**************************************************
* Implementation: ClassEntry.
**************************************************/

ClassEntry::ClassEntry(unsigned int anID)
{
    clid= anID;
    nbrInstances= 0;
}


ClassEntry::Category ClassEntry::category(void)
{
    return simple;		// ATTN: Should be abstract.
}


unsigned int ClassEntry::usageCount(void)
{
    return nbrInstances;
}

/**************************************************
* Implementation: SimpleClass.
**************************************************/

SimpleClass::SimpleClass(ClassFile *aClass, unsigned int anID)
    : ClassEntry(anID)
{
    unsigned int strLength;
    char tmpName[512];

    source= aClass;
    aClass->getName(tmpName);
    strLength= strlen(tmpName) + 1;
    name= new char[strLength];
    memcpy(name, tmpName, strLength * sizeof(char));
}


ClassEntry::Category SimpleClass::category(void)
{
    return simple;
}


char *SimpleClass::getName(void)
{
    return name;
}


ClassFile *SimpleClass::getSource(void)
{
    return source;
}


/**************************************************
* Implementation: ArrayClass.
**************************************************/

ArrayClass::ArrayClass(unsigned int anID)
    : ClassEntry(anID)
{
    size= 0;
}


/**************************************************
* Implementation: BAClass.
**************************************************/

BAClass::BAClass(unsigned char aType, unsigned int aDimension, unsigned int anID)
    : ArrayClass(anID)
{
    basicType= aType;
    dimension= aDimension;
}


ClassEntry::Category BAClass::category(void)
{
    return basicArray;
}


bool BAClass::match(unsigned char aType, unsigned int aDimension)
{
    return ((basicType == aType) && (dimension == aDimension));
}


unsigned char BAClass::getType(void)
{
    return basicType;
}


unsigned int BAClass::getDimension(void)
{
    return dimension;
}


/**************************************************
* Implementation: IAClass.
**************************************************/

IAClass::IAClass(unsigned int aParentID, unsigned int anID)
    : ArrayClass(anID)
{
    sourceCLID= aParentID;
}


ClassEntry::Category IAClass::category(void)
{
    return instanceArray;
}


unsigned int IAClass::getSourceID(void)
{
    return sourceCLID;
}


/**************************************************
* Implementation: AAClass.
**************************************************/

AAClass::AAClass(ClassFile *aClass, unsigned int anID)
    : ArrayClass(anID)
{
// TODO.
}


ClassEntry::Category AAClass::category(void)
{
    return arrayArray;
}


/**************************************************
* Implementation: ClassTable.
**************************************************/

ClassTable::ClassTable(void)
{
    classes= new EntryTable();
    nbrClasses= 0;
}


ClassTable::~ClassTable(void)
{
    delete classes;
}


unsigned int ClassTable::getIdFor(ClassFile *aClass)
{
	// ATTN: TMPTMP (ne sont pas toutes des SimpleClass)
    SimpleClass *cursor;
    unsigned int i;

    for (cursor= NULL, i= 0; i < nbrClasses; i++) {
	// ATTN: TMPTMP (ne sont pas toutes des SimpleClass)
	if ((cursor= ((SimpleClass *)classes->elementAt(i)))->source == aClass) {
	    return cursor->clid;		// Warning: quick exit.
	}
    }

    return addClassReference(aClass);
}


void ClassTable::reset(void)
{
    delete classes;
    classes= new EntryTable();
    nbrClasses= 0;
}


unsigned int ClassTable::addClassReference(ClassFile *aClass)
{
    ClassEntry *newEntry, *cursor;
    unsigned int i;

    // Doit decider quel type de 'Class' il faut creer.
    if (aClass->isArrayClass()) {
	unsigned int nbrDim= ((ArrayClassFile *)aClass)->dimensions();

	if (((ArrayClassFile *)aClass)->rootIsBasicType()) {
	    unsigned char basicType= ((ArrayClassFile *)aClass)->getBasicType();
	    for (i= 0; i < nbrClasses; i++) {
		if ((cursor= (ClassEntry *)classes->elementAt(i))->category() != ClassEntry::basicArray) continue;
		else {
		    if (((BAClass *)cursor)->match(basicType, nbrDim)) break;
		}
	    }
	    if (i == nbrClasses) {
		nbrClasses++;
		newEntry= new BAClass(basicType, nbrDim, nbrClasses);
	    }
	    else {
		return cursor->clid;	// Warning: quick exit.
	    }
	}
	else {	    // Array of (possibly arrays of) 'SimpleClass'.
	    int arraySourceID;

		// First, find the underlying class.
	    for (i= 0; i < nbrClasses; i++) {
		if ((cursor= (ClassEntry *)classes->elementAt(i))->category() != ClassEntry::simple) continue;
		else {
			// ATTN: Big hack to find a ClassFile.  ArrayClassFile copy all
			// pointers from their underlying class, so at the moment it is
			// enough to pick one allocated object within a SimpleClass, like
			// the constant pool array.
			// ArrayClassFile have to be re-implemented with a reference to
			// the underlying ClassFile, instead of a field reproduction !!
		    if (((SimpleClass *)cursor)->getSource()->getConstantPool() == aClass->getConstantPool()) break;
		}
	    }
	    if (i == nbrClasses) {	// Must insert first the underlying class used by the array.
		nbrClasses++;

		    // TMP: Use temporary solution to get the ClassFile that was used to
		    // generate the ArrayClassFile.
		ClassFile *simpleSource= owner->tmpFetchSourceForArray((ArrayClassFile *)aClass);

		cursor= new SimpleClass(simpleSource, nbrClasses);
		classes->addElement(cursor);
		arraySourceID= nbrClasses;
	    }
	    else {
		arraySourceID= cursor->clid;
	    }

	    if (nbrDim == 1) {		// Array of SimpleClass.

		for (i= 0; i < nbrClasses; i++) {
		    if ((cursor= (ClassEntry *)classes->elementAt(i))->category() != ClassEntry::instanceArray) continue;
		    else {
			if (((IAClass *)cursor)->getSourceID() == arraySourceID) break;
		    }
		}
		
		if (i == nbrClasses) {
		    nbrClasses++;
		    newEntry= new IAClass(arraySourceID, nbrClasses);
		}
		else {
		    return cursor->clid;	// Warning: quick exit.
		}
	    }
	    else {				// Array of arrays.
		// TODO.
	    }
	}
    }
    else {
	for (i= 0; i < nbrClasses; i++) {
	    if ((cursor= (ClassEntry *)classes->elementAt(i))->category() != ClassEntry::simple) continue;
	    else {
		if (((SimpleClass *)cursor)->getSource() == aClass) break;
	    }
	}
	if (i == nbrClasses) {
	    nbrClasses++;
	    newEntry= new SimpleClass(aClass, nbrClasses);
	}
	else {
	    return cursor->clid;	// Warning: quick exit.
	}
    }

	// This is done if the class has not been found already.
    classes->addElement(newEntry);
    return nbrClasses;
}


unsigned int ClassTable::count(void)
{
    return nbrClasses;
}


ClassEntry *ClassTable::getClassAt(unsigned int anIndex)
{
    if (anIndex < nbrClasses) return (ClassEntry *)classes->elementAt(anIndex);
    else return NULL;
}


void ClassTable::setOwner(JstArchiver *anArchiver)
{
    owner= anArchiver;
}

#if defined(FUTURE_SOURCE)
// Construit le dictionnaire de champs pour une classe.
	    // Build signature (has to go in the ClassTable).
	classCursor= instanceClass;
	do {
		// ATTN: NEW METHOD.
	    if ((nbrFields= instanceClass->getCountFor(ClassFile::localInstanceFields)) != 0) { 		JVTypeDefinition *fieldType;
		unsigned int i, j;

		for (i= 0, j= 0; i < nbrFields; j++) {
		    if (machine->getFieldDescAt(j)->isStatic()) {
			continue;
		    }
			// ATTN: NEW METHODS.
		    machine->getFieldDescAt(j)->getTypeDefinition(&fieldType);
		    switch(fieldType->runType()) {
			case TYPE_BASIC_ARRAY:
			    break;
			case TYPE_INST_ARRAY:
			    break;
			case TYPE_ARRAY_ARRAY:
			    break;
			case TYPE_CLASS:
			    break;
			case TYPE_BASIC_32:
			case TYPE_BASIC_64H:
			    break;
		    }
		    i++;
		    delete fieldType;
		}
	    }
	    classCursor= classCursor->getSuperClass();
	} while (classCursor != jlObject);

#endif
