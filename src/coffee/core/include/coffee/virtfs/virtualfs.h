#pragma once

#include <coffee/compression/standard.h>
#include <coffee/core/types/chunk.h>
#include <coffee/core/url.h>
#include <coffee/interfaces/byte_provider.h>
#include <coffee/interfaces/file_resolver.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/bit_vector.h>

#if !defined(COFFEE_NO_ENDIAN_OPS)
#include <peripherals/libc/endian_ops.h>
#endif

#include <algorithm>

namespace Coffee {

struct Resource;

/*! \brief fs is used to store an asset file system in a single file.
 * Referring into it is done using filenames, and all the files are
 *  ReadOnly, memory mappable.
 *
 * The constituent files may be compressed on-disk, requiring
 *  in-memory inflation. These VirtualFiles are marked
 *  File_Compressed in the `flags` field.
 *
 * Layout of a .cvfs file:
 *
 * |---------------|-------|----------|----------|--------------|
 * | VFS struct[0] |[other]| file[0] | file[1] | Blob data    |
 * |---------------|-------|----------|----------|--------------|
 *
 * fs struct serves as a header, with a magic segment.
 * VirtualFile structures refer into the Blob data segment.
 *
 * All offsets in VFS use offsets based on global offsets in the header.
 */
} // namespace Coffee

namespace vfs {

using libc_types::i32;
using libc_types::i64;
using libc_types::ptroff;
using libc_types::szptr;
using libc_types::u32;
using libc_types::u64;
using libc_types::u8;
using platform::url::Path;
using platform::url::Url;
using semantic::mem_chunk;
using semantic::Span;

using Span_u8 = Span<const libc_types::u8>;

using namespace std::string_view_literals;

struct file;
struct fs;
struct index;

enum class error : int
{
    not_found = 1,
    not_vfs,
    endian_mismatch,

    version_mismatch,
    no_indexing,
    index_not_found,

    compression_unsupported,
    compression_error,
    compression_codec_unsupported,

    /* Creation of a VFS */
    no_files_provided,
    filename_too_long,
};

/*!
 * \brief In-memory wrapper that handles opening
 *  the VirtualFile from a fs, while complying with ByteProvider.
 */
struct Resource : semantic::ByteProvider
{
  private:
    fs const*   filesystem;
    file const* file;

  public:
    Resource(fs const* base, Url const& url);

    C_MOVE_CONSTRUCTOR(Resource);

    /*!
     * \brief Check validity of resource and/or data it returns
     * \return
     */
    NO_DISCARD bool valid() const;

    std::string_view resource() const;
    /*!
     * \brief Return the data contained in the virtual file.
     *  If the file in question is compressed,
     *  decompression happens here.
     * \return
     */
    NO_DISCARD semantic::mem_chunk<const libc_types::u8> data() const;

    NO_DISCARD operator Path() const;
};

enum FileFlags
{
    File_Compressed = 0x1,
};

enum class Version : u32
{
    v1,
    v2, /*!< Adds indexing */
    v3, /*!< Adds more compression formats */
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

constexpr auto VFSVersion = Version::v3;

constexpr auto MaxPrefixLength    = 40u;
constexpr auto MaxLeafLength      = MaxPrefixLength - sizeof(u64);
constexpr auto MaxExtensionLength = 16u;

constexpr auto MaxFileNameLength   = 96u;
constexpr auto MagicLength         = 8u;
constexpr auto MagicLength_Encoded = MagicLength / sizeof(u32);

constexpr char VFSMagic[MagicLength] = "CfVrtFS";
constexpr u32  VFSMagic_Encoded[2]   = {0x72566643, 0x534674};

/*!
 * \brief Stores a binary ordered tree on disk.
 * For each node, the left side is labeled with the last prefix,
 *  the right side is labeled with the first prefix.
 * Each node also has rules for how their prefix composes a final filename,
 *  through a carry and a directory flag.
 */
struct directory_data_t
{
    using index_type = u32;

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
        static const constexpr index_type sentinel_value =
            std::numeric_limits<index_type>::max();

