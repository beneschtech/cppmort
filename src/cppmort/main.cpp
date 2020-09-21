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
#include <iostream>
#include <cstdlib>
#include <ctype.h>
#include <unistd.h>

#include "cppmort.h"

char *_progname = (char *)"cppmort";

int main(int argc, char *argv[])
{
    Config cfg;
    if (!parseArgs(cfg,argc,argv))
        usage(1);
    // We initialize our output dir first, if we cant write here, theres no point
    OutputDir out(cfg.outputDirectory());
    if (out.valid())
    {
        std::cout << "Using " << out.fullPath() << " as output path" << std::endl;
    } else {
        std::cout << out.errorMessage() << std::endl;
        return 2;
    }

    // Call make/gmake with aliased cc,c++,g++,gcc, etc..

    // Now load the database and perform the analysis
    performAnalysis();
    return 0;
}

bool parseArgs(Config &cfg,int argc, char **argv)
{
    _progname = *argv;
    int c = 0;
    while ((c = getopt(argc,argv,"hvgd:")) != -1)
    {
        switch (c)
        {
        case 'd': {
            cfg.setOutputDir(optarg);
            break;
        }
        case 'h': {
            usage();
            break;
        }
        case 'g': {
            cfg.setGmakeFlag(true);
            break;
        }
        case 'v': {
            std::cout << PACKAGE_STRING << std::endl;
            ::exit(0);
            break;
        }
        default:
        {
            usage(1);
        }
        }
    }
    return cfg.valid();
}

void usage(int returnVal)
{
    std::cout << PACKAGE_STRING << std::endl;
    std::cout << "\n" PACKAGE_DESCRIPTION "\n" << std::endl;
    std::cout << "Bug reports:       " << PACKAGE_BUGREPORT << std::endl;
    std::cout << "Project home page: " << PACKAGE_URL << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << _progname << " -d <outputdir> [-g] [-h] [-v]" << std::endl;
    std::cout << std::endl;
    std::cout << "-d <outputdir> Required, output directory for unused call report" << std::endl;
    std::cout << "-g Optional, invoke gmake instead of make" << std::endl;
    std::cout << "-h Optional, display this help message and exit" << std::endl;
    std::cout << "-v Optional, display the program version" << std::endl;
    ::exit(returnVal);
}
