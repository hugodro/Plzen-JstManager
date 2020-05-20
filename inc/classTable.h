#ifndef _CLASS_TABLE_H_
#define _CLASS_TABLE_H_
/**************************************************
* File: classTable.h.
* Desc: Definition of the ClassTable class.
* Module: AkraLog : JSTManager.
* Rev: 24 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class ClassFile;
class HashTable;
class EntryTable;		// Replace the hash table, while it is being developed.


class ClassEntry : public AkObject {
    friend class ClassTable;
  public:
    enum Category {
	simple, basicArray, instanceArray, arrayArray
    };

  public:	// Instance variables.
    int fieldDictionnary;		// TODO.
    unsigned int clid;
    unsigned int nbrInstances;

  public:		// Instance methods.
   ClassEntry(unsigned int anID);
   virtual Category category(void);	// ATTN: Should be abstract.
   virtual unsigned int usageCount(void);
};


class SimpleClass : public ClassEntry {
 public:	// Instance variables.
    ClassFile *source;
    char *name;

 public:		// Instance methods.
   SimpleClass(ClassFile *aClass, unsigned int anID);
   virtual Category category(void);
   virtual char *getName(void);
   virtual ClassFile *getSource(void);
};


class ArrayClass : public ClassEntry {
 protected:	// Instance variables.
    unsigned int size;		// If not 0, then it is a 'fixed array'.

 public:		// Instance methods.
    ArrayClass(unsigned int anID);
};


class BAClass : public ArrayClass {
 protected:	// Instance variables.
    unsigned char basicType;
    unsigned int dimension;

 public:		// Instance methods.
   BAClass(unsigned char aType, unsigned int aDimension, unsigned int anID);
   virtual Category category(void);
   virtual bool match(unsigned char aType, unsigned int aDimension);
   virtual unsigned char getType(void);
   virtual unsigned int getDimension(void);
};


class IAClass : public ArrayClass {
 protected:	// Instance variables.
    int sourceCLID;

 public:		// Instance methods.
   IAClass(unsigned int aParentID, unsigned int anID);
   virtual Category category(void);
   virtual unsigned int getSourceID(void);
};


class AAClass : public ArrayClass {
 protected:	// Instance variables.
    ArrayClass *source;

 public:		// Instance methods.
   AAClass(ClassFile *aClass, unsigned int anID);
   virtual Category category(void);
};


class JstArchiver;		// TMP: Needed by the link in ClassTable.

class ClassTable : public AkObject {
  protected:	// Instance variables.
//    HashTable *classes;
    unsigned int nbrClasses;
    EntryTable *classes;
    JstArchiver *owner;	// TMP: Link to the owner, so it can fetch SimpleClass from ArrayClasses.

  public:		// Instance methods.
    ClassTable(void);
    virtual ~ClassTable(void);
    virtual unsigned int getIdFor(ClassFile *aClass);
    virtual void reset(void);
    virtual unsigned int addClassReference(ClassFile *aClass);
    virtual unsigned int count(void);
    virtual ClassEntry *getClassAt(unsigned int anIndex);
    void setOwner(JstArchiver *anArchiver);		// TMPTMP: For resolution of unknow ClassFile used in ArrayClassFile.
};


#endif	/* _CLASS_TABLE_H_ */
