/**************************************************
* File: archiver.cc.
* Desc: Implementation of the JstArchiver class.
* Module: AkraLog : JST.
* Rev: 23 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <RunTime/jvInstance.h>
#include <RunTime/classFile.h>
#include <RunTime/valueTypes.h>
#include <RunTime/jmachine.h>
#include <RunTime/machineSupport.h>
#include <RunTime/math64bits.h>
#include "classTable.h"
#include "indexTable.h"
#include "instanceList.h"
#include "archiver.h"

JstArchiver::JstArchiver(JVMachine *aMachine)
{
    nbrRootInstances= 0;
    nbrInstances= 0;
    nbrReferredClasses= 0;
    classes= new ClassTable();
    classes->setOwner(this);		// TMP: Link back so that the machine can go fetch ClassFile used within ArrayClassFile.
    indexes= new IndexTable();
    forwardReferences= new InstanceList();
    machine= aMachine;
}


JstArchiver::~JstArchiver(void)
{
    delete forwardReferences;
    delete indexes;
    delete classes;
}


bool JstArchiver::startSession(void)
{
    classes->reset();
    indexes->reset();
    forwardReferences->empty();
	return true;
}


bool JstArchiver::archive(JVInstance *anObject)
{
    unsigned int objIndex;
    bool result= true;

	// ATTN: Should the 'null' object be included in the archive as root objet ?
	//	For now, just refuse it.
    if (anObject == JVInstance::globalNullObject()) return false;

    if (!indexes->indexForID(anObject, &objIndex)) {
	ClassFile *instanceClass;
	ClassFile *jlObject= JVInstance::globalNullObject()->category();
	unsigned int classID;

	if ((instanceClass= anObject->category()) == jlObject) {
	    classID= 0;
	}
	else {
	    classID= classes->addClassReference(instanceClass);
	}

	if (anObject->vectorialType() == TYPE_INSTANCE) {
	    result= archiveInstance(anObject, instanceClass, classID, objIndex);
	}
	else {
	    switch(anObject->vectorialType() & 0x0380) {        // ATTN: Besoin d'une cte.
		case TYPE_BASIC_ARRAY:
		    result= archiveBasicArray((JVBasicArray *)anObject, classID, objIndex);
		    break;
		case TYPE_INST_ARRAY:
		    result= archiveInstanceArray((JVInstanceArray *)anObject, classID, objIndex);
		    break;
		case TYPE_ARRAY_ARRAY:
		    result= archiveArrayArray((JVArrayArray *)anObject, classID, objIndex);
		    break;
		default:		// Should never happen.
		    // TODO: Indicate an internal error.
		    break;
	    }
	}
	while (forwardReferences->count() > 0) {
	    InstanceHolder *element;
	    JVAbstractArray *anArray;

	    element= forwardReferences->popElement();
	    switch(element->getType()) {
		case InstanceList::instance:
		    anObject= (JVInstance *)element->content();
		    archiveInstance(anObject, anObject->category(), element->getCLID(), element->getContentID());
		    break;
		case InstanceList::arrayBasic:
		    anArray= (JVBasicArray *)element->content();
		    archiveBasicArray((JVBasicArray *)anArray, element->getCLID(), element->getContentID());
		    break;
		case InstanceList::arrayInstance:
		    anArray= (JVInstanceArray *)element->content();
		    archiveInstanceArray((JVInstanceArray *)anArray, element->getCLID(), element->getContentID());
		    break;
		case InstanceList::arrayOfArray:
		    break;
	    }
	}
    }

    return result;

}


bool JstArchiver::archive(JVInstance *anObject, char **fieldList, unsigned int nbrFields)
{
// TODO.
}


unsigned int JstArchiver::getNbrObjects(void)
{
// TODO.
}


unsigned int JstArchiver::getTotalNbrObjects(void)
{
// TODO.
}


bool JstArchiver::endSession(void)
{
    return writeClassDictionnary(classes);
}


// ATTN: Should these encoders do anything ?
void JstArchiver::encodeBoolean(bool aValue)
{
}

void JstArchiver::encodeByte(byte aValue)
{
}

void JstArchiver::encodeChar(char aValue)
{
}

void JstArchiver::encodeShort(short aValue)
{
}

void JstArchiver::encodeFloat(float aValue)
{
}

void JstArchiver::encodeInteger(int aValue)
{
}

void JstArchiver::encodeDouble(Value64Bits *aValue)
{
}

void JstArchiver::encodeLong(Value64Bits *aValue)
{
}


bool JstArchiver::writeObjectSignature(JVAbstractInstance *anInstance, unsigned int anID, int aClassID)
{
// ATTN: Is this enough ?
    writeClassReference(aClassID);

    return true;		// For now, always succeed.
}


bool JstArchiver::writeClassReference(unsigned int aClassID)
{
// ATTN: Doit etre refait pour assurer l'ordre des bytes, pour reconnaissance en lecture.
    if (aClassID < 128) {
	encodeByte(aClassID);
    }
    else if (aClassID < 16384) {
	encodeShort(0x080000 | aClassID);
    }
    else if (aClassID < 1073741824) {
	encodeInteger(0x0C0000000 | aClassID);
    }
    else
    	return false;

    return true;
}


bool JstArchiver::archiveInstance(JVInstance *anInstance, ClassFile *instanceClass, unsigned int classID, int objIndex)
{
    ClassFile *classCursor;
    ClassFile *jlObject= JVInstance::globalNullObject()->category();
    JVInstance *nullObject= JVInstance::globalNullObject();
    JVBasicArray *basicArray;
    JVInstanceArray *instanceArray;
    JVArrayArray *arrayArray;
    unsigned int nbrFields;
    bool result= true;

	// Archive object.
    writeObjectSignature(anInstance, objIndex, classID);
    classCursor= instanceClass;
    while (classCursor != jlObject) {
	if ((nbrFields= classCursor->fieldCount) != 0) {    // ATTN: Besoin de nvlle meth.
	    JVTypeDefinition *fieldType;
	    JVVariable *field;
	    JVInstance *fieldValue;
	    unsigned int i, j, referenceID;

	    for (i= 0, j= 0; j < nbrFields; j++) {
		if (classCursor->fields[j].isStatic()) {
		    continue;
		}
		    // ATTN: Besoin de nouvelles methodes.
		field= classCursor->fields[j].resolve(machine->getClassManager());
		switch(field->detailedType()) {
		    case TYPE_BASIC_ARRAY:
			basicArray= (JVBasicArray *)anInstance->getField(field);
			if ((basicArray == nullObject) || (basicArray == NULL)) {
			    writeClassReference(0);
			    encodeInteger(0);
			}
			else {
			    int fwdClassID;

/* ATTN: Comment ca se gere ?
			    fwdClassID= class->addClassReference(field->);
*/
			    if (!indexes->indexForID(basicArray, &referenceID)) {
				forwardReferences->addElement(basicArray, referenceID, fwdClassID, InstanceList::arrayBasic);
			    }
			    writeClassReference(fwdClassID);
			    encodeInteger(referenceID);
			}
			break;
		    case TYPE_INST_ARRAY:
			instanceArray= (JVInstanceArray *)anInstance->getField(field);
			if ((instanceArray == nullObject) || (instanceArray == NULL)) {
			    writeClassReference(0);
			    encodeInteger(0);
			}
			else {
			    int fwdClassID;

			    fwdClassID= classes->addClassReference(instanceArray->elementType());
			    if (!indexes->indexForID(instanceArray, &referenceID)) {
				forwardReferences->addElement(instanceArray, referenceID, fwdClassID, InstanceList::arrayInstance);
			    }
			    writeClassReference(fwdClassID);
			    encodeInteger(referenceID);
			}
			break;
		    case TYPE_ARRAY_ARRAY:
			arrayArray= (JVArrayArray *)anInstance->getField(field);
			if ((arrayArray == nullObject) || (arrayArray == NULL)) {
			    writeClassReference(0);
			    encodeInteger(0);
			}
			break;
		    case TYPE_CLASS:
			fieldValue= (JVInstance *)anInstance->getField(field);
			if ((fieldValue == nullObject) || (fieldValue == NULL)) {
			    writeClassReference(0);
			    encodeInteger(0);
			}
			else {
			    // Il faut ajouter la classe de l'objet a la liste de classe utilisee.
			    // Il faut ajouter l'objet au 'forwarders' s'il est inconnu.
			    if (!indexes->indexForID(fieldValue, &referenceID)) {
				if (fieldValue->category() == jlObject) {
				    classID= 0;
				}
				else {
				    classID= classes->addClassReference(fieldValue->category());
				}
				forwardReferences->addElement(fieldValue, referenceID, classID, InstanceList::instance);
			    }
			    writeClassReference(classID);
			    encodeInteger(referenceID);
			}
			break;
		    case TYPE_BASIC_32:
		    case TYPE_BASIC_64H:
			int *tmpInt;
			float *tmpFloat;
			switch(((JVBasicTypeDef *)field->isA)->type()) {
			    case JVM_TYPE_BOOLEAN:
				encodeBoolean((bool)anInstance->getField(field));
				break;
			    case JVM_TYPE_SIGNED_BYTE:
				encodeByte((byte)anInstance->getField(field));
				break;
			    case JVM_TYPE_CHAR:
				encodeChar((char)anInstance->getField(field));
				break;
			    case JVM_TYPE_UNSIGNED_SHORT:
				encodeShort((short)anInstance->getField(field));
				break;
			    case JVM_TYPE_INTEGER:
				encodeInteger((int)anInstance->getField(field));
				break;
			    case JVM_TYPE_FLOAT:
				tmpInt= (int *)anInstance->getField(field);
				tmpFloat= (float *)&tmpInt;
				encodeFloat(*tmpFloat);
				break;
			    case JVM_TYPE_DOUBLE:
				encodeDouble((Value64Bits *)anInstance->getField(field));
				break;
			    case JVM_TYPE_LONG:
				encodeLong((Value64Bits *)anInstance->getField(field));
				break;
			}
			break;
		}
		i++;
	    }
	}
	classCursor= classCursor->getSuperClass();
    }
    return result;
}


