#ifndef _JST_ARCHIVER_H_
#define _JST_ARCHIVER_H_
/**************************************************
* File: archiver.h.
* Desc: Definition of the JstArchiver class.
* Module: AkraLog : JSTManager.
* Rev: 23 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>

#if defined(NeXT) || defined(__GNUC__)
typedef unsigned char byte;
#endif

class JVInstance;
class JVAbstractInstance;
class ClassFile;
class ClassTable;
class IndexTable;
class InstanceList;
class JVMachine;
class Value64Bits;
class JVBasicArray;
class JVInstanceArray;
class JVArrayArray;
class ClassEntry;
class ArrayClassFile;		// TMPTMP.


class JstArchiver : public AkObject {
  public:
    enum ClassIndicators {	// 8 bits, bit 0-2: kind, bit 7: array modifier.
	simpleClass= 0, basicArray= 1, instanceArray= 2, arrayArray= 3
	, fixedDim= (1 << 7)
    };

  protected:		// Instance variables.
    unsigned int nbrRootInstances;
    unsigned int nbrInstances;
    unsigned int nbrReferredClasses;
    ClassTable *classes;
    IndexTable *indexes;
    InstanceList *forwardReferences;
    JVMachine *machine;

  public:			// Instance methods.
    JstArchiver(JVMachine *aMachine);
    virtual ~JstArchiver(void);
    virtual bool startSession(void);
    virtual bool archive(JVInstance *anObject);
    virtual bool archive(JVInstance *anObject, char **fieldList, unsigned int nbrFields);
    virtual unsigned int getNbrObjects(void);
    virtual unsigned int getTotalNbrObjects(void);
    virtual bool endSession(void);
    virtual void encodeBoolean(bool aValue);
    virtual void encodeByte(byte aValue);
    virtual void encodeChar(char aValue);
    virtual void encodeShort(short aValue);
    virtual void encodeFloat(float aValue);
    virtual void encodeInteger(int aValue);
    virtual void encodeDouble(Value64Bits *aValue);
    virtual void encodeLong(Value64Bits *aValue);
    virtual bool writeObjectSignature(JVAbstractInstance *anInstance, unsigned int anID, int aClassID); 
    virtual bool writeClassReference(unsigned int aClassID);
    virtual bool archiveInstance(JVInstance *anInstance, ClassFile *instanceClass, unsigned int classID, int objIndex);
    virtual bool archiveBasicArray(JVBasicArray *anArray, unsigned int clid, unsigned int objIndex);
    virtual bool archiveInstanceArray(JVInstanceArray *anArray, unsigned int clid, unsigned int objIndex);
    virtual bool archiveArrayArray(JVArrayArray *anArray, unsigned int clid, unsigned int objIndex);
    virtual bool writeClassDictionnary(ClassTable *aTable);
    virtual bool encodeClass(ClassEntry *aClass);

  public:
    ClassFile *tmpFetchSourceForArray(ArrayClassFile *arrayClassFile);
};



#endif		/* _JST_ARCHIVER_H_ */
