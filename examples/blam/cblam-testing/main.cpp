#include <coffee/CBlam>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>
#include <coffee/core/CApplication>

#include <iterator>
#include <algorithm>

using namespace Coffee;
using namespace CResources;
using namespace Blam;


class tag_index_view
{
    tag_index_t& m_idx;
    file_header_t const* m_file;
    index_item_t const* m_root;

public:

    class index_iterator : public std::iterator<std::forward_iterator_tag, index_item_t const*>
    {
        uint32 i;
        tag_index_view& idx;

        index_item_t const* deref()
        {
            if(!idx.m_root)
                idx.m_root = blam_tag_index_get_items(idx.m_file);
            return &idx.m_root[i];
        }

    public:
        index_iterator(tag_index_view& idx, uint32 i):
            i(i),
            idx(idx)
        {
        }

        bool operator !=(index_iterator const& other) const
        {
            return i != other.i;
        }

        index_iterator& operator++()
        {
            i++;
            return *this;
        }

        index_item_t const* operator*()
        {
            return deref();
        }
    };

    using iterator = index_iterator;

    tag_index_view(file_header_t const* file, tag_index_t& idx):
        m_idx(idx),
        m_file(file),
        m_root(nullptr)
    {
    }

    iterator begin()
    {
        return index_iterator(*this, 0);
    }
    iterator end()
    {
        return index_iterator(*this, m_idx.tagCount - 1);
    }
};

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the main function.
 *   The purpose of this is platform abstraction such that the rest of the code works consistently across platforms.
 */
int coffee_main(int32 argv,cstring_w* argc)
{
    CResources::FileResourcePrefix("sample_data/cblam_data/");

//    cstring mapstring = GetArgument(argv,argc,"halomap");
    cstring mapstring = "bloodgulch.map";
    if(!mapstring)
        return 1;
    Resource mapfile(mapstring);
    if(!FileExists(mapfile))
        return 1;

    Resource bitmfile("bitmaps.map");
    if(!FileMap(bitmfile))
        return 1;
    if(!FileMap(mapfile))
        return 1;
    const file_header_t* map =
            blam_file_header_get(mapfile.data,version_t::pc);
    tag_index_t tags = blam_tag_index_get(map);

    {
        /* Extracting scenario data */
        const blam_scenario* scn = blam_scn_get(map,&tags);

        const blam_scn_bsp_header* sbsp = scn->struct_bsp.data(map,tags.index_magic);
        for(int i=0;i<scn->struct_bsp.count;i++)
        {
            const blam_scn_bsp_header* s_bsp = &sbsp[i];
            //        const blam_scn_chunk* bsp =
            //                (const blam_scn_chunk*)
            //                blam_mptr(map,0,s_bsp->offset);
            cDebug("BSP: {0}",(cstring)blam_mptr(map,tags.index_magic,
                                                 s_bsp->name_ptr));
        }
    }


    struct texture_data_t
    {
        bitm_texture_t tex;
        const index_item_t* tag;
        const bitm_image_t* img;
        cstring tag_name;
    };

    auto debigend = [](cstring src, uint32 len)
    {
        return StrUtil::reverse(StrUtil::encapsulate(src, len));
    };

    tag_index_view index_view(map, tags);

    auto pred = [&](index_item_t const* e)
    {
        return debigend(e->tagclass[0], 4) == "bitm";
    };

    for(auto e : index_view)
    {
        cDebug("Values: {0}", blam_index_item_get_string(e, map, &tags));
    }

//    {
//        /* Extracting texture data */
//        texture_data_t img;
//        const index_item_t* it = nullptr;
//        while(it = fun(tags, map) /*fun_filter(fun, tags, map, bitmfile, img)*/)
//        {
//            cDebug("Index item: {0} -> {1}, {2}, {3}",
//                   it->tagId,
//                   debigend(it->tagclass[0], 4),
//                    (it->tagclass[1][0] != -1) ? debigend(it->tagclass[1], 4) : CString(),
//                    blam_index_item_get_string(it, map, &tags));
////            cDebug("Texture: {0},res={1},mip={2}",img.tag_name,img.tex.resolution,img.tex.mipmaps);
//        }
//    }

    FileUnmap(bitmfile);
    FileUnmap(mapfile);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
