#ifndef CFILES_H
#define CFILES_H

#include "../../types/basetypes.h"
#include "../../plat/plat_file.h"

namespace Coffee{
namespace CResources{

/*!
 * \brief A data resource which location cannot be changed.
 */
struct CResource{
private:

    CString m_resource; /*!< URL for the resource*/
public:
    /*!
     * \brief Constructs a resource
     * \param rsrc Path to resource
     * \param absolute False by default, true to not use global file redirection
     */
    CResource(cstring rsrc = nullptr, bool absolute = false);

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

C_FLAGS(CResource::FileFlags,int);

/*!
 * \brief Easy redirection of all resources in the application. :)
 * \param prefix
 * \return
 */
extern void FileResourcePrefix(cstring prefix);

extern bool FileExists(const CResource& resc);

/*!
 * \brief Memory map file as buffer
 * \return True if success
 */
extern bool FileMap(CResource& resc, ResourceAccess acc = ResourceAccess::ReadOnly);
/*!
 * \brief Unmap file
 * \return True if success
 */
extern bool FileUnmap(CResource& resc);

/*!
 * \brief Free data pointer
 */
extern void FileFree(CResource &resc);
extern bool FilePull(CResource &resc, bool textmode = false, bool bigendian = false);
/*!
 * \brief Save data to file
 * \return
 */
extern bool FileCommit(CResource& resc, bool append = false);
/*!
 * \brief Save data to file, append null-terminator
 * \return
 */
extern void FileCommitTextmode(const CResource& resc, bool append = false);

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

inline C_FORCE_INLINE CByteData FileGetDescriptor(CResource& resc)
{
    CByteData d;
    d.data = (byte_t*)resc.data;
    d.size = resc.size;
    return d;
}

}
}

#endif // CFILES_H
