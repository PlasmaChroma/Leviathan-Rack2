#!/usr/bin/env python3
from pathlib import Path
import sys,csv,json,math,struct,hashlib
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'reference'));import sp67_reference as r

def csvout(path,rows):
    with path.open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
mem=[
('0x20000000',28,'startup_initialized_data','B','Copied from 0x08049ac8'),
('0x20000020',512,'A_quadrature_filter_state_1','T','Two float states per reserved harmonic; analyzer 0x0802f716'),
('0x20000220',512,'A_quadrature_filter_state_2','T','Other quadrature; analyzer 0x0802f716'),
('0x20000420',512,'B_quadrature_filter_state_1','T','Mirrored analyzer'),
('0x20000620',512,'B_quadrature_filter_state_2','T','Mirrored analyzer'),
('0x20000880',4,'A_smoothed_partials','T','Fast CV path'),
('0x20000920',256,'B_array_descriptors','T','16 descriptors, four words each'),
('0x20000a20',256,'A_array_descriptors','T','16 descriptors, four words each'),
('0x20000b20',4,'B_array_selection','T','Read by B Array readers'),
('0x20000d40',256,'A_SAO_delta','T','64 per-sample increments'),
('0x20000e40',256,'A_SAM_adjacent_delta','T','Cleared by analyzer for active terms'),
('0x20000f40',256,'B_SAO_delta','T','64 per-sample increments'),
('0x20001040',256,'B_SAM_adjacent_delta','T','Mirrored analyzer'),
('0x20001140',4,'A_fm_index_candidate','T','Fourth-power control mapping'),
('0x2000227c',4,'dsp_timing_measurement_candidate','H','DWT cycle-count path'),
('0x200023fc',4,'B_smoothed_partials','T','Fast CV path'),
('0x20002430',4,'A_array_selection','T','Read by A Array readers'),
('0x20002434',4,'B_array_clock_offset','T','B reader position term'),
('0x20002438',4,'A_array_clock_offset','T','A reader position term'),
('0x20002a40',256,'A_SAO_coefficients','T','A reader/current coefficient bank'),
('0x20002b40',256,'A_SAM_amplitudes','T','Analyzer stores +256 relative to A SAO bank'),
('0x20002c40',256,'B_SAO_coefficients','T','B reader/current coefficient bank'),
('0x20002d40',256,'B_SAM_amplitudes','T','B analyzer result bank'),
('0x20002e50',4,'B_fm_index_candidate','T','Mirrored fourth-power mapping'),
('0x20002e88',4,'B_mode_related_field','T','Combined with B SAO flag in synthesis dispatch'),
('0x20002e8c',4,'A_mode_related_field','T','Combined with A SAO flag in synthesis dispatch'),
('0x20002e90',4,'B_SAO_related_flag','T','Mode dispatch'),
('0x20002e94',4,'A_SAO_related_flag','T','Mode dispatch'),
('0x20002edc',4,'A_clock_flag','T','GPIO callback'),
('0x20002ee0',4,'A_clock_counter','T','GPIO callback'),
('0x20002ee4',4,'B_clock_flag','T','GPIO callback'),
('0x20002ee8',4,'B_clock_counter','T','GPIO callback'),
('0x20002eec',4,'sample_counter','T','Incremented in audio loop'),
('0x20002f70',4,'B_external_base_pointer','B','Initialized to 0x60001000'),
('0x20002f74',4,'A_external_base_pointer','B','Initialized to 0x60c01000'),
('0x200144d4',0,'calibration_object_candidate','H','Extent/field semantics not fully recovered'),
('0x30000020',12,'slow_control_samples','T','Six halfword channels; total enclosing allocation unproven'),
('0x30000040',0,'fast_CV_interleaved_buffer','T','Four halfword channels/frame; allocation extent not asserted'),
('0x30000440',0,'audio_input_pair','T','32-bit stereo reads'),
('0x30000840',0,'output_pair_0','T','Physical jack mapping incomplete'),
('0x30000c40',0,'output_pair_1','T','Physical jack mapping incomplete'),
('0x30001040',0,'output_pair_2','T','Physical jack mapping incomplete'),
('0x38000000',0,'output_pair_3','T','Physical jack mapping incomplete'),
('0x60001000',0xc00000,'B_initialized_external_region','B','12 MiB memset in main'),
('0x60c01000',0xc00000,'A_initialized_external_region','B','12 MiB memset in main')]
csvout(ROOT/'analysis/memory_map_curated.csv',[dict(address=a,known_bytes=n,name=name,evidence_level=c,note=e) for a,n,name,c,e in mem])
regions=[
('0x0802e87e','0x0802e8e8','FM_index_control_mapping','T','60*u^4; mirrored sides'),
('0x0802e998','0x0802ea0c','SAM_reference_and_pole_setup','T','Exponential Slide and quadratic Focus pole'),
('0x0802ec94','0x0802ed12','DMA_buffer_pointer_setup','B/T','Eight output-lane pointers'),
('0x0802f296','0x0802f4aa','harmonic_quadrature_reference_generation','T','Sine LUT and recurrence'),
('0x0802f4ac','0x0802f716','B_quadrature_analyzer','T','DC block, cascaded filters, magnitude bit hack'),
('0x0802f716','0x0802f9bc','A_quadrature_analyzer','T','Mirrored analyzer; max 60 processed terms'),
('0x0802fa04','0x0802fc16','Chaos_A_branch_region','partial','Not fully translated'),
('0x080302b8','0x0803038c','output_clip_and_store_region','T','Lane mapping still incomplete'),
('0x0803092c','0x08030c70','standard_B_synthesis_region','T','Paired polynomial recurrence'),
('0x08030cd4','0x08030fd2','standard_A_synthesis_region','T','Seeds, recurrence, compensation'),
('0x08031038','0x08031300','Noise_A_branch_region_approximate','partial','LCG/filter path; end is descriptive, not a function boundary'),
('0x080338f4','0x080339fa','WAV_header_format_region','T','Potential PCM16 header inconsistency'),
('0x080346ee','0x08034718','external_memory_base_initialization','B','Separate A/B bases; two 12 MiB clears'),
('0x08034fd6','0x080350c6','Array_dimension_inference_region','T','4096 scalars ->8x8; otherwise framecount x1')]
csvout(ROOT/'analysis/dsp_regions.csv',[dict(start=a,end_reference=b,name=n,evidence=c,note=e) for a,b,n,c,e in regions])
F=[
('image_mapping','B','Raw Thumb application image at 0x08020000','0x08020000; 0x08036430','Exact uploaded image, not bootloader'),
('mcu_family','T','STM32H743/H753-class Cortex-M7 match','vector table and peripheral footprint','Exact package/revision unresolved'),
('cadence','T','64 stereo frames/callback; DSP calculations assume 48000 Hz','0x08032678; 0x08032680; 0x0803038c','Not a physical converter-clock measurement'),
('storage_frame','B/T','64 float32 coefficients per RAM spectrum','0x0802cd0c; 0x08033af8','Not all storage positions are active synthesis terms'),
('active_terms','T','60-term normal bank ceiling in groups of four','0x0802f7c0; 0x08030e42','Extreme signed FM paths not fully analyzed'),
('SAM_algorithm','T','Quadrature harmonic detectors with two cascaded one-poles per component','0x0802f4ac; 0x0802f716','No FFT needed in this traced path; not a whole-program absence proof'),
('magnitude','B/T','Unsigned float-bit square-root approximation','0x0802f946..0x0802f9ae','Zero maps to tiny negative bits 0x9fc00000'),
('analysis_mapping','T','Slide maps reference around20..320Hz; Focus controls detector pole','0x0802e998','Normalized internal control; analog calibration unresolved'),
('partials_recurrence','T','Partials changes polynomial carrier basis with C0=r^2','0x08030d60; 0x08030dd8; 0x08030e42','Independent odd/even phase inputs in reference probe'),
('compensation','T','Cubic compensation plus low-radius taper','0x08030f94..0x08030fd2','Float ordering affects exact rounding'),
('linear_reader','T','Adjacent-frame interpolation plus bit-domain Focus transform','0x0802cd0c; 0x0802ce5c','One-subtraction wrap, exact bypass'),
('planar_reader','T','Bilinear reader with sqrt-derived grid and unusual row edges','0x0802cfac; 0x0802d0f8','Logical bounds need upstream-state validation'),
('wave_payload','T','Observed saver emits mono PCM16, 64 values per spectrum','0x08033af8','Serialized coefficients, not source recording'),
('wave_header_quirk','T','PCM16 path appears to retain byteRate192000 and blockAlign4','0x080338f4..0x080339fa','Awaiting untouched hardware-saved WAV'),
('save_mutation','T','Conditional upward normalization writes back into RAM before16-bit conversion','0x08033af8','Tracked scalar lifecycle not fully reconstructed'),
('external_RAM','B','Two12MiB clear regions at separate A/B bases','0x080346ee..0x08034718','Not proof of installed SDRAM capacity'),
('factory_bank','B/T','4096float factory bank, 64x64 layout','0x08045a98','Extracted vendor asset; redistribution rights not determined'),
('noise','partial','LCG and filter/modulator ingredients identified','0x08031038; 0x08031128','Complete equations/state ordering unfinished'),
('chaos','partial','Coupled phase/modulation/feedback branch identified','0x0802fa04','Not a validated Chaos emulator'),
('io','partial','Input pair and eight output lanes located','0x0802ec94; 0x080302b8','Physical jack mapping/analog gains unfinished')]
(ROOT/'analysis/findings.json').write_text(json.dumps([dict(id=i,level=l,finding=f,evidence=e,boundary=b) for i,l,f,e,b in F],indent=2)+'\n')
Q=[
('P0','Confirm real saved WAV header and payload','Untouched module-saved speca/specb WAV','Validates strongest persistence quirk without needing hardware emulation'),
('P0','Translate complete phase/FM and even-output offset routing','Trace 0x0802e750 and all standard-branch joins; controlled FM captures','Required before a faithful standard oscillator wrapper'),
('P0','Map eight output lanes to physical jacks and gains','Codec/DAC configuration, schematic or output measurements','Needed for voltage/polarity equivalence'),
('P1','Capture scheduling, frame count and clock timeout','Code around0x0802d900 plus short clocked captures','File capacity alone does not give capture time'),
('P1','Calibration object and full panel/CV mapping','0x200144d4 users, ADC configuration, known-voltage tests','Normalized curves are not full knob/jack calibration'),
('P1','Independent verification of recurrence and FMA rounding','ARM instruction emulation or hardware sparse-spectrum tests','Host algebra tests do not validate complete compiled behavior'),
('P1','Complete Noise equations and limits','0x08031038 and mirrored branch; state variable trace','LCG/filter ingredients are not a full model'),
('P1','Complete Chaos state-update ordering','0x0802fa04 and corresponding joins','Chaotic systems are particularly sensitive to ordering/rounding'),
('P1','Planar logical-edge constraints','Upstream clock/dimension bounds; non-square test banks','One-subtraction indexing may read reserved padding; not proven physical OOB'),
('P2','Complete button/long-press/persistence state machine','0x0802d900 and settings file call graph','Documentation roles are not all edge cases'),
('P2','SD transport and media failure handling','FatFs-like diskio call graph','SPI5 presence does not prove SD transport'),
('P2','Exact MCU, populated memory and converter hardware','Board photographs/markings/schematic','Vector family alone is insufficient'),
('P2','Bootloader update protocol/authentication','Separate lower-flash dump or bootloader source','Missing from uploaded image'),
('P2','Cross-version differential analysis','Another genuine Spectraphon firmware image','Only SP67 available in this pass'),
('P2','Meaning of0x2010 trailing zeros','Another version or original linker map','Padding explanation remains a hypothesis')]
(ROOT/'analysis/open_questions.json').write_text(json.dumps([dict(priority=p,question=q,evidence_needed=e,reason=n) for p,q,e,n in Q],indent=2)+'\n')
metrics={'scope':'Derived from host probes and extracted tables, NOT hardware measurements','float32_fma_available':r._fmaf is not None,'sqrt_zero_bits':hex(r.bits(r.fast_sqrt_bits(0))),'sqrt_zero_value':r.fast_sqrt_bits(0),'compensation_at_r1':r.compensation(1)}
errors=[]
for i in range(20001):
    x=r.f32(2**(-100+i/100)); errors.append(r.fast_sqrt_bits(x)/math.sqrt(x)-1)
