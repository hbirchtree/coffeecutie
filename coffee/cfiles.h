#ifndef CFILES_H
#define CFILES_H

#include "coffee.h"

namespace Coffee{
namespace CResources{

struct CResource{
    CResource(std::string resource){
        m_resource = resource;
    }

private:
    enum ResourceType{
        Undefined   = 0x0,
        LocalFile   = 0x1,
        RemoteFile  = 0x2,
    };
    void identify_resource(){
        //Not all of these are implemented!
        const char** remotes    =
        {"^http:.*","^https:.*","^ftp:.*","^ftps:.*","^sftp:.*"};
        uint8_t remotes_size = 5;
        const char** locals     =
        {"^file:.*","^/.*","^[A-Za-z]:.*"};
        uint8_t locals_size = 3;

        for(uint8_t i=0;i<remotes_size;i++){

        }
    }
    std::string m_resource;
};

}
}

#endif // CFILES_H

