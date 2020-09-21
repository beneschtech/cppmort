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
#ifndef CPPMORT_CONFIG_H
#define CPPMORT_CONFIG_H

#include <string>

class Config
{
public:
    Config(): myOutputDir(""),myUseGmake(false),myValid(false) { myInstance = this; }
    static Config *instance();
    std::string outputDirectory() const;
    bool useGmake() const;
    bool valid() const;

    void setGmakeFlag(bool);
    void setOutputDir(const char *);

private:
    static Config *myInstance;
    std::string myOutputDir;
    bool myUseGmake;
    bool myValid;
};

#endif // CONFIG_H
