#include "RenderStats.h"

#include "Memory.h"
#include "time/Time.h"
#include "nsLib/log.h"

#include <algorithm>

namespace {
struct RenderStatsState {
    bool enabled = false;
    std::uint64_t periodStartMs = 0;
    std::uint64_t frames = 0;
    std::uint64_t updateUs = 0;
    std::uint64_t catchUpUs = 0;
    std::uint64_t updateSteps = 0;
    std::uint64_t renderUs = 0;
    std::uint64_t swapUs = 0;
    std::uint64_t drawCalls = 0;
    std::uint64_t vertexUploads = 0;
    std::uint64_t indexUploads = 0;
    std::uint64_t uploadBytes = 0;
    std::uint64_t uniformUpdates = 0;
    std::uint64_t samplerBinds = 0;
    std::uint64_t programBinds = 0;
    std::uint64_t storageAllocations = 0;
    std::uint64_t storageBytes = 0;
    std::uint64_t dynamicBufferHighWaterBytes = 0;
    std::uint64_t swapIntervalNs = 0;
};

struct FrameTimingState {
    bool active = false;
    bool updateFinished = false;
    bool renderStarted = false;
    bool renderFinished = false;
    bool swapFinished = false;
    std::uint64_t updateStartUs = 0;
    std::uint64_t firstUpdateEndUs = 0;
    std::uint64_t updateEndUs = 0;
    std::uint64_t renderStartUs = 0;
    std::uint64_t renderEndUs = 0;
    std::uint64_t swapEndUs = 0;
    unsigned int updateSteps = 0;
};

RenderStatsState g_stats;
FrameTimingState g_frame;

void ResetPeriod(const std::uint64_t nowMs) {
    const bool enabled = g_stats.enabled;
    g_stats = {};
    g_stats.enabled = enabled;
    g_stats.periodStartMs = nowMs;
}

std::uint64_t Elapsed(const std::uint64_t start, const std::uint64_t end) {
    return end >= start ? end - start : 0;
}
}

