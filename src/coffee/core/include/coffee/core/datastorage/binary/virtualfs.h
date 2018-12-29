#pragma once

#include <coffee/core/types/chunk.h>
#include <coffee/core/url.h>
#include <coffee/interfaces/byte_provider.h>
#include <coffee/interfaces/file_resolver.h>
#include <peripherals/libc/endian_ops.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/bit_vector.h>

#include <algorithm>

namespace Coffee {

struct Resource;

/*! \brief VirtualFS is used to store an asset file system in a single file.
 * Referring into it is done using filenames, and all the files are
 *  ReadOnly, memory mappable.
 *
 * The constituent files may be compressed on-disk, requiring
 *  in-memory inflation. These VirtualFiles are marked
 *  File_Compressed in the `flags` field.
 *
 * Layout of a .cvfs file:
 *
 * |---------------|----------|----------|--------------|
 * | VFS struct[0] | VFile[0] | VFile[1] | Blob data    |
 * |---------------|----------|----------|--------------|
 *
 * VirtualFS struct serves as a header, with a magic segment.
 * VirtualFile structures refer into the Blob data segment.
 *
 */
namespace VirtFS {

struct VirtualFile;
struct VirtualFS;
struct VirtualIndex;

using VFS   = VirtualFS;
using VFile = VirtualFile;

enum class VFSError : int
{
    NotFound = 1,
    NotVFS,
    EndianMismatch,

    VersionMismatch,
    NoIndexing,

    CompressionUnsupported,
    CompressionError,

    /* Creation of a VFS */
    NoFilesProvided,
    FilenameTooLong,
};

struct vfs_error_category : error_category
{
    virtual const char* name() const noexcept override;
    virtual std::string message(int error_code) const override;
};

using vfs_error_code = domain_error_code<VFSError, vfs_error_category>;

/*!
 * \brief In-memory wrapper that handles opening
 *  the VirtualFile from a VirtualFS, while complying with ByteProvider.
 */
struct Resource : semantic::ByteProvider
{
  private:
    VFS const*   filesystem;
    VFile const* file;

  public:
    Resource(VFS const* base, Url const& url);

    C_MOVE_CONSTRUCTOR(Resource);

    /*!
     * \brief Check validity of resource and/or data it returns
     * \return
     */
    bool valid() const;

    cstring resource() const;
    /*!
     * \brief Return the data contained in the virtual file.
     *  If the file in question is compressed,
     *  decompression happens here.
     * \return
     */
    Bytes data() const;

    operator Bytes()
    {
        return data();
    }

    operator Path() const;
};

enum FileFlags
{
    File_Compressed = 0x1,
};

enum class Version : u32
{
    v1,
    v2,
};

enum class LookupMethod : u32
{
    linear_search,
    binary_tree,
};

enum class search_strategy
{
    exact,    /*!< All or nothing search, needs exact prefix match.
                    Aims for log N lookup for single files. */
    earliest, /*!< Return at earliest detection.
                    Is used for finding all files in a directory,
                    files with same name but different extension. */
};

static const constexpr Version VFSVersion = Version::v2;

static const constexpr szptr MaxPrefixLength    = 40;
static const constexpr szptr MaxLeafLength      = MaxPrefixLength - sizeof(u64);
static const constexpr szptr MaxExtensionLength = 16;

static const constexpr szptr MaxFileNameLength   = 96;
static const constexpr szptr MagicLength         = 8;
static const constexpr szptr MagicLength_Encoded = MagicLength / sizeof(u32);

static const constexpr char VFSMagic[MagicLength] = "CfVrtFS";
static const constexpr u32  VFSMagic_Encoded[2]   = {0x72566643, 0x534674};

/*!
 * \brief Stores a binary ordered tree on disk.
 * For each node, the left side is labeled with the last prefix,
 *  the right side is labeled with the first prefix.
 * Each node also has rules for how their prefix composes a final filename,
 *  through a carry and a directory flag.
 */
struct directory_data_t
{
    u64 num_nodes;

    template<typename T>
    struct child_t
    {
        child_t() : value(node_t::sentinel_value)
        {
        }

        T value;

        child_t& operator=(T v)
        {
            value = v;
            return *this;
        }

        child_t& operator=(child_t<T> const& v)
        {
            value = v;
            return *this;
        }

        operator T() const
        {
            return value;
        }
        operator T&()
        {
            return value;
        }

        bool valid() const
        {
            return value != node_t::sentinel_value;
        }
    };

