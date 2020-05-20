/**************************************************
* File: testArch.cc.
* Desc: Test module for the JSTArchiver system.
* Module: AkraLog : JSTManager.
* Rev: 26 aout 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <JstManager/fileArchiver.h>
#include <RunTime/jmachine.h>
#include <RunTime/jvcup.h>
#include <RunTime/jvInstance.h>


main(int argc, char **argv)
{
    JVInstance *anInstance;
    JstFileArchiver *archiver;
    JVCup *pzCup;

    if (argc < 2) {
	cout << "Util: tArch [ <options> ] <class-name>.\n";
	cout << "Options: none defined yet.\n";
	exit(0);
    }

    pzCup= new JVCup();

    if (pzCup->boot() == 0) {
	archiver= new JstFileArchiver("/tmp/archive.jst", pzCup->getDefaultMachine());

	if ((anInstance= pzCup->createInstance(argv[argc-1])) != NULL) {
	    archiver->startSession();
	    archiver->archive(anInstance);
	    archiver->endSession();
	}
	else {
	    cout << "Test: can't create an instance of class " << argv[argc-1] << ".\n";
	}
    }
    else {
	cout << "Test: Can't boot.\n";
    }

}


#if defined(IRIX) || defined(NeXT)
#include <pwd.h>
#endif
#include <RunTime/stringSupport.h>

void fakeSymbols(void)
{
    getpwuid(getuid());
    gettimeofday(NULL, NULL);
    StringSupport::mapStringToNarrowArray(NULL, NULL);
}
