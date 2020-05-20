#ifndef _JST_FILE_ARCHIVER_H_
#define _JST_FILE_ARCHIVER_H_
/**************************************************
* File: fileArchiver.h.
* Desc: Definition of the JstFileArchiver class.
* Module: AkraLog : JSTManager.
* Rev: 25 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <fstream>

#include "archiver.h"


class JstFileArchiver : public JstArchiver {
  protected:		// Class variables.
    static unsigned int version;

  protected:		// Instance variables.
    std::ofstream *output;
    char *fileName;

  public:			// Instance methods.
    JstFileArchiver(JVMachine *aMachine);
    JstFileArchiver(char *aDestName, JVMachine *aMachine);
    virtual ~JstFileArchiver(void);
    virtual void useFile(char *aDestName);
    virtual bool startSession(void);
    virtual bool endSession(void);
    virtual void writeHeader(void);
    void writeInteger(unsigned int anInt);
    virtual void encodeBoolean(bool aValue);
    virtual void encodeByte(byte aValue);
    virtual void encodeChar(char aValue);
    virtual void encodeShort(short aValue);
    virtual void encodeFloat(float aValue);
    virtual void encodeInteger(int aValue);
    virtual void encodeDouble(Value64Bits *aValue);
    virtual void encodeLong(Value64Bits *aValue);
    virtual bool writeClassDictionnary(ClassTable *aTable);
};


#endif	/* _JST_FILE_ARCHIVER_H_ */
