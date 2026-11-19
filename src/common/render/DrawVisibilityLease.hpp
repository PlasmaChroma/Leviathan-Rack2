#pragma once

namespace visual_assets {
// UI-thread only. Rack steps offscreen widgets too; actual draws renew a short
// lease for preview work. One initial lease preserves first-frame preparation.
struct DrawVisibilityLease {
    static constexpr double duration=.25;
    double expiresAt=0;
    bool started=false;

    void renew(double now) {expiresAt=now+duration;started=true;}
    bool active(double now) {
        if(!started)renew(now);
        return now<=expiresAt;
    }
};
}
