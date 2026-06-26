#pragma once

/* Demand-paged virtual memory on the GameCube (PowerPC 750 / Gekko).
 *
 * See PAGED_MMAP.md for the design and staged plan. A 256MB virtual window at
 * 0x90000000 (segment 9, no BAT) is translated through a hashed page table we
 * own; the first access to each page raises a DSI, and a pure-C handler fills a
 * frame from backing storage, installs a PTE and returns -- retrying the
 * instruction. Multiple files can be mapped at once: each gets a sub-range of
 * the window, and the fault handler dispatches by address to the owning region.
 *
 * Backing per region: ARAM (file staged there at map time, paged in via polled
 * AR DMA), DVD-direct (polled DI read, when the file doesn't fit ARAM), or an
 * in-RAM buffer (tests). Pages are filled into a shared frame pool with FIFO
 * eviction; unmapping a region frees its frames.
 */

#include <platforms/gekko/dvd.h> // FST lookup for DVD-backed regions

#include <tuxedo/ppc/context.h>
#include <tuxedo/ppc/exception.h>
#include <tuxedo/ppc/intrinsics.h>
#include <tuxedo/ppc/spr.h>

#include <ogc/aram.h>
#include <ogc/cache.h>

#include <gctypes.h>
#include <malloc.h>
#include <string.h>

namespace platform::file::gekko::vmem {

namespace detail {

constexpr u32 kHtabSize   = 0x10000;     // 64KB page table = 1024 PTEGs * 64B
constexpr u32 kPageSize   = 0x1000;
constexpr u32 kWindowBase = 0x90000000;  // segment 9: free on GC, no BAT
constexpr u32 kWindowSpan = 0x10000000;  // 256MB (one segment)
constexpr u32 kWindowVsid = 9;
constexpr u32 kPoolPages  = 64;          // 256KB shared frame pool

// Max files mapped concurrently. Set via CMake (GEKKO_MMAP_MAX_REGIONS); a fixed
// array so the DSI handler can scan it without allocating.
#ifndef GEKKO_MMAP_MAX_REGIONS
#define GEKKO_MMAP_MAX_REGIONS 8
#endif
constexpr u32 kMaxRegions = GEKKO_MMAP_MAX_REGIONS;

// GameCube ARAM is 16MB; skip the bottom 64KB. AR_StartDMA/AR_GetDMAStatus are
// register pokes that work without AR_Init (whose size probe hangs waiting on the
// DSP, which the app never starts).
constexpr u32 kAramSize = 0x1000000;
constexpr u32 kAramBase = 0x10000;

struct pte_t
{
    u32 hi;
    u32 lo;
};

struct region_t
{
    bool active   = false;
    bool dvd      = false;   // true: DVD/ARAM-backed; false: in-RAM alias
    u32  base     = 0;       // window VA assigned to this region
    u32  size     = 0;
    u8*  ram      = nullptr; // in-RAM backing
    u32  disc_off = 0;       // DVD-backed: file's disc byte offset
    bool aram     = false;   // staged in ARAM: page in via DMA, not DVD
    u32  aram_off = 0;       // ARAM offset of the staged file
};

inline pte_t*          g_htab       = nullptr;
inline u32             g_htab_phys  = 0;
inline PPCExcptPanicFn g_prev_panic = nullptr;
inline bool            g_init       = false;

/* First-fit range allocator with coalescing free, for VA + ARAM. Few regions, so
 * a small fixed hole list is plenty. off 0 is reserved to mean "out of space"
 * (both pools start at a nonzero base). */
struct range_alloc
{
    struct hole
    {
        u32 off;
        u32 size;
    };
    hole h[2 * kMaxRegions];
    u32  n = 0;

    void init(u32 base, u32 size)
    {
        h[0] = {base, size};
        n    = 1;
    }

    u32 alloc(u32 size)
    {
        for(u32 i = 0; i < n; i++)
            if(h[i].size >= size)
            {
                u32 const off = h[i].off;
                h[i].off += size;
                h[i].size -= size;
                if(h[i].size == 0)
                {
                    for(u32 j = i; j + 1 < n; j++)
                        h[j] = h[j + 1];
                    n--;
                }
                return off;
            }
        return 0;
    }

