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
#include <mutex>
#include "CPPMConfig.h"

Config *Config::myInstance;

Config *Config::instance() { return myInstance; }

std::string Config::outputDirectory() const
{
    return myOutputDir;
}

bool Config::valid() const
{
    return myValid;
}

bool Config::useGmake() const
{
    return myUseGmake;
}

void Config::setGmakeFlag(bool v)
{
    myUseGmake = v;
}

void Config::setOutputDir(const char *d)
{
    myValid = true;
    myOutputDir = d;
}
