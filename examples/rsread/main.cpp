#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <peripherals/libc/types.h>

using namespace Coffee;

struct C_FORCE_PACKING RSCacheSectorHeader
{
    int16  fileNumber;
    int16  filePartNumber;
    uint24 sectorNumber;
    uint8  cacheNumber;
};

struct RSCacheSector
{
    uint8 data[512];
};

struct C_FORCE_PACKING RSIndexEntry
{
    uint24 fileSize;
    uint24 sectorAt;
};

struct C_FORCE_PACKING RSArchiveHeader
{
    uint24 decompSize;
    uint24 compSize;
};

struct C_FORCE_PACKING RSArchiveFileHeader
{
    int    fileId;
    uint24 decompSize;
    uint24 compSize;
};

struct C_FORCE_PACKING RSImgIndexEntry
{
    uint16  maxWidth;
    uint16  maxHeight;
    uint8   colorCount;
    uint24* colorMap;
};

struct C_FORCE_PACKING RSIndexImgEntry
{
    uint8  drawOffsetX;
    uint8  drawOffsetY;
    uint16 width;
    uint16 height;
    uint8  packType;
};

struct C_FORCE_PACKING RSBadEncWord
{
    uint8 wordLen;
    int8* wordChars;
    int8  unk1;
    int8  unk2;
    int8  unk3;
};

struct C_FORCE_PACKING RSTLDEnc
{
    int8  unk;
    uint8 tldLen;
    int8* tldChars;
};

struct C_FORCE_PACKING RSDomainEnc
{
    uint8 domLen;
    int8* domChars;
};

struct C_FORCE_PACKING RSFloorConfig
{
    cstring name;
    uint24  mapColor;
    uint24  actColor;
    uint8   texture;
    bool    unknown;
    bool    occlude;
};

struct C_FORCE_PACKING RSItemConfig
{
    int     modelId;
    int     value;
    cstring desc;
    bool    memberOnly;
    int     templateId;
    cstring actions[5];
    cstring groundActions[5];
    int     certId;
    bool    isMembers;
    int     lightMod;
    int     shadowMod;
    int     modelSizeX;
    int     modelSizeY;
    int     modelSizeZ;
    bool    stackable;
    int     stackId[10];
    int     stackAmounts[10];
    int     modelInvZoom;
    int     modelInvRotX;
    int     modelInvRotY;
    int     modelInvOffsetX;
    int     modelInvOffsetY;
    int     team;
    int     diagRot;
    int8    maleYOffset;
    int8    femaleYOffset;
    int     maleWornModelId;
    int     femaleWornModelId;
    int     femaleEmblem;
    int     maleEmblem;
    int     femaleArmsId;
    int     maleArmsId;
    int     maleDialg;
    int     femaleDialg;
    int     maleDialgHat;
    int     femaleDialgHat;
    int     lendId;
    int     lentItemId;
    int     modifiedModelColors[256];
    int     originalModelColors[256];
};

int32 coffee_main(int32, cstring_w*)
{
    FileResourcePrefix("sample_data/runescape-data/");

    /* Looking at member offsets, if they are packed */
    cDebug(
        "Offsets: &name={0}, &mapColor={1}, &actColor={2}, &texture={3}",
        offsetof(RSFloorConfig, name),
        offsetof(RSFloorConfig, mapColor),
        offsetof(RSFloorConfig, actColor),
        offsetof(RSFloorConfig, texture));

    Resource idxfile("main_file_cache.idx0");
    FilePull(idxfile);

    RSCacheSectorHeader* t1 = C_CAST<RSCacheSectorHeader*>(idxfile.data);
    RSCacheSector*       t3 = C_FCAST<RSCacheSector*>(&t1[1]);

    cDebug("Test {0}", t1->cacheNumber);
    cDebug("Test {0}", t3->data[0]);

    FileFree(idxfile);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