    void free(u32 off, u32 size)
    {
        if(n >= 2 * kMaxRegions)
            return; // give up (leak) -- shouldn't happen with few regions
        h[n++] = {off, size};
        for(u32 i = 1; i < n; i++) // insertion sort by off
        {
            hole k = h[i];
            u32   j = i;
            while(j > 0 && h[j - 1].off > k.off)
            {
                h[j] = h[j - 1];
                j--;
            }
            h[j] = k;
        }
        u32 w = 0; // merge adjacent
        for(u32 i = 1; i < n; i++)
        {
            if(h[w].off + h[w].size == h[i].off)
                h[w].size += h[i].size;
            else
                h[++w] = h[i];
        }
        n = w + 1;
    }
};

inline region_t    g_regions[kMaxRegions];
inline range_alloc g_va;   // window VA allocator
inline range_alloc g_aram; // ARAM allocator

inline u8*    g_pool                  = nullptr;
inline u32    g_pool_next             = 0;
inline u32    g_evict_next            = 0;
inline u32    g_frame_va[kPoolPages]  = {}; // VA each frame holds (0 = free)
inline pte_t* g_frame_pte[kPoolPages] = {}; // its PTE slot

inline u32  g_fault_count = 0;
inline bool g_failed      = false;

inline u32 align_up(u32 v)
{
    return (v + kPageSize - 1) & ~(kPageSize - 1);
}

inline u32 virt_to_phys(void const* v)
{
    return reinterpret_cast<u32>(v) & 0x0FFFFFFF;
}

inline void set_sr(u32 seg, u32 value)
{
    u32 ea = seg << 28;
    __asm__ __volatile__("mtsrin %0,%1; isync" ::"r"(value), "r"(ea) : "memory");
}

inline void tlbie(u32 ea)
{
    __asm__ __volatile__("tlbie %0; eieio; tlbsync; sync" ::"r"(ea) : "memory");
}

/* Install a primary-hash PTE: page-aligned va -> page-aligned phys, cached rw.
 * Returns the PTE slot written (so it can be invalidated on eviction). */
inline pte_t* map_page(u32 va, u32 phys)
{
    u32 const vsid = kWindowVsid;
    u32 const pi   = (va >> 12) & 0xFFFF;
    u32 const api  = (pi >> 10) & 0x3F;
    u32 const hash = (vsid ^ pi) & 0x7FFFF;

    pte_t* grp = reinterpret_cast<pte_t*>(
        reinterpret_cast<u8*>(g_htab) + ((hash & 0x3FF) << 6));

    u32 const hi = 0x80000000u | (vsid << 7) | api;
    u32 const lo = (phys & 0xFFFFF000u) | 0x0002u; // WIMG=0000, PP=10 (rw)

    for(int i = 0; i < 8; i++)
        if(!(grp[i].hi & 0x80000000u))
        {
            grp[i].lo = lo;
            grp[i].hi = hi;
            DCFlushRange(&grp[i], sizeof(pte_t));
            PPCSyncInner();
            tlbie(va);
            return &grp[i];
        }
    return nullptr;
}

inline void unmap_pte(pte_t* pte, u32 va)
{
    if(!pte)
        return;
    pte->hi = 0; // clear V
    DCFlushRange(pte, sizeof(pte_t));
    PPCSyncInner();
    tlbie(va);
}

/* Polled DVD read through the Drive Interface regs (uncached 0xCC006000). No
 * interrupts -> safe in the DSI handler. offset/len/dst must be 32-byte aligned. */
inline void di_read(u32 dst_phys, u32 len, u32 disc_off)
{
    auto* DI = reinterpret_cast<volatile u32*>(0xCC006000);
    DI[0]    = DI[0];        // ack pending DI interrupt flags
    DI[2]    = 0xA8000000;   // DICMDBUF0: read
    DI[3]    = disc_off >> 2; // DICMDBUF1: offset (4-byte units)
    DI[4]    = len;          // DICMDBUF2: length
    DI[5]    = dst_phys & 0x1FFFFFFF; // DIMAR
    DI[6]    = len;          // DILENGTH
    DI[7]    = 3;            // DICR: TSTART | DMA (read)
    while(DI[7] & 1)
        ;
}

constexpr u32 kPteRef = 0x00000100; // PTE.lo Referenced bit (set by HW on access)

/* Pick a frame: a fresh one, then a freed one, else evict by clock /
 * second-chance (approximate LRU using the hardware-set PTE Referenced bit). */
inline u32 alloc_frame()
{
    if(g_pool_next < kPoolPages)
        return g_pool_next++;
    for(u32 i = 0; i < kPoolPages; i++)
        if(g_frame_va[i] == 0)
            return i;

    // At most two sweeps: the first clears R on recently-used frames, the second
    // is guaranteed to find one with R clear.
    for(u32 scan = 0; scan < 2 * kPoolPages; scan++)
    {
        u32 const idx = g_evict_next;
        g_evict_next  = (g_evict_next + 1) % kPoolPages;
        pte_t* pte    = g_frame_pte[idx];
        if(pte && (pte->lo & kPteRef))
        {
            pte->lo &= ~kPteRef; // give a second chance
            DCFlushRange(pte, sizeof(pte_t));
            tlbie(g_frame_va[idx]); // force a re-walk so the next access re-sets R
            continue;
        }
        unmap_pte(pte, g_frame_va[idx]);
        return idx;
    }
    u32 const idx = g_evict_next; // fallback (all referenced): take the cursor
    g_evict_next  = (g_evict_next + 1) % kPoolPages;
    unmap_pte(g_frame_pte[idx], g_frame_va[idx]);
    return idx;
}

inline void fill_and_map(region_t const& r, u32 va)
{
    u32 const off   = va - r.base;
    u32 const idx   = alloc_frame();
    u8*       frame = g_pool + idx * kPageSize;

    if(r.aram)
    {
        AR_StartDMA(
            AR_ARAMTOMRAM, virt_to_phys(frame), r.aram_off + off, kPageSize);
        while(AR_GetDMAStatus())
            ;
        DCInvalidateRange(frame, kPageSize);
    } else if(r.dvd)
    {
        di_read(virt_to_phys(frame), kPageSize, r.disc_off + off);
        PPCSyncInner();
        DCInvalidateRange(frame, kPageSize);
    } else
    {
        memcpy(frame, r.ram + off, kPageSize);
        DCFlushRange(frame, kPageSize);
    }

    g_frame_pte[idx] = map_page(va, virt_to_phys(frame));
    g_frame_va[idx]  = va;
}

inline void dsi_handler(unsigned exid, PPCContext* ctx)
{
    if(exid == PPC_EXCPT_DSI)
    {
        u32 const dar = PPCMfspr(DAR);
        for(u32 i = 0; i < kMaxRegions; i++)
        {
            region_t const& r = g_regions[i];
            if(r.active && dar >= r.base && dar < r.base + r.size)
            {
                g_fault_count++;
                if(g_fault_count > 0x40000) // safety valve: never hang headless
                {
                    g_failed = true;
                    ctx->pc += 4;
                    return;
                }
                fill_and_map(r, dar & ~(kPageSize - 1));
                return;
            }
        }
    }
    g_prev_panic(exid, ctx);
}

inline void ensure_init()
{
    if(g_init)
        return;

    g_htab = static_cast<pte_t*>(memalign(kHtabSize, kHtabSize));
    memset(g_htab, 0, kHtabSize);
    DCFlushRange(g_htab, kHtabSize);
    g_htab_phys = virt_to_phys(g_htab);

    g_pool = static_cast<u8*>(memalign(kPageSize, kPoolPages * kPageSize));

    g_va.init(kWindowBase, kWindowSpan);
    g_aram.init(kAramBase, kAramSize - kAramBase);

    PPCMtspr(SDR1, g_htab_phys & 0xFFFF0000u);
    PPCIsync();
    set_sr(kWindowBase >> 28, kWindowVsid);

    g_prev_panic       = PPCExcptCurPanicFn;
    PPCExcptCurPanicFn = dsi_handler;
    PPCSyncInner();
    g_init = true;
}

/* Stage a disc file into ARAM at aram_off (caller-reserved). DVD -> RAM bounce
 * -> ARAM via polled DMA, at map time (normal context). */
inline bool stage_to_aram(u32 disc_off, u32 size, u32 aram_off)
{
    u32 const need = (size + 31) & ~31u;

    constexpr u32 kChunk = 0x8000; // 32KB DVD->RAM->ARAM staging chunk
    u8* const     bounce = static_cast<u8*>(memalign(32, kChunk));
    if(!bounce)
        return false;
    for(u32 pos = 0; pos < need; pos += kChunk)
    {
        u32 const n = (need - pos < kChunk) ? (need - pos) : kChunk;
        dvd::detail::read_at(bounce, n, disc_off + pos);
        DCFlushRange(bounce, n);
        AR_StartDMA(AR_MRAMTOARAM, virt_to_phys(bounce), aram_off + pos, n);
        while(AR_GetDMAStatus())
            ;
    }
    free(bounce);
    return true;
}

/* Free the pool frames + PTEs belonging to a window range (on unmap). */
inline void free_region_frames(u32 base, u32 size)
{
    for(u32 i = 0; i < kPoolPages; i++)
        if(g_frame_va[i] >= base && g_frame_va[i] < base + size)
        {
            unmap_pte(g_frame_pte[i], g_frame_va[i]);
            g_frame_va[i]  = 0;
            g_frame_pte[i] = nullptr;
        }
}

inline int find_free_region()
{
    for(u32 i = 0; i < kMaxRegions; i++)
        if(!g_regions[i].active)
            return static_cast<int>(i);
    return -1;
}

/* A contiguous, pinned-resident lock over a window range (for GX/DMA). */
struct lock_t
{
    bool active = false;
    u32  base   = 0;       // window VA start (page-aligned)
    u32  size   = 0;       // page-rounded length
    u8*  buf    = nullptr; // contiguous backing buffer
};

inline lock_t g_locks[kMaxRegions];

inline int find_free_lock()
{
    for(u32 i = 0; i < kMaxRegions; i++)
        if(!g_locks[i].active)
            return static_cast<int>(i);
    return -1;
}

/* Clear the PTE for a single window VA (used to drop a lock's 1:1 mapping). */
inline void clear_pte(u32 va)
{
    u32 const vsid = kWindowVsid;
    u32 const pi   = (va >> 12) & 0xFFFF;
    u32 const api  = (pi >> 10) & 0x3F;
    u32 const hash = (vsid ^ pi) & 0x7FFFF;
    pte_t*    grp  = reinterpret_cast<pte_t*>(
        reinterpret_cast<u8*>(g_htab) + ((hash & 0x3FF) << 6));
    u32 const want = 0x80000000u | (vsid << 7) | api;
    for(int i = 0; i < 8; i++)
        if(grp[i].hi == want)
        {
            unmap_pte(&grp[i], va);
            return;
        }
}

} // namespace detail

/* ------------------------------------------------------------------ public API
 * A mapped view: `base` points into the demand-paged window; read base[0..size)
 * and pages fault in on demand. Multiple mappings coexist. */
struct mapping
{
    u8* base = nullptr;
    u32 size = 0;