    struct node_t
    {
        static const constexpr u32 sentinel_value =
            std::numeric_limits<u32>::max();

        char         prefix[MaxPrefixLength];
        child_t<u32> left;
        child_t<u32> right;

        union
        {
            u64 _info;
            struct
            {
                u8 prefix : 6; /*!< prefix string length */
            } len_info;
        };

        inline bool operator<=(CString const other) const
        {
            return prefix <= other;
        }

        inline bool operator>=(CString const other) const
        {
            return prefix >= other;
        }
    };

    struct leaf_t
    {
        leaf_t() : mask()
        {
        }

        static const constexpr u64 mask_value = std::numeric_limits<u64>::max();

        u64  mask;
        char prefix[MaxLeafLength];
        u64  fileIdx;

        union
        {
            u64 _info;
            struct
            {
                u8 prefix : 6; /*!< prefix string length */
            } len_info;
        };

        inline void set_mask()
        {
            mask = mask_value;
        }
    };

    struct node_base_t
    {
        node_base_t() : flags(0)
        {
            node.left  = 0;
            node.right = 0;
            MemClear(Bytes::From(node.prefix, MaxPrefixLength));
        }

        u32 flags;
        u32 _pad;
        union
        {
            node_t node;
            leaf_t leaf;
        };

        enum node_flags
        {
            prefix_carry     = 0x1,
            prefix_directory = 0x2,
        };

        inline cstring prefix() const
        {
            return is_leaf() ? leaf.prefix : node.prefix;
        }

        inline szptr prefix_length() const
        {
            return is_leaf() ? leaf.len_info.prefix : node.len_info.prefix;
        }

        inline szptr longest_match(CString const& other, szptr offset = 0) const
        {
            auto node_prefix = prefix();
            auto comparison =
                (std::min)(other.size() - offset, prefix_length());

            return libc::str::longest_prefix(
                node_prefix, &other[offset], comparison);
        }

        inline bool is_leaf() const
        {
            return leaf.mask == leaf_t::mask_value;
        }
    };

    _cbasic_data_chunk<const node_base_t> nodes(VirtualIndex const& idx) const;

    struct node_container_t
    {
        node_container_t() : node(nullptr), index(nullptr)
        {
        }

        node_container_t(node_base_t const* node, VirtualIndex const* index) :
            node(node), index(index)
        {
        }

        node_base_t const*  node;
        VirtualIndex const* index;

        VFile const* file(const VirtualFS* vfs);

        FORCEDINLINE bool valid()
        {
            return node && index;
        }

        node_container_t left();

        node_container_t right();
    };

    struct cached_index
    {
        bit_vector   node_match;
        child_t<u32> sub_root;
    };

    struct result_t
    {
        cached_index     index;
        node_container_t node;

        search_strategy strategy;
    };
};

struct VirtualIndex : non_copy
{
    enum class index_t : u32
    {
        directory_tree = 0x10,
        file_extension = 0x20,
    };

    u64     next_index; /*!< Number of bytes till next index from here */
    index_t kind;       /*!< Describes which index structure is valid */

    const void* data() const
    {
        static_assert(
            sizeof(directory_data_t::node_t) ==
                sizeof(directory_data_t::leaf_t),
            "Mismatching node and leaf size");

        return &this[1];
    }

    const VirtualIndex* next() const
    {
        const byte_t* basePtr = C_RCAST<const byte_t*>(this);

        return C_RCAST<const VirtualIndex*>(basePtr + next_index);
    }

    /*!
     * \brief Describes a list of files sharing a common file extension
     */
    struct extension_data_t
    {
        char ext[MaxExtensionLength];
        u64  num_files;

        _cbasic_data_chunk<const u64> indices(VirtualIndex const& idx) const
        {
            return _cbasic_data_chunk<const u64>::From(
                C_RCAST<const u64*>(idx.data()), num_files);
        }
    };

    using directory_data_t = VirtFS::directory_data_t;

