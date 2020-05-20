#ifndef _BLOCKED_TABLE_H_
#define _BLOCKED_TABLE_H_
/**************************************************
* File: entryTable.h.
* Desc: Definition of the EntryTable class.
* Module: AkraLog : JSTManager.
* Rev: 26 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>

typedef unsigned int ClassSizeArray[2];

class EntryTable : public AkObject {
  private:		// Class variables.
    static unsigned int nbrSubClasses;	// Nbr of registred sub-classes.
/* Should be this:
    static ClassSizeArray *classSizes;		// Array of table sizes, for subclasses.
*/
    static ClassSizeArray classSizes[1];		// Array of table sizes, for subclasses.

  public:		// Class methods.
    static void registerSubClass(void *classID, unsigned int tableSize);

  protected:	// Instance variables.
    EntryTable *next;
    void **elements;
    unsigned int nbrElements;

  public:		// Instance methods.
    EntryTable(void);
    EntryTable(EntryTable *aClonee);
    virtual ~EntryTable(void);
    virtual void addElement(void *anElement);
    virtual void *elementAt(unsigned int anIndex);
};


#endif	/* _BLOCKED_TABLE_H_ */