bool JstArchiver::archiveBasicArray(JVBasicArray *anArray, unsigned int classID, unsigned int objIndex)
{
    unsigned int nbrElements, i, *tmpIntPtr;

    writeObjectSignature(anArray, objIndex, classID);
    nbrElements= anArray->nbrElements;	// ATTN: Besoin d'une nouvelle methode.
    encodeInteger(anArray->nbrElements);
    switch(anArray->elementType()) {
	case JVM_TYPE_SIGNED_BYTE:
	    for (i= 0; i < nbrElements; i++) {
		encodeByte((byte)anArray->getElement(i));
	    }
	    break;
	case JVM_TYPE_CHAR:
	    for (i= 0; i < nbrElements; i++) {
		encodeChar((char)anArray->getElement(i));
	    }
	    break;
	case JVM_TYPE_BOOLEAN:
	    for (i= 0; i < nbrElements; i++) {
		encodeBoolean((bool)anArray->getElement(i));
	    }
	    break;
	case JVM_TYPE_UNSIGNED_SHORT:
	    for (i= 0; i < nbrElements; i++) {
		encodeShort((short)anArray->getElement(i));
	    }
	    break;
	case JVM_TYPE_INTEGER:
	    for (i= 0; i < nbrElements; i++) {
		encodeInteger((int)anArray->getElement(i));
	    }
	    break;
	case JVM_TYPE_FLOAT:
	    for (i= 0; i < nbrElements; i++) {
		tmpIntPtr= (int *)anArray->getElement(i);
		encodeFloat(*(float *)(int *)&tmpIntPtr);
	    }
	    break;
	case JVM_TYPE_LONG:
	    for (i= 0; i < nbrElements; i++) {
		encodeLong((Value64Bits *)anArray->getElement(i));
	    }
	    break;
	case JVM_TYPE_DOUBLE:
	    for (i= 0; i < nbrElements; i++) {
		encodeDouble((Value64Bits *)anArray->getElement(i));
	    }
	    break;
    }
}


