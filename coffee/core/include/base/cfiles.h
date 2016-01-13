#ifndef CFILES_H
#define CFILES_H

#include <types/basetypes.h>

namespace Coffee{
namespace CResources{

/*!
 * \brief Easy redirection of all resources in the application. :)
 * \param prefix
 * \return
 */
extern void FileResourcePrefix(cstring prefix);

struct CResourceUrl
{
    enum ResourceFlags{
        /*! Resource is in a remote location, internet and etc.*/
        Remote      = 0x1,
        /*! Resource is read-only*/
        ReadOnly    = 0x2,
        /*! Resource is read-write*/
        ReadWrite   = 0x4,
        /*! Resource is write-only*/
        WriteOnly   = 0x8,

        /*! Resource is mapped or streamed*/
        Streaming   = 0x10,
    };

    CResourceUrl();

    cstring url;
    uint8 flags;
};

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

};

extern bool FileExists(const CResource& resc);

/*!
 * \brief Memory map file as buffer
 * \return True if success
 */
extern bool FileMap(CResource& resc);
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
extern bool FileMkdir(cstring dirname, bool recursive);

}
}

#endif // CFILES_H