        char                prefix[MaxPrefixLength];
        child_t<index_type> left;
        child_t<index_type> right;

        union
        {
            u64 _info;
            struct
            {
                u8 prefix : 6; /*!< prefix string length */
            } len_info;
        };

        inline bool operator<=(std::string const other) const
        {
            return prefix <= other;
        }

        inline bool operator>=(std::string const other) const
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
            auto nodes = Span<char>(node.prefix, MaxPrefixLength);
            std::fill(nodes.begin(), nodes.end(), 0);
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

        inline std::string_view prefix() const
        {
            return is_leaf() ? leaf.prefix : node.prefix;
        }

        inline szptr prefix_length() const
        {
            return is_leaf() ? leaf.len_info.prefix : node.len_info.prefix;
        }

        inline szptr longest_match(
            std::string_view const& other, szptr offset = 0) const
        {
            auto node_prefix = prefix();
            auto comparison =
                (std::min)(other.size() - offset, prefix_length());

            return libc::str::longest_prefix(
                node_prefix.substr(0, comparison), other.substr(offset, comparison));
        }

        inline bool is_leaf() const
        {
            return leaf.mask == leaf_t::mask_value;
        }
    };

    Span<const node_base_t> nodes(index const& idx) const;

    struct node_container_t
    {
        node_container_t() : node(nullptr), index(nullptr)
        {
        }

        node_container_t(node_base_t const* node, index const* index) :
            node(node), index(index)
        {
        }

        node_base_t const* node;
        index const*       index;

        file const* file(const fs* vfs);

        FORCEDINLINE bool valid()
        {
            return node && index;
        }

        node_container_t left();

        node_container_t right();
    };

    struct cached_index
    {
        std::vector<bool>   node_match;
        child_t<index_type> sub_root;
        index const*        virt_index;

        Span<const node_base_t> nodes() const;
    };

    struct result_t
    {
        cached_index     index;
        node_container_t node;

        search_strategy strategy;
    };
};

struct index : stl_types::non_copy
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

    const index* next() const
    {
        auto base =
            mem_chunk<const u8>::ofBytes(this, next_index + sizeof(index));
        return (*base.at(next_index)).as<const index>().data;
    }

    /*!
     * \brief Describes a list of files sharing a common file extension
     */
    struct extension_data_t
    {
        char ext[MaxExtensionLength];
        u64  num_files;

        mem_chunk<const u64> indices(index const& idx) const
        {
            return mem_chunk<const u64>::of(
                C_RCAST<const u64*>(idx.data()), num_files);
        }
    };

    using directory_data_t = vfs::directory_data_t;

    union
    {
        directory_data_t directory;
        extension_data_t extension;
    };
};

