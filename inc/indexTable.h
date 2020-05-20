#ifndef _INDEX_TABLE_H_
#define _INDEX_TABLE_H_
/**************************************************
* File: indexTable.h.
* Desc: Definition of the IndexTable class.
* Module: AkraLog : JSTManager.
* Rev: 25 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class EntryTable;


class IndexTable : public AkObject {
 protected:	// Instance variables.
    EntryTable *indexes;
    unsigned int nbrIndexes;

 public:		// Instance methods.
    IndexTable(void);
    virtual void reset(void);
    virtual bool indexForID(void *anID, unsigned int *dest);
};


#endif	/* _INDEX_TABLE_H_ */
