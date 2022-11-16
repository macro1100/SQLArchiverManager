// The Resource file for the SQLArchiver manager
#ifndef SQLArchiverRESOURCES_H
#define SQLArchiverRESOURCES_H

#include  <OaResources.hxx>

class SQLArchiverResources : public OaResources
{
  public:
    // These functions initializes the manager
    static void init(int &argc, char *argv[]);

    // Read the config section
    static PVSSboolean readSection();

  public:
    static const CharString& getServer() { return dbServer; }
    static const CharString& getUsername() { return dbUsername; }
    static const CharString& getPassword() { return dbPassword; }
    static const CharString& getDatabase() { return dbDatabase; }
    static const CharString& getDatabasePort() { return dbPort; }

  private:
    static CharString dbServer;
    static CharString dbUsername;
    static CharString dbPassword;
    static CharString dbDatabase;
    static CharString dbPort;
};

#endif
