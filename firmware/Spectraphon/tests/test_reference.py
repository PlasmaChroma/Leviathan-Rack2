#!/usr/bin/env python3
"""Host-side consistency tests, NOT validation against a physical Spectraphon."""
from pathlib import Path
import sys, math, json, unittest, struct, hashlib, random, time
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'reference'))
import sp67_reference as r

def constant_frames(n):return [[i/(max(1,n-1))]*64 for i in range(n)]
class ReferenceTests(unittest.TestCase):
    def test_firmware_identity(self):
        b=(ROOT/'firmware/sp67.bin').read_bytes()
        self.assertEqual(len(b),178932);self.assertEqual(hashlib.sha256(b).hexdigest(),'b50a87b9bd4f6ad90007918a16e8a4d033e2236c863faec55f5585abe34927d9')
    def test_elf_payload_identity(self):
        e=(ROOT/'firmware/sp67_analysis_mapped.elf').read_bytes();b=(ROOT/'firmware/sp67.bin').read_bytes()
        self.assertEqual(e[:4],b'\x7fELF');self.assertEqual(e[0x1000:0x1000+len(b)],b)
        self.assertEqual(struct.unpack_from('<I',e,24)[0],0x08036431)
    def test_all_table_hashes(self):
        for t in json.loads((ROOT/'tables/manifest.json').read_text()):self.assertEqual(len(r.table(t['name'])),t['count'])
    def test_sine_tables(self):
        for name in ('sine_8192','waveform_sine_1024'):
            vals=r.table(name);self.assertLess(max(abs(v-math.sin(2*math.pi*i/len(vals))) for i,v in enumerate(vals)),4e-8)
    def test_exp2_table(self):
        self.assertLess(max(abs(v-2**(i/2048)) for i,v in enumerate(r.table('exp2_2048'))),1.3e-7)
    def test_factory_shape(self):
        vals=r.table('factory_spectra_64x64');self.assertEqual(len(vals),4096);self.assertTrue(all(math.isfinite(v) for v in vals));self.assertGreater(max(vals),1)
    def test_fast_sqrt_normal_error(self):
        error=max(abs(r.fast_sqrt_bits(2**(-100+i/10))/math.sqrt(r.f32(2**(-100+i/10)))-1) for i in range(2001))
        self.assertLess(error,.07)
    def test_fast_sqrt_zero_not_repaired(self):self.assertEqual(r.bits(r.fast_sqrt_bits(0)),0x9fc00000)
    def test_focus_exact_bypass(self):self.assertEqual(r.focus_transform(.123,r.f32(1/3)),r.f32(.123))
    def test_focus_nonpositive_bypass(self):
        for f in (0,.5,1):self.assertEqual(r.focus_transform(-.5,f),-.5);self.assertEqual(r.focus_transform(0,f),0)
    def test_analysis_pitch_endpoints(self):
        a,_=r.analysis_parameters(0,0);b,_=r.analysis_parameters(1,0)
        self.assertAlmostEqual(a*48000,20,places=5);self.assertGreater(b*48000,319);self.assertLess(b*48000,320)
    def test_focus_narrows_analysis(self):
        _,p0=r.analysis_parameters(.5,0);_,p1=r.analysis_parameters(.5,1);self.assertGreater(p1,p0)
    def test_group_limits(self):
        self.assertEqual(r.active_terms(0),60);self.assertEqual(r.active_terms(.01),44);self.assertEqual(r.active_terms(.2),0)
        self.assertTrue(all(r.active_terms(i/10000)%4==0 for i in range(2000)))
    def test_synth_unity_radius_equivalence(self):
        rng=random.Random(67);a=[rng.random()/64 for _ in range(64)]
        for phase in (.007,.1,.211,.499):
            odd,even=r.polynomial_synthesis(a,1,phase,phase)
            eo=sum((-1)**m*a[2*m]*math.cos((2*m+1)*2*math.pi*phase) for m in range(30))*r.compensation(1)
            ee=sum((-1)**(m+1)*a[2*m+1]*math.sin((2*m+2)*2*math.pi*phase) for m in range(30))*r.compensation(1)
            self.assertAlmostEqual(odd,eo,places=11);self.assertAlmostEqual(even,ee,places=11)
    def test_synth_zero_partials_mute(self):self.assertEqual(r.polynomial_synthesis([1]*64,0,.1,.2),(0,0))
    def test_synth_last_four_not_used(self):
        a=[0]*60+[1]*4;self.assertEqual(r.polynomial_synthesis(a,1,.1,.2),(0,0))
    def test_soft_clip_endpoints(self):
        self.assertAlmostEqual(r.soft_clip(100),1,places=7);self.assertAlmostEqual(r.soft_clip(-100),-1,places=7)
    def test_linear_interpolation(self):
        self.assertAlmostEqual(r.linear_read(constant_frames(4),.5,.4,apply_focus=False)[0],.5,places=6)
    def test_linear_wrap(self):
        self.assertEqual(r.linear_read(constant_frames(4),1,0,1,False),[0]*64)
    def test_planar_grid_center(self):
        self.assertAlmostEqual(r.planar_read(constant_frames(4),.5,.5)[0],.5,places=6)
    def test_planar_last_corner(self):
        self.assertAlmostEqual(r.planar_read(constant_frames(64),1,1)[0],1,places=6)
    def test_invalid_array_offsets_rejected(self):
        with self.assertRaises(ValueError):r.linear_read(constant_frames(4),1,0,100)
        with self.assertRaises(ValueError):r.planar_read(constant_frames(4),1,0,100)
    def test_ramp(self):self.assertEqual(r.ramp_delta([0]*64,[1]*64),[1/64]*64)
    def test_wav_pcm16_roundtrip(self):
        frames=constant_frames(4);info,decoded=r.decode_array_wav(r.encode_pcm16(frames))
        self.assertEqual(info.frame_count,4);self.assertEqual(info.inferred_dimensions,(4,1));self.assertFalse(info.warnings)
        self.assertLess(max(abs(a-b) for ra,rb in zip(frames,decoded) for a,b in zip(ra,rb)),2/32768)
    def test_wav_64_frame_dimensions(self):
        info,_=r.decode_array_wav(r.encode_pcm16(constant_frames(64)));self.assertEqual(info.inferred_dimensions,(8,8))
    def test_wav_header_quirk_reported(self):
        info,_=r.decode_array_wav(r.encode_pcm16(constant_frames(4),firmware_header_quirk=True));self.assertEqual(len(info.warnings),2)
    def test_writer_does_not_mutate(self):
        frames=[[.2]*64];before=[row[:] for row in frames];r.encode_pcm16(frames,peak=.2);self.assertEqual(frames,before)
    def test_writer_refuses_overflow(self):
        with self.assertRaises(ValueError):r.encode_pcm16([[2]*64])
    def test_wav_pcm_formats(self):
        for tag,bps in ((1,8),(1,16),(1,24),(1,32),(3,32)):
            width=bps//8;payload=(bytes([128]) if bps==8 else bytes(width))*64
            fmt=struct.pack('<HHIIHH',tag,1,48000,48000*width,width,bps)
            chunks=b'fmt '+struct.pack('<I',16)+fmt+b'data'+struct.pack('<I',len(payload))+payload
            info,frames=r.decode_array_wav(b'RIFF'+struct.pack('<I',len(chunks)+4)+b'WAVE'+chunks)
            self.assertEqual(frames,[[0]*64]);self.assertEqual(info.frame_count,1)
    def test_malformed_wav_rejected(self):
        valid=r.encode_pcm16(constant_frames(4))
        for cut in (0,4,11,12,20,35,len(valid)-1):
            with self.assertRaises(ValueError):r.decode_array_wav(valid[:cut])
        bad=bytearray(valid);struct.pack_into('<H',bad,22,2)
        with self.assertRaises(ValueError):r.decode_array_wav(bytes(bad))
    def test_quadrature_detector_bounded_tone(self):
        d=r.QuadratureDetector()
        for i in range(10000):
            phase=2*math.pi*i/100;y=d.step(math.sin(phase),math.cos(phase),math.sin(phase),.995)
        self.assertTrue(.4<y<.6)

if __name__=='__main__':
    start=time.time();suite=unittest.defaultTestLoader.loadTestsFromTestCase(ReferenceTests)
    result=unittest.TextTestRunner(verbosity=2).run(suite)
    summary={'tests_run':result.testsRun,'failures':len(result.failures),'errors':len(result.errors),'seconds':time.time()-start,'passed':result.wasSuccessful(),'scope':'Host-side helper tests only. No hardware recording, ARM execution, or audio null comparison.'}
    (ROOT/'analysis/test_results.json').write_text(json.dumps(summary,indent=2)+'\n')
    raise SystemExit(not result.wasSuccessful())
