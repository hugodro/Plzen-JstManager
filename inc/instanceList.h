#ifndef _INSTANCE_LIST_H_
#define _INSTANCE_LIST_H_
/**************************************************
* File: instanceList.h.
* Desc: Definition of the InstanceList class.
* Module: AkraLog : JSTManager.
* Rev: 25 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class InstanceHolder : public AkObject {
    friend class InstanceList;
  protected:
    InstanceHolder *next;
    void *instance;
    unsigned int instanceID;
    unsigned int classID;
    unsigned char type;

  public:
    InstanceHolder(void *anInstance, unsigned int anID, unsigned int aClid, unsigned char aType);
    unsigned char getType(void);
    void *content(void);
    unsigned int getCLID(void);
    unsigned int getContentID(void);
};


class InstanceList : public AkObject {
 public:
   enum Types {
	instance, arrayBasic, arrayInstance, arrayOfArray
   };

 protected:	// Instance variables.
    unsigned int nbrInstances;
    InstanceHolder *first, *last;

 public:		// Instance methods.
    InstanceList(void);
    virtual void empty(void);
    virtual void addElement(void *anInstance, unsigned int anID, unsigned int aClid, Types instanceType);
    virtual InstanceHolder *popElement(void);
    virtual unsigned int count(void);
};


#endif	/* _INSTANCE_LIST_H_ */
