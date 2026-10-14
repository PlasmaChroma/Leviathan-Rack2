#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace sibyl {
enum class AutomationClock { SCENE_REPEAT, SCENE_VISIT, ARRANGEMENT };
enum class AutomationShape { STEP, LINEAR, SMOOTHSTEP };
struct AutomationPoint {
    double beat = 0., value = 0.;
    AutomationShape shape = AutomationShape::LINEAR;
    bool operator==(const AutomationPoint& b) const { return beat==b.beat && value==b.value && shape==b.shape; }
};
struct AutomationSegment {
    double begin=0., end=0., inverseDuration=0.;
    double a=0., b=0., c=0., d=0.;
};
struct AutomationCurve {
    std::string id, trackId, sceneId;
    int channel=0, lane=0;
    AutomationClock clock=AutomationClock::SCENE_VISIT;
    bool add=false, enabled=true;
    double transitionMs=0., duration=0.;
    std::vector<AutomationPoint> points;
    std::vector<AutomationSegment> segments;
    bool audibleEquals(const AutomationCurve& b) const {
        return trackId==b.trackId && sceneId==b.sceneId && channel==b.channel && lane==b.lane &&
            clock==b.clock && add==b.add && enabled==b.enabled && transitionMs==b.transitionMs &&
            duration==b.duration && points==b.points;
    }
};
// Control-side compiler builds these indices; -1 means legacy note ownership.
using AutomationRoute = std::array<int,48>;

inline double sampleAutomation(const AutomationCurve& curve, double beat, size_t& cursor) {
    if (curve.segments.empty() || beat >= curve.points.back().beat) return curve.points.back().value;
    if (beat <= 0.) { cursor=0; return curve.points.front().value; }
    if (cursor>=curve.segments.size() || beat<curve.segments[cursor].begin || beat>=curve.segments[cursor].end) {
        if (cursor+1<curve.segments.size() && beat>=curve.segments[cursor+1].begin && beat<curve.segments[cursor+1].end) ++cursor;
        else {
            // A seek jumps directly to its interval, including many skipped points.
            size_t lo=0,hi=curve.segments.size();
            while(lo<hi) { size_t mid=lo+(hi-lo)/2; if(curve.segments[mid].end<=beat) lo=mid+1; else hi=mid; }
            cursor=std::min(lo,curve.segments.size()-1);
        }
    }
    const auto& s=curve.segments[cursor];
    const double u=std::max(0.,std::min(1.,(beat-s.begin)*s.inverseDuration));
    return s.a+u*(s.b+u*(s.c+u*s.d));
}

struct AutomationLaneState {
    int owner=-1, revision=-1;
    uint64_t scopeToken=0;
    size_t segment=0;
    double outgoingMs=0., blendFrom=0., rate=0.;
    int64_t blendSample=0, blendSamples=0;
    bool blending=false;

    // Last emitted voltage is supplied by the existing physical output buffer.
    // No pointer from a previous composition generation survives here.
    double process(const AutomationCurve* curve, int nextOwner, int nextRevision,
            bool changed, double coordinate, double targetBase, double offsetMacro,
            double legacy, double previousOutput, double sampleRate, uint64_t nextScope=0) {
        bool generationChanged=revision!=nextRevision;
        bool handover=changed || (owner<0)!=(nextOwner<0) || (!generationChanged && owner!=nextOwner) ||
            (curve && scopeToken!=nextScope);
        if(generationChanged || owner!=nextOwner) segment=0;
        if(handover) {
            double ms=curve?curve->transitionMs:outgoingMs;
            blendFrom=previousOutput;blendSample=0;blendSamples=int64_t(std::ceil(ms*sampleRate/1000.));
            blending=blendSamples>0;rate=sampleRate;
        } else if(blending && rate!=sampleRate) {
            double remaining=double(std::max(int64_t(0),blendSamples-std::max(int64_t(0),blendSample-1)))/rate;
            blendSamples=int64_t(std::ceil(remaining*sampleRate));blendSample=0;
            blendFrom=previousOutput;blending=blendSamples>0;rate=sampleRate;
        }
        owner=nextOwner;revision=nextRevision;scopeToken=nextScope;
        if(curve) outgoingMs=curve->transitionMs;
        double target=curve ? sampleAutomation(*curve,coordinate,segment)+(curve->add?targetBase:0.)+offsetMacro : legacy;
        double output=target;
        if(blending) {
            double weight=double(blendSample)/double(blendSamples);
            output=blendFrom+(target-blendFrom)*weight;
            if(blendSample>=blendSamples) blending=false; else ++blendSample;
        }
        return std::max(-10.,std::min(10.,output));
    }
};
} // namespace sibyl
