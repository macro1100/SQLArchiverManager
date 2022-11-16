// Our resources administration
#include  <SQLArchiverResources.hxx>
#include  <ErrHdl.hxx>

// Our static Variable
CharString  SQLArchiverResources::dbServer = "MACRO01";
CharString  SQLArchiverResources::dbUsername = "";
CharString  SQLArchiverResources::dbPassword = "";
CharString  SQLArchiverResources::dbDatabase = "WINCCOAArchive";
CharString  SQLArchiverResources::dbPort = "1433";


// Wrapper to read config file
void  SQLArchiverResources::init(int &argc, char *argv[])
{
  begin(argc, argv);
  while ( readSection() || generalSection() )
    ;
  end(argc, argv);
}


// Read the config file.
// Our section is [SQLArchiver] or [SQLArchiver<num>],
// our keywords are "dpNameSet" and dpNameConnect
PVSSboolean  SQLArchiverResources::readSection()
{
  // Is it our section ?
  // This will test for [SQLArchiver] and [SQLArchiver_<num>]
  if (!isSection("SQLArchiver"))
    return PVSS_FALSE;

  // Read next entry
  getNextEntry();

  // Loop thru section
  while ( (getCfgState() != CFG_SECT_START) &&  // Not next section
          (getCfgState() != CFG_EOF) )          // End of config file
  {
    if (!getKeyWord().icmp("server"))
      getCfgStream() >> dbServer;
    else if (!getKeyWord().icmp("username"))
      getCfgStream() >> dbUsername;
    else if (!getKeyWord().icmp("password"))
        getCfgStream() >> dbPassword;
    else if (!getKeyWord().icmp("database"))
        getCfgStream() >> dbDatabase;
    else if (!getKeyWord().icmp("port"))
        getCfgStream() >> dbPort;
    else if (!readGeneralKeyWords())
    {
      unknownKeyWordError();
    }

    getNextEntry();
  }

  // So the loop will stop at the end of the file
  return getCfgState() != CFG_EOF;
}