bool JstArchiver::archiveInstanceArray(JVInstanceArray *anArray, unsigned int clid, unsigned int objIndex)
{
    JVInstance *element, *nullObject= JVInstance::globalNullObject();
    ClassFile *jlObject= JVInstance::globalNullObject()->category();
    unsigned int nbrElements, classID, referenceID, i;

    writeObjectSignature(anArray, objIndex, classID);
    nbrElements= anArray->nbrElements;	// ATTN: Besoin d'une nouvelle methode.
    encodeInteger(anArray->nbrElements);
    for (i= 0; i < nbrElements; i++) {
	element= anArray->getElement(i);
	if ((element == nullObject) || (element == NULL)) {
	    writeClassReference(0);
	    encodeInteger(0);
	}
	else {
	    if (!indexes->indexForID(element, &referenceID)) {
		if (element->category() == jlObject) {
		    classID= 0;
		}
		else {
		    classID= classes->addClassReference(element->category());
		}
		forwardReferences->addElement(element, referenceID, classID, InstanceList::instance);
	    }
	    writeClassReference(classID);
	    encodeInteger(referenceID);
	}
    }
}


bool JstArchiver::archiveArrayArray(JVArrayArray *anArray, unsigned int clid, unsigned int objIndex)
{

}


bool JstArchiver::writeClassDictionnary(ClassTable *aTable)
{
    unsigned int nbrClasses;

    nbrClasses= classes->count();
    encodeInteger(nbrClasses);

    if (nbrClasses > 0) {
	ClassEntry *cursor;
	unsigned int i;

	for (i= 0; i < nbrClasses; i++) {
	    cursor= classes->getClassAt(i);
	    if (!encodeClass(cursor)) return false;    // Warning: quick exit.
	}
    }

    return true;
}


