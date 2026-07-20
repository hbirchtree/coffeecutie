#pragma once

#include <chrono>
#include <fstream>
#include <string_view>

#include <coffee/components/components.h>
#include <peripherals/stl/json.h>
#include <platforms/environment.h>
#include <url/url.h>

/* Per-process test/debug journal: appends one JSON object per line to
 * <TMPDIR>/journal.jsonl. Purely local — nothing goes over the network;
 * the multi-process harness (dummy plug "spawn" support) gives each
 * process its own TMPDIR, and .github/tests/net/compare_journals.py diffs
 * the resulting files offline. JSONL rather than one big JSON array so a
 * crash or SIGKILL keeps every entry written up to that point (each line
 * is flushed as it's recorded) — post-mortems are half the point.
 *
 * Entry shape: {"t_ms": <ms since journal open>, "type": "...", "data": {...}}
 *
 * Enabled when a dummy plug config is present (DUMMY_PLUG_CONFIG) or
 * explicitly via COFFEE_JOURNAL=1; record() is a no-op otherwise, so call
 * sites don't need their own gating. */
struct Journal : compo::SubsystemBase
{
    using type = Journal;

    Journal()
    {
        m_enabled = platform::env::var("DUMMY_PLUG_CONFIG").has_value() ||
                    platform::env::var("COFFEE_JOURNAL").has_value();
        if(!m_enabled)
            return;
        using platform::url::constructors::MkUrl;
        auto path = *MkUrl("journal.jsonl", semantic::RSCA::TempFile);
        m_out.open(path, std::ios::out | std::ios::trunc);
        m_enabled = m_out.is_open();
        m_epoch   = std::chrono::steady_clock::now();
    }

    bool enabled() const
    {
        return m_enabled;
    }

    void record(std::string_view type, nlohmann::json data = {})
    {
        if(!m_enabled)
            return;
        using namespace std::chrono;
        nlohmann::json entry{
            {"t_ms",
             duration_cast<milliseconds>(steady_clock::now() - m_epoch)
                 .count()},
            {"type", type},
        };
        if(!data.is_null())
            entry["data"] = std::move(data);
        m_out << entry.dump() << '\n';
        m_out.flush();
    }

  private:
    std::ofstream                         m_out;
    bool                                  m_enabled{false};
    std::chrono::steady_clock::time_point m_epoch{};
};
