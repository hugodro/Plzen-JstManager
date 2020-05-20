/**************************************************
* File: fileArchiver.cc.
* Desc: Implementation of the JstFileArchiver class.
* Module: AkraLog : JSTManager.
* Rev: 25 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <fstream.h>
#include <RunTime/math64bits.h>
#include "fileArchiver.h"


unsigned int JstFileArchiver::version= 1;


JstFileArchiver::JstFileArchiver(JVMachine *aMachine)
    : JstArchiver(aMachine)
{
    output= NULL;
    fileName= NULL;
}


JstFileArchiver::JstFileArchiver(char *aDestName, JVMachine *aMachine)
    : JstArchiver(aMachine)
{
    unsigned int strLength;

    output= NULL;
    strLength= strlen(aDestName)+1;
    fileName= new char[strLength];
    memcpy(fileName, aDestName, strLength * sizeof(char));
}


JstFileArchiver::~JstFileArchiver(void)
{
    if (fileName != NULL) delete fileName;
    if (output != NULL) delete output;
}


void JstFileArchiver::useFile(char *aDestName)
{
    unsigned int strLength;

    if (fileName != NULL) delete fileName;
    strLength= strlen(aDestName)+1;
    fileName= new char[strLength];
    memcpy(fileName, aDestName, strLength * sizeof(char));
}


bool JstFileArchiver::startSession(void)
{
    if (JstArchiver::startSession()) {
	if (output != NULL) delete output;
#if defined(_WIN32)
	output= new ofstream(fileName, ios::binary);
#else
	output= new ofstream(fileName);
#endif
	if (output->good()) {
	    writeHeader();
	}
	else return false;
    }
    else return false;
}


bool JstFileArchiver::endSession(void)
{
    if (JstArchiver::endSession()) {
	output->close();
	return output->good();
    }
    else return false;
}


void JstFileArchiver::writeHeader(void)
{
    writeInteger(version);
    writeInteger(0);		// Offset to Class dictionnary.
    writeInteger(0);		// Nbr of objects in archive.
}


void JstFileArchiver::writeInteger(unsigned int anInt)
{
    unsigned char buffer[4];	// ATTN-980522 [HD]: Not multi-threadable.

    buffer[3]= (unsigned char)(anInt & 0x0FF);
    buffer[2]= (unsigned char)((anInt >> 8) & 0x0FF);
    buffer[1]= (unsigned char)((anInt >> 16) & 0x0FF);
    buffer[0]= (unsigned char)((anInt >> 24) & 0x0FF);
    output->write(buffer, 4);
}


void JstFileArchiver::encodeBoolean(bool aValue)
{
    unsigned char buffer= (unsigned char)(aValue & 0x01);

    output->write(&buffer, 1);
}

void JstFileArchiver::encodeByte(byte aValue)
{
    unsigned char buffer= (unsigned char)aValue;

    output->write(&buffer, 1);
}

void JstFileArchiver::encodeChar(char aValue)
{
    output->write(&aValue, 1);
}

void JstFileArchiver::encodeShort(short aValue)
{
    unsigned char buffer[2];

    buffer[1]= (unsigned char)(aValue & 0x0FF);
    buffer[0]= (unsigned char)((aValue >> 8) & 0x0FF);
    output->write(&buffer, 2);
}

void JstFileArchiver::encodeFloat(float aValue)
{
    unsigned int intPtr;
    float *tmpFloat;

    tmpFloat= &aValue; intPtr= (int)tmpFloat;
    writeInteger(*(int *)intPtr);
}

void JstFileArchiver::encodeInteger(int aValue)
{
    writeInteger(aValue);
}

void JstFileArchiver::encodeDouble(Value64Bits *aValue)
{
    writeInteger(aValue->data.lint.highWord);
    writeInteger(aValue->data.lint.lowWord);
}

void JstFileArchiver::encodeLong(Value64Bits *aValue)
{
    writeInteger(aValue->data.lint.highWord);
    writeInteger(aValue->data.lint.lowWord);
}


bool JstFileArchiver::writeClassDictionnary(ClassTable *aTable)
{
    streampos currentPosition;

    currentPosition= output->tellp();
    output->seekp(sizeof(int));			// Go back just past version ID.
    writeInteger(currentPosition - (2 * sizeof(int)));
    writeInteger(nbrInstances);
    output->seekp(currentPosition);
    JstArchiver::writeClassDictionnary(aTable);
}
