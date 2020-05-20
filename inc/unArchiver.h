#ifndef _JST_UNARCHIVER_H_
#define _JST_UNARCHIVER_H_
/**************************************************
* File: unArchiver.h.
* Desc: Definition of the JstUnarchiver class.
* Module: AkraLog : JSTManager.
* Rev: 23 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class JVInstance;
class JVMachine;


class JstUnarchiver : public AkObject {
  protected:		// Instance variables.
    unsigned int nbrRootInstances;
    unsigned int nbrInstances;
    unsigned int nbrReferredClasses;
    ClassTable *classes;
    JVMachine *machine;

  public:			// Instance methods.
    JstUnarchiver(void);
    virtual ~JstUnarchiver(void);
    bool loadArchive(void);
    unsigned int getNbrObjects(void);
    unsigned int getTotalNbrObjects(void);
    JVInstance *getObject(unsigned int anIndex);
};



#endif		/* _JST_UNARCHIVER_H_ */