    union
    {
        directory_data_t directory;
        extension_data_t extension;
    };
};

FORCEDINLINE _cbasic_data_chunk<const directory_data_t::node_base_t>
             directory_data_t::nodes(const VirtualIndex& idx) const
{
    return _cbasic_data_chunk<const node_base_t>::From(
        C_RCAST<const node_base_t*>(idx.data()), num_nodes);
}

FORCEDINLINE directory_data_t::node_container_t directory_data_t::
    node_container_t::right()
{
    if(!valid())
        Throw(undefined_behavior("bad dereference"));

    if(node->node.right == node->node.sentinel_value)
        return {};

    return {&index->directory.nodes(*index)[node->node.right], index};
}

FORCEDINLINE directory_data_t::node_container_t directory_data_t::
    node_container_t::left()
{
    if(!valid())
        Throw(undefined_behavior("bad dereference"));

    if(node->node.left == node->node.sentinel_value)
        return {};

    return {&index->directory.nodes(*index)[node->node.left], index};
}

struct VirtualFS
{
    /*!
     * \brief Given bytes from `src', open the VFS inside iff
     *  it is correct. A VFS is only correct if comparing
     *  the file magic completes without error.
     * \param src
     * \param vfs
     * \return
     */
    static bool OpenVFS(
        Bytes const& src, VirtualFS const** vfs, vfs_error_code& ec)
    {
        static_assert(
            MagicLength == MagicLength_Encoded * sizeof(u32),
            "Invalid magic length");
        static_assert(
            sizeof(VFSMagic) == sizeof(VFSMagic_Encoded),
            "Invalid magic length for data");

        using namespace libc;
        using IntData = _cbasic_data_chunk<u32>;

        *vfs = nullptr;

        if(src.size < sizeof(VirtualFS))
        {
            ec = VFSError::NotVFS;
            return false;
        }

        VirtualFS* temp_vfs = C_RCAST<VirtualFS*>(src.data);

        IntData magic     = IntData::From(VFSMagic_Encoded, 2);
        IntData fileMagic = IntData::From(temp_vfs->vfs_header, MagicLength);

        if(magic[0] != fileMagic[0] || magic[1] != fileMagic[1])
        {
            /* It's nice to specify if the endian is wrong here */
            if(magic[0] == endian::to<endian::u32_net>(fileMagic[0]) &&
               magic[1] == endian::to<endian::u32_net>(fileMagic[1]))
                ec = VFSError::EndianMismatch;
            else
                ec = VFSError::NotVFS;

            return false;
        }

        *vfs = temp_vfs;

        return true;
    }

    u32 vfs_header[MagicLength_Encoded]; /*!< Magic file header */

    u64 num_files;   /*!< Number of VFile entries */
    u64 data_offset; /*!< Offset from start of this structure to the data
                        segment */

    /* Version field, introduced in v2 */
    u32 backcomp_sentinel; /*!< This value will be checked to see if a file
                              supports versioning */
    Version virtfs_version;

    u64 files_offset; /*!< Offset from start of this structure to the file
                         segment, introduced in v2 */

    /* Extensions, introduced in v2 */
    struct index_data_t
    {
        u64 num;
        u64 offset; /*!< Offset from start of this structure to  */
    };

    index_data_t ext_index;

    FORCEDINLINE Version version() const
    {
        if(backcomp_sentinel != std::numeric_limits<u32>::max())
            return Version::v1;

        return virtfs_version;
    }

    FORCEDINLINE const void* files() const
    {
        szptr offset = 0;

        switch(version())
        {
        case Version::v1:
            offset += sizeof(u32) * MagicLength_Encoded + sizeof(u64) * 2;
            break;
        default:
            offset = files_offset;
            break;
        }

        return C_RCAST<const char*>(&this[0]) + offset;
    }

    FORCEDINLINE const void* data() const
    {
        return C_RCAST<const char*>(&this[0]) + data_offset;
    }

    FORCEDINLINE const VirtualIndex* indices() const
    {
        return C_RCAST<const VirtualIndex*>(
            C_RCAST<const char*>(&this[0]) + ext_index.offset);
    }

    template<
        LookupMethod Method = LookupMethod::linear_search,
        typename std::enable_if<Method == LookupMethod::linear_search>::type* =
            nullptr>
    /*!
     * \brief Given a VFS, get the handle to a file with the name `name'. This
     * performs a linear search through the VFS, which becomes slow on larger
     * filesystems.
     *
     * This function is kept for backward-compatibility,
     * eg. if older tools are used with a newer library.
     *
     * \param vfs Target VFS
     * \param name Filename to perform search with
     * \return nullptr if file not found
     */
    STATICINLINE VFile const* GetFile(
        VFS const* vfs, cstring name, vfs_error_code& ec)
    {
        return GetFileLinear(vfs, name, ec);
    }