FORCEDINLINE Span<const directory_data_t::node_base_t> directory_data_t::nodes(
    const index& idx) const
{
    return mem_chunk<const node_base_t>::of(
               C_RCAST<const node_base_t*>(idx.data()), num_nodes)
        .view;
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

FORCEDINLINE Span<const directory_data_t::node_base_t> directory_data_t::
    cached_index::nodes() const
{
    return virt_index->directory.nodes(*virt_index);
}

PACKEDSTRUCT(file {
    char name_[MaxFileNameLength]; /*!< File name */
    u64  offset;                   /*!< Offset to file */
    u64  size;  /*!< Size of file. If compressed, size of compressed file */
    u64  rsize; /*!< Size of file in memory. If compressed, uncompressed size */
    u32  flags;
    compression::codec codec = compression::codec::none;

    FORCEDINLINE std::string_view name() const
    {
        return name_;
    }
});

struct fs
{
    /*!
     * \brief Given bytes from `src', open the VFS inside iff
     *  it is correct. A VFS is only correct if comparing
     *  the file magic completes without error.
     * \param src
     * \param vfs
     * \return
     */
    template<typename T>
    static stl_types::result<const fs*, error> open(Span<T> const& src)
    {
        static_assert(
            MagicLength == MagicLength_Encoded * sizeof(u32),
            "Invalid magic length");
        static_assert(
            sizeof(VFSMagic) == sizeof(VFSMagic_Encoded),
            "Invalid magic length for data");

        using namespace libc;
        using IntData = mem_chunk<const u32>;

        if(src.size() < sizeof(fs))
        {
            return error::not_vfs;
        }

        auto* temp_vfs = C_RCAST<const fs*>(src.data());

        IntData magic     = IntData::of(VFSMagic_Encoded, 2);
        IntData fileMagic = IntData::of(temp_vfs->vfs_header, MagicLength);

        if(magic[0] != fileMagic[0] || magic[1] != fileMagic[1])
        {
#if !defined(COFFEE_NO_ENDIAN_OPS)
            /* It's nice to specify if the endian is wrong here */
            if(magic[0] == endian::to<endian::u32_net>(fileMagic[0]) &&
               magic[1] == endian::to<endian::u32_net>(fileMagic[1]))
                return error::endian_mismatch;
            else
#endif
                return error::not_vfs;
        }

        return temp_vfs;
    }

    u32 vfs_header[MagicLength_Encoded]; /*!< Magic file header */

    u64 num_files;   /*!< Number of file entries */
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

    FORCEDINLINE bool supportsIndex(index::index_t type) const
    {
        if(version() == Version::v1)
            return false;

        for(C_UNUSED(auto _) : stl_types::Range<>(ext_index.num))
            if(indices()[0].kind == type)
                return true;

        return false;
    }

    FORCEDINLINE Span<const file> files() const
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

        return mem_chunk<const file>::ofBytes(
                   C_RCAST<const u8*>(this) + offset, num_files * sizeof(file))
            .view;
    }

    FORCEDINLINE Span_u8 data() const
    {
        return mem_chunk<const u8>::ofBytes(this, Span<const u8>::extent).view;
    }

    FORCEDINLINE Span<const index> indices() const
    {
        return mem_chunk<const index>::ofBytes(
                   this + ext_index.offset, sizeof(index))
            .view.subspan<0, 1>();
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
    STATICINLINE stl_types::result<file const*, error> GetFile(
        fs const* vfs, std::string_view name)
    {
        return GetFileLinear(vfs, name);
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
    STATICINLINE stl_types::result<file const*, error> GetFile(
        fs const* vfs, std::string_view name)
    {
        return GetFileTreeExact(vfs, name);
    }

    static stl_types::result<directory_data_t::result_t, error> SearchFile(
        fs const*                             vfs,
        std::string_view                      name,
        search_strategy                       strat  = search_strategy::exact,
        const directory_data_t::cached_index* filter = nullptr);

    /*!
     * \brief Given a VFS, get the handle to the idx'th file.
     * \param vfs
     * \param idx
     * \return nullptr if file not found
     */
    static stl_types::result<file const*, error> GetFile(
        fs const* vfs, szptr idx);

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
    static stl_types::result<mem_chunk<const u8>, error> GetData(
        fs const* vfs, file const* file);

    static Coffee::ResourceResolver<vfs::Resource> GetResolver(fs const* vfs);

  private:
    static stl_types::result<file const*, error> GetFileLinear(
        fs const* vfs, std::string_view name);

    static stl_types::result<file const*, error> GetFileTreeExact(
        fs const* vfs, std::string_view name);
};

FORCEDINLINE stl_types::result<file const*, error> fs::GetFileLinear(
    fs const* vfs, std::string_view name)
{
    /* We start finding files after the fs structure */
    auto vf_start = vfs->files();

    for(szptr i = 0; i < vfs->num_files; i++)
    {
        auto& current_file = vf_start[i];

        if(name == current_file.name())
            return &current_file;
    }

    return error::not_found;
}

FORCEDINLINE stl_types::result<file const*, error> fs::GetFileTreeExact(
    const fs* vfs, std::string_view name)
{
    auto vfs_files = vfs->files();
    if(auto search = SearchFile(vfs, name, search_strategy::exact);
       search.has_error())
        return search.error();
    else if(auto file = search.value(); file.node.node)
        return &vfs_files[file.node.node->leaf.fileIdx];
    return error::not_found;
}

FORCEDINLINE stl_types::result<file const*, error> fs::GetFile(
    const fs* vfs, szptr idx)
{
    auto vf_start = vfs->files();

    if(idx >= vfs->num_files)
        return error::not_found;

    return &vf_start[idx];
}

FORCEDINLINE file const* index::directory_data_t::node_container_t::file(
    fs const* vfs)
{
    if(node && node->is_leaf())
        return &vfs->files()[node->leaf.fileIdx];

    return nullptr;
}

struct vfs_linear_iterator
    : stl_types::Iterator<stl_types::ForwardIteratorTag, file>
{
    static constexpr szptr npos = C_CAST<szptr>(-1);

    /*!
     * \brief constructor for end-iterator
     */
    vfs_linear_iterator() : m_idx(npos), m_file(nullptr)
    {
    }

    vfs_linear_iterator(fs const* vfs, szptr idx) : m_vfs(vfs), m_idx(idx)
    {
        m_file = vfs ? fs::GetFile(vfs, idx).value() : nullptr;
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

    file const& operator*() const
    {
        if(!m_file)
            Throw(resource_error("non-existent virtual file"));
        return *m_file;
    }

    operator mem_chunk<const u8>() const
    {
        return fs::GetData(m_vfs, m_file).value();
    }

  private:
    void update_file()
    {
        m_file = fs::GetFile(m_vfs, m_idx).value();
    }

    fs const*   m_vfs;
    szptr       m_idx;
    file const* m_file;
};

/*!
 * \brief Nearly STL-compatible wrapper around fs,
 *  allowing iteration and some convenience functions
 *
 * This is only useful if you want to view *every* file in the filesystem.
 */
struct view
{
    using iterator = vfs_linear_iterator;

    template<typename T>
    static auto of(Span<T> const& base)
    {
        view out;
        out.m_vfs = fs::open(base).value();
        return out;
    }

    NO_DISCARD iterator begin() const
    {
        return iterator(m_vfs, 0);
    }

    NO_DISCARD iterator end() const
    {
        return iterator();
    }

    NO_DISCARD iterator starting_with(Path const& path, iterator start)
    {
        return std::find_if(start, end(), [&](file const& file) {
            /* +4 for extension */
            if(file.name().size() + 4 < path.internUrl.size())
                return false;
            else
            {
                return file.name().substr(0, path.internUrl.size()) ==
                           path.internUrl &&
                       file.name().at(path.internUrl.size()) == '.';
            }
        });
    }

  private:
    view()
    {
    }

    fs const* m_vfs;
};

/*!
 * \brief In-memory description of a to-be VirtualFile.
 * Used dynamic storage, and should never be read from disk.
 * Should be passed to GenVirtFS()
 */
struct desc
{
    desc(std::string_view fname, Span_u8&& data, u32 flags = 0) :
        filename(fname), data(std::move(data)), flags(flags)
    {
    }

    desc(Path const& fname, Span_u8&& data, u32 flags = 0) :
        filename(fname.internUrl), data(std::move(data)), flags(flags)
    {
    }

    std::string filename;
    Span_u8     data;
    /*!
     * \brief when matching File_Compressed,
     *  compress the input data using zlib.
     */
    u32 flags;
};

struct generation_settings
{
    szptr              workers; /*!< 0 will default to maximum */
    compression::codec file_codec =
        compression::codec::deflate; /*!< Codec for compressing files */
};

extern stl_types::result<std::vector<libc_types::byte_t>, error> generate(
    std::vector<desc>& filenames, generation_settings&& settings = {});

NO_DISCARD FORCEDINLINE Url operator"" _vfs(const char* url, size_t)
{
    Url out      = Coffee::MkUrl(url);
    out.category = Url::Local;
    return {
        .internUrl = url,
        .category  = Url::StorageType::Local,
        .flags     = semantic::RSCA::Virtual | semantic::RSCA::AssetFile,
    };
}

FORCEDINLINE std::string_view Resource::resource() const
{
    return file->name();
}

NO_DISCARD FORCEDINLINE Resource::operator Path() const
{
    return Path(file->name());
}

} // namespace vfs
