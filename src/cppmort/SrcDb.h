#ifndef SRCDB_H
#define SRCDB_H

#include <vector>
#include <string>
#include <functionref.h>

class SrcDB
{
public:
    SrcDB();
    bool loadDatabase(std::string);
    functionRefList getLibraryEntries();
    functionRefList getEntries();
private:
    functionRefList myLibEntries;
    functionRefList myEntries;
};

#endif // SRCDB_H
