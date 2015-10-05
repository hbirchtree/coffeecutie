#ifndef CBLAM_MAP
#define CBLAM_MAP

#include "coffee_types.h"

namespace Coffee{

static bool coffee_cmp_memarea(
        cstring target, //non-null-terminated string
        cstring cmp,    //null-terminated string
        szptr len)      //length of comparison
{
    return memcmp(target,cmp,len)==0;
}

namespace CBlam{

//Metadata

enum blam_maptype
{
    blam_maptype_singleplayer  = 0,
    blam_maptype_multiplayer   = 1,
    blam_maptype_ui            = 2,
};

enum blam_versions
{
    blam_version_xbox   = 5,
    blam_version_pc     = 7,
};

static cstring const blam_index_item_type_mod2 = "2dom";
static cstring const blam_index_item_type_bitm = "mtib";
static cstring const blam_index_item_type_scnr = "rncs";

static cstring const blam_header_head = "deah";
static cstring const blam_header_foot = "toof";

const static int32 blam_num_map_names = 28;
const static struct mapnames {
    byte inname[32];
    byte outname[32];
} blam_map_names[28] = {
//Single player maps
{"a10","Pillar of Autumn"},
{"a30", "Halo"},
{"a50", "Truth and Reconciliation"},
{"b30","Silent Cartographer"},
{"b40","Assault on the Control Room"},
{"c10","343 Guilty Spark"},
{"c20","The Library"},
{"c40","Two Betrayals"},
{"d20","Keyes"},
{"d40", "The Maw"},

//Multi-player maps
{"beavercreek","Beaver Creek"},
{"bloodgulch","Blood Gulch"},
{"boardingaction","Boarding Action"},
{"carousel","Derelict"},
{"chillout","Chill Out"},
{"deathisland","Death Island"},
{"damnation","Damnation"},
{"dangercanyon","Danger Canyon"},
{"gephyrophobia","Gephyrophobia"},
{"hangemhigh","Hang 'em High"},
{"icefields","Ice Fields"},
{"longest","Longest"},
{"prisoner","Prisoner"},
{"putput","Chiron TL34"},
{"ratrace","Rat Race"},
{"sidewinder","Sidewinder"},
{"timberland","Timberland"},
{"wizard","Wizard"},
};

//Data structures

struct blam_file_header
{
    int32   id;
    int32   version;
    int32   decomp_len;
    int32   Unknown1;
    int32   tagIndexOffset;
    int32   tagIndexMetaLen;
    int32   reserved_1[2];
    byte    name[32];
    byte    buildDate[32];
    int32   mapType;
    int32   unknown_4;
    int32   reserved_2[485];
    int32   footer;
};

struct blam_tag_index
{
    int32   index_magic;
    int32   baseTag;
    int32   vertexSize;
    int32   tagCount;
    int32   vertexObjectCount;
    uint32  vertexOffset;
    int32   indicesObjectCount;
    uint32  indicesOffset;
    int32   modelRawDataSize;
    int32   tagStart;
};

struct blam_index_item
{
    byte    tagclass[3][4];
    int32   tagId;
    uint32  stringOffset;
    int32   offset;
    int32   zeroes[2];
};

struct blam_reflexive
{
    int32 count;
    int32 offset;
    int32 zero;
};

struct blam_tagref
{
    byte    tag[4];
    int32   namePtr;
    int32   unknown;
    int32   tagId;
};

//Essential functions!

//Get the proper, full name of the map
static cstring blam_file_header_full_mapname(const blam_file_header* map)
{
    for(int32 i=0;i<blam_num_map_names;i++)
    {
        if(!strcmp(map->name,blam_map_names[i].inname))
            return blam_map_names[i].outname;
    }
    return "";
}

//Validate and get the file header from a base pointer, mostly validation
static blam_file_header* blam_file_header_get(void* baseptr, int32 expectedVersion)
{
    blam_file_header* fh = (blam_file_header*)baseptr;
    if(
            fh->version!=expectedVersion&&
            coffee_cmp_memarea((cstring)&fh->id,blam_header_head,4)&&
            coffee_cmp_memarea((cstring)&fh->footer,blam_header_foot,4))
        return nullptr;
    return fh;
}

//Set index magic for a tag index, special sauce lies here
//Modifies the index structure
static void blam_tag_index_magic(
        blam_tag_index* tagindex,   //tagindex structure
        int32 tagIndexOffset        //tag index offset from map
        )
{
    tagindex->index_magic -= tagIndexOffset+40;
}

//Get the tag index pointer out of the file header
static const blam_tag_index* blam_tag_index_ptr(const blam_file_header* file)
{
    return (const blam_tag_index*)&((const byte*)file)[file->tagIndexOffset];
}

//Get the tag index items
static const blam_index_item* blam_tag_meta_offset(const blam_file_header* file)
{
    const blam_tag_index* ptr = blam_tag_index_ptr(file);
    return (const blam_index_item*)&(ptr[1]);
}

//Create a copy of the tag index, do the magic index trick
static blam_tag_index blam_tag_index_get(const blam_file_header* file)
{
    const blam_tag_index* tgi = blam_tag_index_ptr(file);
    blam_tag_index dupe;
    memcpy(&dupe,tgi,sizeof(blam_tag_index));
    blam_tag_index_magic(&dupe,file->tagIndexOffset);
    return dupe;
}

//For magic pointers
static const void* blam_magic_ptr(const void* base, int32 magic, int32 offset)
{
    return ((const byte*)base)+offset-magic;
}

//For getting the name of a tag
static cstring blam_index_item_get_string(
        const blam_index_item* idx,     //index item, obviously
        const blam_file_header* map,    //pointer for the map
        const blam_tag_index* tagindex  //for the index magic
        )
{
    return ((const byte*)map)+idx->stringOffset-tagindex->index_magic;
}

}
}

#endif //CBLAM_MAP