metrics['sqrt_probe']={'samples':len(errors),'domain':'float32-rounded powers2^e, e=-100..100 at.01 increments','maximum_absolute_relative_error':max(abs(e) for e in errors),'minimum_relative_error':min(errors),'maximum_relative_error':max(errors)}
controls=[]
for s in (0,.25,.5,.75,1):
    for f in (0,.25,.5,.75,1):
        inc,p=r.analysis_parameters(s,f);controls.append(dict(slide=s,focus=f,reference_hz=inc*48000,pole=p,one_pole_time_constant_seconds=-1/(48000*math.log(p))))
csvout(ROOT/'analysis/analyzer_control_grid.csv',controls)
metrics['analysis_reference_endpoints_hz']=[r.analysis_parameters(s,0)[0]*48000 for s in (0,1)]
metrics['table_count']=len(json.loads((ROOT/'tables/manifest.json').read_text()))
metrics['inventory_function_candidates']=sum(1 for _ in csv.DictReader((ROOT/'analysis/function_candidates.csv').open()))
metrics['inventory_issues']=sum(1 for _ in csv.DictReader((ROOT/'analysis/cfg_issues.csv').open()))
(ROOT/'analysis/numerical_metrics.json').write_text(json.dumps(metrics,indent=2)+'\n')
# IRQ labels use the official H743 vector ordering; exact chip still unresolved.
vp=ROOT/'analysis/vector_table.csv';rows=list(csv.DictReader(vp.open()))
core={1:'Reset',2:'NMI',3:'HardFault',4:'MemManage',5:'BusFault',6:'UsageFault',11:'SVCall',12:'DebugMonitor',14:'PendSV',15:'SysTick'}
irqs={**{11+i:f'DMA1_Stream{i}' for i in range(7)},18:'ADC',23:'EXTI9_5',47:'DMA1_Stream7',85:'SPI5',102:'DMAMUX1_OVR',129:'BDMA_Channel0'}
for row in rows:
    idx=int(row['index']);irq=int(row['irq']);row['assigned_irq_name']=core.get(idx,irqs.get(irq,'')) if row['kind']=='handler' else ''
csvout(vp,rows)
print(json.dumps(metrics,indent=2))
