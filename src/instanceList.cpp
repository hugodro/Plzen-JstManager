/**************************************************
* File: instanceList.cc.
* Desc: Implementation of the InstanceList class.
* Module: AkraLog : JSTManager.
* Rev: 25 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "instanceList.h"


/**************************************************
* Implementation: InstanceHolder.
**************************************************/

InstanceHolder::InstanceHolder(void *anInstance, unsigned int anID, unsigned int aClid, unsigned char aType)
{
    next= NULL;
    instance= anInstance;
    instanceID= anID;
    classID= aClid;
    type= aType;
}


unsigned char InstanceHolder::getType(void)
{
    return type;
}


void *InstanceHolder::content(void)
{
    return instance;
}


unsigned int InstanceHolder::getCLID(void)
{
    return classID;
}


unsigned int InstanceHolder::getContentID(void)
{
    return instanceID;
}


/**************************************************
* Implementation: InstanceList.
**************************************************/

InstanceList::InstanceList(void)
{
    nbrInstances= 0;
    first= last= NULL;
}


void InstanceList::empty(void)
{
    InstanceHolder *cursor= first, *tmpNext;

    while (cursor != NULL) {
	tmpNext= cursor->next;
	delete cursor;
	cursor= tmpNext;
    }
    nbrInstances= 0;
}


void InstanceList::addElement(void *anInstance, unsigned int anID, unsigned int aClid, Types instanceType)
{
    InstanceHolder *newElement;

    newElement= new InstanceHolder(anInstance, anID, aClid, instanceType);
    if (first == NULL) {
	first= newElement;
    }
    else {
	last->next= newElement;
    }
    last= newElement;
    nbrInstances++;
}


InstanceHolder *InstanceList::popElement(void)
{
    InstanceHolder *result;

    result= first;
    if (first != NULL) {
	first= first->next;
	if (first == NULL) last= NULL;
	nbrInstances--;
    }
    return result;
}


unsigned int InstanceList::count(void)
{
    return nbrInstances;
}