    explicit operator bool() const { return base != nullptr; }
};

/* Map a DVD file by name into the paged window. The file is staged into ARAM if
 * it fits (paged in by fast DMA), else paged directly off the disc. Returns
 * {nullptr,0} if the file is missing or the window/region table is full. */
inline mapping map(char const* name)
{
    auto const* e = dvd::detail::find(name);
    if(!e)
        return {};
    detail::ensure_init();

    int const slot = detail::find_free_region();
    if(slot < 0)
        return {};
    u32 const rounded = detail::align_up(e->size);
    u32 const base    = detail::g_va.alloc(rounded);
    if(!base)
        return {};

    bool aram = false;
    u32  aoff = detail::g_aram.alloc(rounded);
    if(aoff)
    {
        if(detail::stage_to_aram(e->off, e->size, aoff))
            aram = true;
        else
            detail::g_aram.free(aoff, rounded); // staging failed -> give it back
    }

    detail::g_regions[slot] =
        {true, true, base, e->size, nullptr, e->off, aram, aoff};
    return {reinterpret_cast<u8*>(static_cast<uintptr_t>(base)), e->size};
}

/* Drop a mapping: free its frames/PTEs, region slot, and VA/ARAM ranges. */
inline void unmap(mapping const& m)
{
    if(!m)
        return;
    u32 const base = reinterpret_cast<u32>(m.base);
    for(u32 i = 0; i < detail::kMaxRegions; i++)
    {
        auto& r = detail::g_regions[i];
        if(!r.active || r.base != base)
            continue;
        u32 const rounded = detail::align_up(r.size);
        detail::free_region_frames(r.base, r.size);
        detail::g_va.free(r.base, rounded);
        if(r.aram)
            detail::g_aram.free(r.aram_off, rounded);
        r.active = false;
        break;
    }
}

/* Page faults serviced since the last full unmap -- instrumentation. */
inline u32 fault_count()
{
    return detail::g_fault_count;
}

/* A pinned, physically-contiguous view of a window sub-range, for GX/DMA. */
struct locked_region
{
    u8* ptr  = nullptr; // contiguous cached CPU pointer (use for CPU + as a
                        // TPL/GX source: GX's virtual->physical works on this,
                        // unlike the window VA). The original window addresses
                        // also stay valid (now 1:1-backed).
    u32 phys = 0;       // physical base for GX/DMA
    u32 size = 0;       // page-rounded length