namespace nsRenderStats {

namespace detail {
bool enabled = false;

void AddDrawCall() { ++g_stats.drawCalls; }
void AddVertexUpload(const std::size_t bytes) {
    ++g_stats.vertexUploads;
    g_stats.uploadBytes += bytes;
}
void AddIndexUpload(const std::size_t bytes) {
    ++g_stats.indexUploads;
    g_stats.uploadBytes += bytes;
}
void AddUniformUpdate() { ++g_stats.uniformUpdates; }
void AddSamplerBind() { ++g_stats.samplerBinds; }
void AddProgramBind() { ++g_stats.programBinds; }
void AddBufferStorageAllocation(const std::size_t bytes) {
    ++g_stats.storageAllocations;
    g_stats.storageBytes += bytes;
}
void SetDynamicBufferHighWater(const std::size_t bytes) {
    g_stats.dynamicBufferHighWaterBytes = std::max(g_stats.dynamicBufferHighWaterBytes,
                                                   static_cast<std::uint64_t>(bytes));
}
void SetSwapInterval(const std::uint64_t intervalNs) { g_stats.swapIntervalNs = intervalNs; }
}

void BeginFrame(const bool enabled) {
    if (detail::enabled == enabled) {
        g_frame = {};
    } else {
        g_stats = {};
        detail::enabled = enabled;
        g_stats.enabled = enabled;
        g_frame = {};
    }

    if (!enabled) return;

    g_frame.active = true;
    g_frame.updateStartUs = nsTime::GetTimeUS();
}

void AfterUpdateStep() {
    if (!g_frame.active || g_frame.updateFinished) return;

    ++g_frame.updateSteps;
    if (g_frame.updateSteps == 1) {
        g_frame.firstUpdateEndUs = nsTime::GetTimeUS();
    }
}

void AfterUpdate() {
    if (!g_frame.active || g_frame.updateFinished) return;

    g_frame.updateEndUs = nsTime::GetTimeUS();
    g_frame.updateFinished = true;
}

void BeforeRender() {
    if (!g_frame.active || g_frame.renderStarted) return;

    g_frame.renderStartUs = nsTime::GetTimeUS();
    g_frame.renderStarted = true;
}

void BeforeSwap() {
    if (!g_frame.active || !g_frame.renderStarted || g_frame.renderFinished) return;

    g_frame.renderEndUs = nsTime::GetTimeUS();
    g_frame.renderFinished = true;
}

void AfterSwap() {
    if (!g_frame.active || !g_frame.renderFinished || g_frame.swapFinished) return;

    g_frame.swapEndUs = nsTime::GetTimeUS();
    g_frame.swapFinished = true;
}

void FinishFrame() {
    if (!g_frame.active || !g_stats.enabled) return;

    const std::uint64_t nowMs = nsTime::GetTimeMS();
    const std::uint64_t updateUs = g_frame.updateFinished
        ? Elapsed(g_frame.updateStartUs, g_frame.updateEndUs)
        : 0;
    const std::uint64_t catchUpUs = g_frame.updateFinished && g_frame.updateSteps > 1
        ? Elapsed(g_frame.firstUpdateEndUs, g_frame.updateEndUs)
        : 0;
    const std::uint64_t renderUs = g_frame.renderFinished
        ? Elapsed(g_frame.renderStartUs, g_frame.renderEndUs)
        : 0;
    const std::uint64_t swapUs = g_frame.swapFinished
        ? Elapsed(g_frame.renderEndUs, g_frame.swapEndUs)
        : 0;

    if (!g_stats.periodStartMs) g_stats.periodStartMs = nowMs;

    ++g_stats.frames;
    g_stats.updateUs += updateUs;
    g_stats.catchUpUs += catchUpUs;
    g_stats.updateSteps += g_frame.updateSteps;
    g_stats.renderUs += renderUs;
    g_stats.swapUs += swapUs;

    const std::uint64_t elapsedMs = nowMs - g_stats.periodStartMs;
    if (elapsedMs < 1000 || !g_stats.frames) {
        g_frame.active = false;
        return;
    }

    const double frames = static_cast<double>(g_stats.frames);
    const double fps = static_cast<double>(g_stats.frames) * 1000.0 / static_cast<double>(elapsedMs);
    nsMemoryLoopAllocScope reportAllocationScope;
    Log::Info(
        "Render stats: fps=%.1f update=%.2fms catchup=%.2fms steps=%.2f render=%.2fms swap=%.2fms "
        "draws=%.1f vbo=%.1f ebo=%.1f upload=%.1fKB uniforms=%.1f samplers=%.1f programs=%.1f "
        "storage=%.1f/%.1fKB vbslot=%.1fKB swappy=%.2fms",
        fps,
        static_cast<double>(g_stats.updateUs) / frames / 1000.0,
        static_cast<double>(g_stats.catchUpUs) / frames / 1000.0,
        static_cast<double>(g_stats.updateSteps) / frames,
        static_cast<double>(g_stats.renderUs) / frames / 1000.0,
        static_cast<double>(g_stats.swapUs) / frames / 1000.0,
        static_cast<double>(g_stats.drawCalls) / frames,
        static_cast<double>(g_stats.vertexUploads) / frames,
        static_cast<double>(g_stats.indexUploads) / frames,
        static_cast<double>(g_stats.uploadBytes) / frames / 1024.0,
        static_cast<double>(g_stats.uniformUpdates) / frames,
        static_cast<double>(g_stats.samplerBinds) / frames,
        static_cast<double>(g_stats.programBinds) / frames,
        static_cast<double>(g_stats.storageAllocations) / frames,
        static_cast<double>(g_stats.storageBytes) / frames / 1024.0,
        static_cast<double>(g_stats.dynamicBufferHighWaterBytes) / 1024.0,
        static_cast<double>(g_stats.swapIntervalNs) / 1000000.0);

    ResetPeriod(nowMs);
    g_frame.active = false;
}

}
