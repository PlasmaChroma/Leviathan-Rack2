#pragma once
#include <algorithm>
#include <atomic>
#include <cmath>

namespace sil {
// Matches Octavia's live meters: 5 V full scale, 100 ms peaks and
// per-channel K-weighted 400 ms momentary loudness. Audio owns filter/history;
// UI reads only atomically published measurements. No sample buffers.
struct StereoLevelMeter {
    struct Coeffs { double b0=1., b1=0., b2=0., a1=0., a2=0.; };
    Coeffs shelf, highPass;
    double z[2][4] = {};
    double sum[2] = {}, peak[2] = {};
    float blocks[2][4] = {};
    std::atomic<float> power[2] {}, peaks[2] {};
    std::atomic<bool> connected {false};
    int frames=0, blockTarget=1, write=0, count=0;
    static Coeffs makeHighPass(float fs, float hz, float q) {
        const double k = tan(3.14159265358979323846 * hz / fs);
        const double a0 = 1.0 + k / q + k * k;
        Coeffs f;
        f.b0 = 1.0 / a0; f.b1 = -2.0 / a0; f.b2 = f.b0;
        f.a1 = 2.0 * (k * k - 1.0) / a0;
        f.a2 = (1.0 - k / q + k * k) / a0;
        return f;
    }

    static Coeffs makeHighShelf(float fs, float hz, float gainDb) {
        constexpr double kShelfQ = 0.7071752369554196;
        constexpr double kVbExponent = 0.4996667741545416;
        const double k = tan(3.14159265358979323846 * hz / fs);
        const double vh = pow(10.0, gainDb / 20.0);
        const double vb = pow(vh, kVbExponent);
        const double a0 = 1.0 + k / kShelfQ + k * k;
        Coeffs f;
        f.b0 = (vh + vb * k / kShelfQ + k * k) / a0;
        f.b1 = 2.0 * (k * k - vh) / a0;
        f.b2 = (vh - vb * k / kShelfQ + k * k) / a0;
        f.a1 = 2.0 * (k * k - 1.0) / a0;
        f.a2 = (1.0 - k / kShelfQ + k * k) / a0;
        return f;
    }


    void configure(float sr) {
        shelf=makeHighShelf(sr,1681.974450955533f,3.999843853973347f);
        highPass=makeHighPass(sr,38.13547087602444f,.5003270373238773f);
        blockTarget=std::max(1,int(std::round(sr*.1f)));
        frames=write=count=0;
        for(int j=0;j<2;++j) {
            sum[j]=peak[j]=0.;
            for(auto& v:z[j]) v=0.;
            for(auto& v:blocks[j]) v=0.f;
            power[j].store(0.f,std::memory_order_relaxed);
            peaks[j].store(0.f,std::memory_order_relaxed);
        }
        connected.store(false,std::memory_order_relaxed);
    }
    void process(float l,float r,bool active) {
        const float input[2]={l,r};
        for(int j=0;j<2;++j) {
            const double in=input[j]*.2;
            peak[j]=std::max(peak[j],std::fabs(in));
            const double y=shelf.b0*in+z[j][0];
            z[j][0]=shelf.b1*in-shelf.a1*y+z[j][1];
            z[j][1]=shelf.b2*in-shelf.a2*y;
            const double k=highPass.b0*y+z[j][2];
            z[j][2]=highPass.b1*y-highPass.a1*k+z[j][3];
            z[j][3]=highPass.b2*y-highPass.a2*k;
            sum[j]+=k*k;
        }
        if(++frames<blockTarget) return;
        count=std::min(count+1,4);
        for(int j=0;j<2;++j) {
            blocks[j][write]=float(sum[j]/frames);
            float total=0.f;
            for(int i=0;i<4;++i) total+=blocks[j][i];
            power[j].store(total/count,std::memory_order_relaxed);
            peaks[j].store(float(peak[j]),std::memory_order_relaxed);
            sum[j]=peak[j]=0.;
        }
        connected.store(active,std::memory_order_relaxed);
        write=(write+1)%4; frames=0;
    }
};
} // namespace sil
