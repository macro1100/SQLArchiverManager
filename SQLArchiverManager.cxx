#include <SQLArchiverManager.hxx>
#include <SQLArchiverResources.hxx>
#include <HotLinkWaitForAnswer.hxx>   // include/Manager
#include <StartDpInitSysMsg.hxx>      // include/Messages
#include <DpMsgAnswer.hxx>            // include/Messages
#include <DpMsgHotLink.hxx>           // include/Messages
#include <DpHLGroup.hxx>              // include/Basics
#include <DpVCItem.hxx>               // include/Basics
#include <ErrHdl.hxx>                 // include/Basics
#include <ErrClass.hxx>               // include/Basics
#include <signal.h>
#include "sync/init.h"

// -------------------------------------------------------------------------
// Our manager class

PVSSboolean SQLArchiverManager::doExit = PVSS_FALSE;

//--------------------------------------------------------------------------------
// The constructor defines manager type (API_MAN) and manager number

SQLArchiverManager::SQLArchiverManager()
           : OaManager(ManagerIdentifier(API_MAN, SQLArchiverResources::getManNum()))
{
}

//--------------------------------------------------------------------------------
// Run our demo manager

void SQLArchiverManager::run()
{
  long sec, usec;

  // First connect to Data manager.
  // We want Typecontainer and Identification so we can resolve names
  // This call succeeds or the manager will exit
  connectToData(StartDpInitSysMsg::TYPE_CONTAINER | StartDpInitSysMsg::DP_IDENTIFICATION);

  // While we are in STATE_INIT we are initialized by the Data manager
  while (getManagerState() == STATE_INIT)
  {
    // Wait max. 1 second in select to receive next message from data.
    // It won't take that long...
    sec = 1;
    usec = 0;
    dispatch(sec, usec);
  }

  // We are now in STATE_ADJUST and can connect to Event manager
  // This call will succeed or the manager will exit
  connectToEvent();

  // We are now in STATE_RUNNING. 
  sync::init();
  // Now loop until we are finished
  while ( 1 )
  {
    // Exit flag set ?
    if (doExit)
      return;

    // Wait 10 ms
    sec = 0;
    usec = 10000;
    dispatch(sec, usec);
  }
}

//--------------------------------------------------------------------------------
// Receive Signals.
// We are interested in SIGINT and SIGTERM.

void SQLArchiverManager::signalHandler(int sig)
{
  if ( (sig == SIGINT) || (sig == SIGTERM) )
    SQLArchiverManager::doExit = PVSS_TRUE;
  else
    OaManager::signalHandler(sig);
}

//--------------------------------------------------------------------------------
