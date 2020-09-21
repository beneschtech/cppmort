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
#ifndef OUTPUTDIR_H
#define OUTPUTDIR_H

#include <string>
class OutputDir
{
public:
    OutputDir(std::string);
    bool valid() const;
    std::string errorMessage() const;
    std::string fullPath() const;

private:
    void setSysError(std::string msg,int);
    bool prepareDirectory(std::string);
    bool clearDirectory(std::string);

    bool myValid;
    std::string myDirectory;
    std::string myErrorMessage;
};

#endif // OUTPUTDIR_H
