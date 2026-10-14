#!/usr/bin/env python3
"""Reproduce isolated mathematical checks for Vessel's expander-branch audit.
This is NOT a build or end-to-end audio-quality test of the Rack module.
Requires numpy, scipy and mpmath. Writes coefficients and numerical_results.json.
"""
from pathlib import Path
import json
import platform
import numpy as np
import scipy
from scipy.signal import freqz, remez
import mpmath as mp

OUT = Path(__file__).resolve().parent

def reference_fir():
    c = np.zeros(65, dtype=float)
    total = 0.0
    for i in range(65):
        n = float(i - 64)
        sinc = 0.45 if n == 0 else np.sin(0.45*np.pi*n)/(np.pi*n)
        window = .42-.5*np.cos(2*np.pi*i/128)+.08*np.cos(4*np.pi*i/128)
        c[i] = sinc*window
        total += c[i] * (1.0 if i == 64 else 2.0)
    c /= total
    return np.r_[c, c[-2::-1]]

def filter_metrics(h):
    f, H = freqz(h, worN=2**20, fs=1.0)
    # Include exact band edges, which need not lie on the FFT grid.
    fe = np.array([0., .2, .25, .5])
    _, He = freqz(h, worN=2*np.pi*fe)
    pas = np.r_[np.abs(H[f <= .2]), abs(He[:2])]
    stop = np.r_[np.abs(H[f >= .25]), abs(He[2:])]
    return {
        'taps':len(h), 'symmetric_products_per_channel':(len(h)+1)//2,
        'passband_span_db':float(20*np.log10(pas.max()/pas.min())),
        'worst_stopband_db':float(20*np.log10(stop.max())),
        'mean_stopband_power_db':float(10*np.log10(np.mean(abs(H[f>=.25])**2))),
        'dc_gain':float(np.sum(h)),
        'gain_at_20khz_for_96khz_input_db':float(20*np.log10(abs(freqz(h,worN=[2*np.pi*20000/96000])[1][0]))),
        'tagged_host_latency_samples_at_2x':((len(h)-1)/2-1)/2,
    }

def gradient(d0, d1, k):
    if d1 < 0:
        potential = .4*k*d0*d0*np.sqrt(d0)
        gap = d0-d1
        return potential/gap, potential/(gap*gap)
    a, b = np.sqrt(d0), np.sqrt(d1)
    s = a+b
    if s == 0:
        return 0., 0.
    value = .4*k*(d1*d1+d1*b*a+d1*d0+b*a*d0+d0*d0)/s
    deriv = .2*k*((3*d1+6*a*b+4*d0)*b+2*d0*a)/(s*s)
    return value, deriv

def gradient_checks():
    mp.mp.dps = 85
    rng = np.random.default_rng(314159)
    pairs = [(0.,0.),(0.,1e-10),(1e-10,0.),(1e-10,-1e-12)]
    for _ in range(800):
        a = float(10**rng.uniform(-18,-1))
        b = float(10**rng.uniform(-18,-1))
        pairs.extend([(a,b),(a,-b),(a,a),(a,np.nextafter(a,np.inf)),
                      (a,a*(1+1e-10)),(a,a*(1-1e-10))])
    max_g=max_d=0.
    for d0,d1 in pairs:
        k=2e8
        got,der=gradient(d0,d1,k)
        u,v,kk=mp.mpf(d0),mp.mpf(d1),mp.mpf(k)
        potential=lambda x: mp.mpf('0.4')*kk*max(x,0)**mp.mpf('2.5')
        if u == v:
            expected=kk*max(u,0)**mp.mpf('1.5')
            derivative=mp.mpf('.75')*kk*mp.sqrt(max(u,0))
        else:
            expected=(potential(v)-potential(u))/(v-u)
            derivative=(kk*max(v,0)**mp.mpf('1.5')-expected)/(v-u)
        max_g=max(max_g,float(abs(mp.mpf(got)-expected)/max(mp.mpf('1e-290'),abs(expected))))
        max_d=max(max_d,float(abs(mp.mpf(der)-derivative)/max(mp.mpf('1e-290'),abs(derivative))))
    assert max_g < 5e-14 and max_d < 5e-14
    return {'cases':len(pairs),'reference_decimal_digits':85,
            'max_relative_gradient_error':max_g,'max_relative_derivative_error':max_d}

def main():
    filters={'original129':reference_fir()}
    for n in (81,93,101,105):
        h=remez(n,[0,.2,.25,.5],[1.,0.],weight=[1.,1.82],fs=1.,maxiter=100,grid_density=32)
        filters[f'equiripple{n}']=h/np.sum(h)
    # Wider passband candidate: protect the existing 20 kHz transition response.
    h=remez(109,[0,.2045,.25,.5],[1.,0.],weight=[1.,3.],fs=1.,maxiter=100,grid_density=32)
    filters['equiripple109']=h/np.sum(h)
    text=['#pragma once','#include <array>','namespace vessel_audit {']
    for name,h in filters.items():
        text.append(f'static const std::array<double,{len(h)}> {name} = {{{{')
        text.extend('    '+format(float(x),'.17g')+',' for x in h)
        text.append('}};')
        np.savetxt(OUT/f'{name}.csv',h,fmt='%.17g',header='coefficient',comments='')
    text.append('} // namespace vessel_audit')
    (OUT/'FirCoefficients.hpp').write_text('\n'.join(text)+'\n')
    modes=[('metal',18.122208955234012,2.0263794896615717),
           ('crystal',23.297408241459625,4.4)]
    thresholds={name:{str(fs):.4*fs/(ratio*2**(split/2400.)) for fs in (48000,96000)}
                for name,ratio,split in modes}
    compliance={str(f):{str(fs):float((np.pi*f/fs/np.tan(np.pi*f/fs))**2)
                           for fs in (48000,96000)} for f in (5000,10000,15000,18000)}
    data={'scope':'Isolated calculations; no full Vessel callback or audio corpus was run.',
          'versions':{'python':platform.python_version(),'numpy':np.__version__,'scipy':scipy.__version__},
          'filters':{name:filter_metrics(h) for name,h in filters.items()},
          'strike_gradient':gradient_checks(),
          'max_center_frequency_hz_default_imperfection_single_bowl':thresholds,
          'small_damping_static_compliance_ratio':compliance}
    (OUT/'numerical_results.json').write_text(json.dumps(data,indent=2)+'\n')
    print(json.dumps(data,indent=2))

if __name__=='__main__':main()