    template<
        LookupMethod Method,
        typename std::enable_if<Method == LookupMethod::binary_tree>::type* =
            nullptr>
    /*!
     * \brief GetFile, but using a binary tree to lookup the file.
     * This has a better worst-case performance than
     * GetFile<linear_search>(...).
     *
     * \param vfs
     * \param name
     * \param ec
     * \return
     */
    STATICINLINE VFile const* GetFile(
        VFS const* vfs, cstring name, vfs_error_code& ec)
    {
        return GetFileTreeExact(vfs, name, ec);
    }

    static directory_data_t::result_t SearchFile(
        VFS const*      vfs,
        cstring         name,
        vfs_error_code& ec,
        search_strategy strat = search_strategy::exact);

    /*!
     * \brief Given a VFS, get the handle to the idx'th file.
     * \param vfs
     * \param idx
     * \return nullptr if file not found
     */
    static VFile const* GetFile(VFS const* vfs, szptr idx, vfs_error_code& ec);

    /* TODO: Return read-only pointer here */
    /*!
     * \brief Given a VFS and a valid file within, return the
     *  data.
     * If the file is compressed, decompression will be done internally.
     * A compressed file will be decompressed for each call to GetData().
     * \param vfs
     * \param file
     * \return
     */
    static Bytes GetData(VFS const* vfs, VFile const* file, vfs_error_code& ec);

    static ResourceResolver<VirtFS::Resource> GetResolver(VirtualFS const* vfs);

  private:
    static VFile const* GetFileLinear(
        VFS const* vfs, cstring name, vfs_error_code& ec);

    static VFile const* GetFileTreeExact(
        VFS const* vfs, cstring name, vfs_error_code& ec);
};

PACKEDSTRUCT VirtualFile
{
    char name[MaxFileNameLength]; /*!< File name */
    u64  offset;                  /*!< Offset to file */
    u64  size;  /*!< Size of file. If compressed, size of compressed file */
    u64  rsize; /*!< Size of file in memory. If compressed, uncompressed size */
    u32  flags;
    const u32 _pad = 0;
};

FORCEDINLINE VFile const* VirtualFS::GetFileLinear(
    VFS const* vfs, cstring name, vfs_error_code& ec)
{
    /* We start finding files after the VirtualFS structure */
    VFile const* vf_start = C_RCAST<VFile const*>(vfs->files());

    auto searchName = CString(name);

    for(szptr i = 0; i < vfs->num_files; i++)
    {
        auto& current_file = vf_start[i];

        if(searchName == current_file.name)
            return &current_file;
    }

    ec = VFSError::NotFound;
    return nullptr;
}

FORCEDINLINE VFile const* VirtualFS::GetFileTreeExact(
    const VFS* vfs, cstring name, vfs_error_code& ec)
{
    auto file      = SearchFile(vfs, name, ec, search_strategy::exact);
    auto vfs_files = C_RCAST<VFile const*>(vfs->files());

    if(file.node.node)
        return &(vfs_files[file.node.node->leaf.fileIdx]);

    return nullptr;
}

FORCEDINLINE VFile const* VirtualFS::GetFile(
    const VFS* vfs, szptr idx, vfs_error_code& ec)
{
    VFile const* vf_start = C_RCAST<VFile const*>(vfs->files());

    if(idx >= vfs->num_files)
    {
        ec = VFSError::NotFound;
        return nullptr;
    }

    return &vf_start[idx];
}

FORCEDINLINE VFile const* VirtualIndex::directory_data_t::node_container_t::
    file(VirtualFS const* vfs)
{
    if(node && node->is_leaf())
        return &C_RCAST<VFile const*>(vfs->files())[node->leaf.fileIdx];

    return nullptr;
}

struct vfs_linear_iterator : Iterator<ForwardIteratorTag, VFile>
{
    static constexpr szptr npos = C_CAST<szptr>(-1);

    /*!
     * \brief constructor for end-iterator
     */
    vfs_linear_iterator() : m_idx(npos), m_file(nullptr)
    {
    }

    vfs_linear_iterator(VFS const* vfs, szptr idx) : m_vfs(vfs), m_idx(idx)
    {
        vfs_error_code ec;
        m_file = vfs ? VFS::GetFile(vfs, idx, ec) : nullptr;

        if(!m_vfs)
            m_idx = npos;
    }

    void update_idx()
    {
        if(m_idx < m_vfs->num_files)
            update_file();
        else
            m_idx = npos;
    }

