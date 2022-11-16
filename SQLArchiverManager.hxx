// Declaration of our SQLArchiverManager-class
#ifndef  DEMOMANAGER_H
#define  DEMOMANAGER_H

#include   <OaManager.hxx>
#include   <DpIdentifier.hxx>

class SQLArchiverManager : public OaManager
{
  public:
    // Default constructor
    SQLArchiverManager();

    // Run the manager
    void run();

  private:
    // our exit flag. The signal handler will set it to PVSS_TRUE
    static PVSSboolean doExit;

    // callback from signal handler
    virtual void signalHandler(int sig);
};

#endif
