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
        std::vector<std::string> remotes =
        {"http:.*","https:.*","ftp:.*","ftps:.*","sftp:.*"};
        std::vector<std::string> =
        {"file:.*","/.*","[A-Za-z]:.*"};

        for(uint8_t i=0;i<remotes_size;i++){

        }
    }
    std::string m_resource;
};

}
}

#endif // CFILES_H

