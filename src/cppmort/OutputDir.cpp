/**
 *
 *    ######   #######  #     #  #######   #####    #####   #     #  #######  #######   #####   #     #
 *   #     #  #        ##    #  #        #     #  #     #  #     #     #     #        #     #  #     #
 *   #     #  #        # #   #  #        #        #        #     #     #     #        #        #     #
 *  ######   #####    #  #  #  #####     #####   #        #######     #     #####    #        #######
 *  #     #  #        #   # #  #              #  #        #     #     #     #        #        #     #
 * #     #  #        #    ##  #        #     #  #     #  #     #     #     #        #     #  #     #
 * ######   #######  #     #  #######   #####    #####   #     #     #     #######   #####   #     #
 *
 * LLC
 *
 **/
#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <linux/limits.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include "OutputDir.h"

OutputDir::OutputDir(std::string d)
    :myValid(false)
    ,myDirectory("")
{
    myValid = prepareDirectory(d);
}

bool OutputDir::valid() const
{
    return myValid;
}

std::string OutputDir::fullPath() const
{
    return myDirectory;
}

std::string OutputDir::errorMessage() const
{
    return myErrorMessage;
}

void OutputDir::setSysError(std::string msg, int c)
{
    myErrorMessage = msg + ": ";
    myErrorMessage += strerror(c);
    myValid = false;
}

bool OutputDir::prepareDirectory(std::string d)
{
    char pathbuf[PATH_MAX];
    memset(pathbuf,0,sizeof(pathbuf));
    myDirectory = d;
    if (realpath(d.c_str(),pathbuf) == nullptr)
    {
        int err = errno;
        if (err != ENOENT)
        {
            setSysError(d,err);
            return false;
        }
    }
    myDirectory = pathbuf;
    struct stat s;
    int rv = stat(pathbuf,&s);
    if (rv != 0)  // Path doesnt exist, we dont do recurvise creation, only one level
    {
        if (::mkdir(pathbuf,S_IRWXU | S_IRWXG | S_IRWXO) != 0)
        {
            setSysError(d,errno);
            return false;
        }
        // IF that succeeded then we can stat again
        stat(pathbuf,&s);
    }
    if (!S_ISREG(s.st_mode) && !S_ISDIR(s.st_mode))
    {
        myErrorMessage = "Nice try...";
        return false;
    }
    if (S_ISREG(s.st_mode)) // Regular file, delete and change into a directory
    {
        rv = ::unlink(pathbuf);
        rv += ::mkdir(pathbuf,S_IRWXU | S_IRWXG | S_IRWXO);
        if (rv) // One of them failed, and probably for the same reason
        {
            setSysError(d,errno);
            return false;
        }
    }

    // Ok, now its a directory, with or without content
    return clearDirectory(myDirectory);
}

bool OutputDir::clearDirectory(std::string dir)
{
    DIR *dp = nullptr;
    if (!(dp = opendir(dir.c_str())))
    {
        setSysError(dir,errno);
        return false;
    }
    struct dirent *dep = nullptr;
    while ((dep = ::readdir(dp)))
    {
        struct stat s;
        std::string ename = dep->d_name;
        if (ename == "." || ename == "..")
            continue;
        ename = dir +"/" + dep->d_name;
        // I know we *could* use d_type, but for portability, lets stat it
        if (::stat(ename.c_str(),&s) != 0)
        {
            setSysError(ename,errno);
            return false;
        }
        if (S_ISREG(s.st_mode)) {
            if (::unlink(ename.c_str()) != 0)
            {
                setSysError(ename,errno);
                return false;
            }
        }
        if (S_ISDIR(s.st_mode)) {
            if (!clearDirectory(ename))
                return false;
            if (::rmdir(ename.c_str()) != 0)
            {
                setSysError(ename,errno);
                return false;
            }
        }
    }
    return true;
}