bool JstArchiver::encodeClass(ClassEntry *aClass)
{
    unsigned int nameLength, i;
    char *className;

    switch(aClass->category()) {
	case ClassEntry::simple:
	    encodeByte(simpleClass);
	    className= ((SimpleClass *)aClass)->getName();
	    nameLength= strlen(className);
	    encodeInteger(nameLength);
		// ATTN: Need a faster method.
	    for (i= 0; i < nameLength; i++) {
		encodeChar(className[i]);
	    }
	    // Must encode here the field dictionnary.
	    break;
	case ClassEntry::basicArray:
	    encodeByte(basicArray);	// ATTN: Must implement fixed array modifier.
	    encodeByte(((BAClass *)aClass)->getType());
	    encodeInteger(((BAClass *)aClass)->getDimension());
	    break;
	case ClassEntry::instanceArray:
	    encodeByte(instanceArray);	// ATTN: Must implement fixed array modifier.
	    encodeInteger(((IAClass *)aClass)->getSourceID());
	    break;
	case ClassEntry::arrayArray:
	    break;
    }
    encodeInteger(aClass->usageCount());
}


ClassFile *JstArchiver::tmpFetchSourceForArray(ArrayClassFile *arrayClassFile)
{
    char tmpName[512];

    arrayClassFile->getName(tmpName);
    return machine->getClassFile(tmpName);
}


#if 0
	if ((nbrFields= classCursor->getCountFor(ClassFile::localInstanceFields)) != 0) { 		JVTypeDefinition *fieldType;
	    unsigned int i, j;

	    for (i= 0, j= 0; i < nbrFields; j++) {
		if (machine->getFieldDescAt(j)->isStatic()) {
		    continue;
		}
		    // ATTN: NEW METHODS.
		machine->getFieldDescAt(j)->getTypeDefinition(&fieldType);
		switch(fieldType->runType()) {
		}
		i++;
		delete fieldType;
	    }
	}
#endif