    vfs_linear_iterator& operator++()
    {
        m_idx++;
        update_idx();
        return *this;
    }

    vfs_linear_iterator& operator--()
    {
        m_idx--;
        update_idx();
        return *this;
    }

    vfs_linear_iterator operator+(difference_type i)
    {
        auto off_idx = C_FCAST<difference_type>(m_idx);
        off_idx += i;
        m_idx = C_FCAST<szptr>(off_idx);

        update_idx();
        return *this;
    }

    vfs_linear_iterator operator-(difference_type i)
    {
        auto off_idx = C_FCAST<difference_type>(m_idx);
        off_idx -= i;
        m_idx = C_FCAST<szptr>(off_idx);

        update_idx();
        return *this;
    }

    vfs_linear_iterator& operator+=(difference_type i)
    {
        *this = (*this) + i;
        return *this;
    }

    vfs_linear_iterator& operator-=(difference_type i)
    {
        *this = (*this) - i;
        return *this;
    }

    difference_type operator-(vfs_linear_iterator& other)
    {
        return C_FCAST<difference_type>(m_idx) -
               C_FCAST<difference_type>(other.m_idx);
    }

    bool operator!=(vfs_linear_iterator const& other) const
    {
        return other.m_idx != m_idx;
    }
    bool operator==(vfs_linear_iterator const& other) const
    {
        return other.m_idx == m_idx;
    }

    VFile const& operator*() const
    {
        if(!m_file)
            Throw(resource_error("non-existent virtual file"));
        return *m_file;
    }

    operator Bytes()
    {
        vfs_error_code ec;
        return VFS::GetData(m_vfs, m_file, ec);
    }

  private:
    void update_file()
    {
        vfs_error_code ec;
        m_file = VFS::GetFile(m_vfs, m_idx, ec);
    }

    VFS const*   m_vfs;
    szptr        m_idx;
    VFile const* m_file;
};

/*!
 * \brief Nearly STL-compatible wrapper around VirtualFS,
 *  allowing iteration and some convenience functions
 *
 * This is only useful if you want to view *every* file in the filesystem.
 */
struct vfs_view
{
    using iterator = vfs_linear_iterator;

    vfs_view(Bytes const& base)
    {
        vfs_error_code ec;
        VFS::OpenVFS(base, &m_vfs, ec);
    }

    iterator begin() const
    {
        return iterator(m_vfs, 0);
    }

    iterator end() const
    {
        return iterator();
    }

    C_DEPRECATED_S("Use GetFile<binary_tree> for better performance")
    iterator starting_with(Path const& path, iterator start)
    {
        return std::find_if(start, end(), [&](VFile const& file) {
            CString fname = file.name;
            /* +4 for extension */
            if(fname.size() + 4 < path.internUrl.size())
                return false;
            else
            {
                return fname.substr(0, path.internUrl.size()) ==
                           path.internUrl &&
                       fname.at(path.internUrl.size()) == '.';
            }
        });
    }

  private:
    VFS const* m_vfs;
};

/*!
 * \brief In-memory description of a to-be VirtualFile.
 * Used dynamic storage, and should never be read from disk.
 * Should be passed to GenVirtFS()
 */
struct VirtDesc
{
    VirtDesc(cstring fname, Bytes&& data, u32 flags = 0) :
        filename(fname), data(std::move(data)), flags(flags)
    {
    }

    VirtDesc(Path const& fname, Bytes&& data, u32 flags = 0) :
        filename(fname.internUrl.c_str()), data(std::move(data)), flags(flags)
    {
    }

    CString filename;
    Bytes   data;
    /*!
     * \brief when matching File_Compressed,
     *  compress the input data using zlib.
     */
    u32 flags;
};

struct generation_settings
{
    generation_settings(szptr workers = 0) : workers(workers)
    {
    }

    szptr workers; /*!< 0 will default to maximum */
};

extern bool GenVirtFS(
    Vector<VirtDesc>&     filenames,
    Vector<byte_t>*       output,
    vfs_error_code&       ec,
    generation_settings&& settings = {});

FORCEDINLINE Url operator"" _vfs(const char* url, size_t)
{
    Url out      = MkUrl(url);
    out.category = Url::Memory;
    return out;
}

FORCEDINLINE cstring Resource::resource() const
{
    return file->name;
}

FORCEDINLINE Resource::operator Path() const
{
    return Path(file->name);
}

} // namespace VirtFS

} // namespace Coffee