    explicit operator bool() const { return ptr != nullptr; }
};

/* Pin only the sub-range [addr, addr+len) that GX/DMA needs: fault it in, copy
 * it into a contiguous 32-byte-aligned resident buffer, and remap those window
 * pages 1:1 to it (existing window pointers stay valid and become contiguous +
 * non-evictable). The rest of the mapping stays demand-paged. Page-rounded.
 * Feed GX with `phys`; access via `ptr`. munlock to release. */
inline locked_region mlock(void* addr, u32 len)
{
    u32 const start = reinterpret_cast<u32>(addr) & ~(detail::kPageSize - 1);
    u32 const end   = detail::align_up(reinterpret_cast<u32>(addr) + len);
    u32 const n     = end - start;

    int const slot = detail::find_free_lock();
    if(slot < 0)
        return {};
    // Page-aligned: the 1:1 remap maps whole pages, so buf must start on a page.
    u8* buf = static_cast<u8*>(memalign(detail::kPageSize, n));
    if(!buf)
        return {};

    // Read the range through the window (faults pages in) into the contiguous buf.
    memcpy(
        buf,
        reinterpret_cast<void const*>(static_cast<uintptr_t>(start)),
        n);
    // Drop the scattered pool frames, then remap the window 1:1 onto buf.
    detail::free_region_frames(start, n);
    for(u32 i = 0; i < n; i += detail::kPageSize)
        detail::map_page(start + i, detail::virt_to_phys(buf + i));
    DCFlushRange(buf, n); // GX/DMA read physical RAM

    detail::g_locks[slot] = {true, start, n, buf};
    return {buf, detail::virt_to_phys(buf), n};
}

/* Release a lock taken by mlock(): drop the 1:1 mapping (so the range re-pages
 * from its backing on next access) and free the contiguous buffer. */
inline void munlock(void* addr)
{
    u32 const start = reinterpret_cast<u32>(addr) & ~(detail::kPageSize - 1);
    for(u32 i = 0; i < detail::kMaxRegions; i++)
    {
        auto& l = detail::g_locks[i];
        if(!l.active || l.base != start)
            continue;
        for(u32 off = 0; off < l.size; off += detail::kPageSize)
            detail::clear_pte(l.base + off);
        free(l.buf);
        l.active = false;
        return;
    }
}

/* ----------------------------------------------------------- development tests
 * Exercise the VM directly; not part of the asset path. */
namespace test {

/* PTE Referenced bit for a resident window page: 1 if the hardware has set R
 * since the page was mapped, 0 if not, -1 if the page isn't resident. Used to
 * check whether R-bit-driven clock-LRU is effective on this implementation. */
inline int page_referenced(void* va)
{
    u32 const a = reinterpret_cast<u32>(va) & ~(detail::kPageSize - 1);
    for(u32 i = 0; i < detail::kPoolPages; i++)
        if(detail::g_frame_va[i] == a && detail::g_frame_pte[i])
            return (detail::g_frame_pte[i]->lo & detail::kPteRef) ? 1 : 0;
    return -1;
}

/* Behavioural LRU check: fill the pool, then access a wave of new pages while
 * re-touching page 0 ("hot") each round. Clock-LRU keeps page 0 (R freshly set)
 * and evicts the cold pages; plain FIFO would evict page 0 (oldest-loaded). Pass
 * = hot page still resident AND a cold page evicted. */
inline bool lru(char const* name)
{
    auto m = gekko::vmem::map(name);
    if(!m)
        return false;
    u32 const pages = m.size / detail::kPageSize;
    u32 const N     = detail::kPoolPages;
    if(pages < 2 * N)
    {
        gekko::vmem::unmap(m);
        return false; // need more pages than the pool to force eviction
    }
    auto* w  = reinterpret_cast<volatile u8*>(m.base);
    auto  rd = [&](u32 p) { (void)w[p * detail::kPageSize]; };

    for(u32 i = 0; i < N; i++) // fill the pool (page 0 = hot)
        rd(i);
    for(u32 i = 0; i < N; i++) // wave of new pages, re-touching page 0 each round
    {
        rd(0);
        rd(N + i);
    }

    bool const hot_resident = page_referenced(m.base) != -1;
    bool const cold_evicted =
        page_referenced(m.base + detail::kPageSize) == -1;
    gekko::vmem::unmap(m);
    return hot_resident && cold_evicted;
}

inline mapping map_ram(u8* src, u32 size)
{
    detail::ensure_init();
    int const slot = detail::find_free_region();
    if(slot < 0)
        return {};
    u32 const base = detail::g_va.alloc(detail::align_up(size));
    if(!base)
        return {};
    detail::g_regions[slot] = {true, false, base, size, src, 0, false, 0};
    return {reinterpret_cast<u8*>(static_cast<uintptr_t>(base)), size};
}

/* In-RAM backing: prove fault -> fix -> resume. */
inline bool ram(u32* fault_count_out = nullptr)
{
    using namespace detail;
    constexpr u32 N   = 0x4000; // 16KB = 4 pages
    u8*           src = static_cast<u8*>(memalign(kPageSize, N));
    for(u32 i = 0; i < N; i++)
        src[i] = static_cast<u8>(i * 7 + 3);
    DCFlushRange(src, N);

    auto m  = map_ram(src, N);
    auto* w = reinterpret_cast<volatile u8*>(m.base);
    bool  ok = m && !g_failed;
    for(u32 i = 0; ok && i < N; i += 0x111)
        if(w[i] != static_cast<u8>(i * 7 + 3))
            ok = false;
    if(fault_count_out)
        *fault_count_out = g_fault_count;
    gekko::vmem::unmap(m);
    return ok;
}

/* DVD-backed: map a disc file and verify windowed reads match its bytes. */
inline bool dvd(char const* name, u32* fault_count_out = nullptr)
{
    std::vector<u8> ref;
    if(!dvd::read_file(name, ref))
        return false;

    auto m = gekko::vmem::map(name);
    if(!m)
        return false;
    auto* w  = reinterpret_cast<volatile u8*>(m.base);
    bool  ok = true;
    for(u32 i = 0; i < ref.size(); i += 0x123)
        if(w[i] != ref[i])
        {
            ok = false;
            break;
        }
    if(fault_count_out)
        *fault_count_out = detail::g_fault_count;
    gekko::vmem::unmap(m);
    return ok && !detail::g_failed;
}

} // namespace test

} // namespace platform::file::gekko::vmem
