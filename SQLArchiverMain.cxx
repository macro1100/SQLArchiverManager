/*
  Example of an API manager
  This will copy the values from one datapoint to another.
  To do this the manager will connect to the first datapoint and
  for every hotlink it receives it will set the second one.
  The names of both datapoints can be given in the config file.
*/

#include  <SQLArchiverManager.hxx>
#include  <SQLArchiverResources.hxx>
#include  <signal.h>

int main(int argc, char *argv[])
{
  // Let Manager handle SIGINT and SIGTERM (Ctrl+C, kill)
  // Manager::sigHdl will call virtual function Manager::signalHandler
  signal(SIGINT, SQLArchiverManager::sigHdl);
  signal(SIGTERM, SQLArchiverManager::sigHdl);

  // Initialize resources, i.e.
  //  - interpret commandline arguments
  //  - interpret config file
  SQLArchiverResources::init(argc, argv);

  // Are we called with -helpdbg or -help ?
  if ( SQLArchiverResources::getHelpDbgFlag() )
  {
    SQLArchiverResources::printHelpDbg();
    return 0;
  }

  if ( SQLArchiverResources::getHelpFlag() )
  {
    SQLArchiverResources::printHelp();
    return 0;
  }

  // the user wants to know std. report flags
  if ( SQLArchiverResources::getHelpReportFlag() )
  {
    SQLArchiverResources::printHelpReport();
    return 0;
  }

  // Now run our manager
  SQLArchiverManager *thisManager = new SQLArchiverManager;
  thisManager->run();

  // Exit gracefully :)
  // Call Manager::exit instead of ::exit, so we can clean up
  SQLArchiverManager::exit(0);

  // Just make the compilers happy...
  return 0;
}
