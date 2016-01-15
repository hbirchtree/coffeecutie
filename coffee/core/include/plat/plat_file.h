#ifndef COFFEE_PLAT_FILE_H
#define COFFEE_PLAT_FILE_H

#include <types/basetypes.h>

namespace Coffee{
namespace CResources{
namespace CFiles{

//

struct CFile;

extern CFile *coffee_file_open(cstring fname, cstring mode);

extern szptr coffee_file_size(CFile *file);

extern cstring_w coffee_file_read(CFile *file, c_ptr ptr, uint64 *size, bool textmode);

extern bool coffee_file_write(CFile *file, c_cptr data, uint64 size);

extern int coffee_file_close(CFile *file);

//

}
}
}

#endif
