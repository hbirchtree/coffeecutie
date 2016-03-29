#ifndef CFILES_H
#define CFILES_H

#include "../../types/edef/enumfun.h"
#include "../../types/tdef/stltypes.h"
#include "../../types/tdef/integertypes.h"
#include "../../plat/plat_file.h"

namespace Coffee{
namespace CResources{

/*!
 * \brief A data resource which location cannot be changed.
 */
struct Resource{
private:
    CString m_resource; /*!< URL for the resource*/

    FileFun::FileMapping m_mapping;
    FileFun::FileHandle* m_handle;

public:

    friend bool FilePull(Resource &resc, bool textmode, bool bigendian);
    friend bool FileCommit(Resource &resc, bool append, ResourceAccess acc);

    friend bool FileMap(Resource &resc, ResourceAccess acc);
    friend bool FileUnmap(Resource &resc);

    /*!
     * \brief Constructs a resource
     * \param rsrc Path to resource
     * \param absolute False by default, true to not use global file redirection
     */
    Resource(cstring rsrc = nullptr,
             bool absolute = false,
             ResourceAccess acc = ResourceAccess::None);

    void* data; /*!< Data pointer*/
    uint64 size; /*!< Data size*/

    cstring resource() const;

    enum FileFlags
    {
        Undefined,
        FileIO,
        Mapped,
    };

    FileFlags flags;
};

C_FLAGS(Resource::FileFlags,int);

/*!
 * \brief Easy redirection of all resources in the application. :)
 * \param prefix
 * \return
 */
extern void FileResourcePrefix(cstring prefix);

extern bool FileExists(const Resource& resc);

/*!
 * \brief Memory map file as buffer
 * \return True if success
 */
extern bool FileMap(Resource& resc, ResourceAccess acc = ResourceAccess::ReadOnly);
/*!
 * \brief Unmap file
 * \return True if success
 */
extern bool FileUnmap(Resource& resc);

/*!
 * \brief Free data pointer
 */
extern void FileFree(Resource &resc);
extern bool FilePull(Resource &resc, bool textmode = false, bool bigendian = false);
/*!
 * \brief Save data to file
 * \return
 */
extern bool FileCommit(Resource& resc, bool append = false,
		       ResourceAccess acc = ResourceAccess::None);
/*!
 * \brief Save data to file, append null-terminator
 * \return
 */
extern void FileCommitTextmode(const Resource& resc, bool append = false);

/*!
 * \brief Create a directory
 * \param dirname
 * \param recursive Whether or not to create non-existent parent directories. Equal to the "-p" option for mkdir.
 * \return True if process succeeded
 */
inline C_FORCE_INLINE bool FileMkdir(cstring dirname, bool recursive)
{
    return DirFun::MkDir(dirname,recursive);
}

FORCEDINLINE CByteData FileGetDescriptor(Resource& resc)
{
    CByteData d;
    d.data = (byte_t*)resc.data;
    d.size = resc.size;
    return d;
}

}
}

#endif // CFILES_H
