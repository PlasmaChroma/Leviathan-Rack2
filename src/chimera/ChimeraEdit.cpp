#include "ChimeraEdit.hpp"
#include <algorithm>
#include <vector>

namespace chimera { namespace edit {
namespace {
Result fail(const char* error) { Result result; result.error = error; return result; }
} // namespace

Result build(const Reel& source, const Request& request,
             std::uint32_t capacityPages, std::uint32_t reservePages) {
    if (!source.readyForWorker()) return fail("snapshot_not_ready");
    const SnapshotMetadata& meta = source.snapshotMetadata();
    if (!capacityPages || capacityPages > kMaxPages || reservePages > capacityPages ||
        meta.validFrames > capacityPages * kPageFrames)
        return fail("invalid_edit_capacity");
    if (request.kind != ClearReel && request.splice >= meta.markerCount)
        return fail("invalid_splice_index");
    const std::uint32_t begin = request.kind == ClearReel ? 0 :
        meta.markers[request.splice].frame;
    const std::uint32_t end = request.kind == ClearReel ? meta.validFrames :
        (request.splice + 1 < meta.markerCount ?
         meta.markers[request.splice + 1].frame : meta.validFrames);
    if ((request.kind == MoveMarker || request.kind == RemoveMarker) &&
        request.splice == 0) return fail("frame_zero_marker_is_fixed");
    if (request.kind == MoveMarker &&
        (request.frame <= meta.markers[request.splice - 1].frame ||
         request.frame >= end)) return fail("moved_marker_crosses_neighbor");
    std::vector<Marker> markers;
    if (request.kind != ClearReel) {
        for (std::uint16_t i = 0; i < meta.markerCount; ++i) {
            if ((request.kind == DeleteSplice || request.kind == RemoveMarker) &&
                i == request.splice) continue;
            Marker marker = meta.markers[i];
            if (request.kind == DeleteSplice && i > request.splice)
                marker.frame -= end - begin;
            if (request.kind == MoveMarker && i == request.splice)
                marker.frame = request.frame;
            markers.push_back(marker);
        }
    }
    const std::uint32_t outputFrames = request.kind == ClearReel ? 0 :
        (request.kind == DeleteSplice ? meta.validFrames - (end - begin) :
         meta.validFrames);
    if (outputFrames && (markers.empty() || markers[0].frame != 0 ||
        markers.size() > kMaxSplices)) return fail("invalid_edit_markers");
    Result result;
    try { result.reel.reset(new Reel(capacityPages, reservePages)); }
    catch (...) { return fail("edit_allocation_failed"); }
    for (std::uint32_t frame = 0; frame < outputFrames; ++frame) {
        const std::uint32_t sourceFrame = request.kind == DeleteSplice && frame >= begin ?
            frame + end - begin : frame;
        StereoFrame sample = source.readSnapshot(sourceFrame);
        if (request.kind == EraseSplice && frame >= begin && frame < end)
            sample = {0.f, 0.f};
        if (!result.reel->appendImported(sample))
            return fail("edit_write_failed");
    }
    // This destination is exclusively worker-owned and sequential. Build the
    // playback moments once instead of maintaining all levels for every frame.
    result.reel->finishImport();
    if (outputFrames && !result.reel->replaceMarkers(markers.data(),
        std::uint16_t(markers.size()))) return fail("edit_marker_replace_failed");
    result.reel->restoreRevisions(meta.documentRevision + 1,
        meta.audioRevision +
        ((request.kind == EraseSplice || request.kind == DeleteSplice ||
          request.kind == ClearReel) ? 1 : 0));
    return result;
}

} } // namespace chimera::edit
