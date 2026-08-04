#pragma once

#include <cstddef>
#include <cstdint>

namespace nsRenderStats {

namespace detail {
extern bool enabled;
void AddDrawCall();
void AddVertexUpload(std::size_t bytes);
void AddIndexUpload(std::size_t bytes);
void AddUniformUpdate();
void AddSamplerBind();
void AddProgramBind();
void AddBufferStorageAllocation(std::size_t bytes);
void SetDynamicBufferHighWater(std::size_t bytes);
void SetSwapInterval(std::uint64_t intervalNs);
}

void BeginFrame(bool enabled);
void AfterUpdateStep();
void AfterUpdate();
void BeforeRender();
void BeforeSwap();
void AfterSwap();
void FinishFrame();

inline void AddDrawCall() {
    if (detail::enabled) detail::AddDrawCall();
}
inline void AddVertexUpload(const std::size_t bytes) {
    if (detail::enabled) detail::AddVertexUpload(bytes);
}
inline void AddIndexUpload(const std::size_t bytes) {
    if (detail::enabled) detail::AddIndexUpload(bytes);
}
inline void AddUniformUpdate() {
    if (detail::enabled) detail::AddUniformUpdate();
}
inline void AddSamplerBind() {
    if (detail::enabled) detail::AddSamplerBind();
}
inline void AddProgramBind() {
    if (detail::enabled) detail::AddProgramBind();
}
inline void AddBufferStorageAllocation(const std::size_t bytes) {
    if (detail::enabled) detail::AddBufferStorageAllocation(bytes);
}
inline void SetDynamicBufferHighWater(const std::size_t bytes) {
    if (detail::enabled) detail::SetDynamicBufferHighWater(bytes);
}
inline void SetSwapInterval(const std::uint64_t intervalNs) {
    if (detail::enabled) detail::SetSwapInterval(intervalNs);
}

}
