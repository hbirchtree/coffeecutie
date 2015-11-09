#ifndef COFFEE_PLAT_FILE_H
#define COFFEE_PLAT_FILE_H

#include "coffee/core/coffee_basetypes.h"

namespace Coffee{
namespace CResources{
namespace CFiles{

struct CFile;

CFile *coffee_file_open(cstring fname, cstring mode);

szptr coffee_file_size(CFile *file);

cstring_w coffee_file_read(CFile *file, void *ptr, szptr *size, bool textmode);

bool coffee_file_write(CFile *file, const void *data, szptr size);

int coffee_file_close(CFile *file);

}
}
}

#endif
