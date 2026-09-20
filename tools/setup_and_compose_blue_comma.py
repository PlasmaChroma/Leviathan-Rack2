import sys
import json
import urllib.request
from pathlib import Path

# Add repo root to path
repo_root = Path("c:/msys64/home/Plasm/Leviathan")
sys.path.insert(0, str(repo_root))

from tools.sibyl_composer import SibylClient, SibylScore, PatternBuilder, EdoTuning

BASE_URL = "http://127.0.0.1:34570"

def post_json(endpoint, payload):
    data = json.dumps(payload).encode("utf-8")
    req = urllib.request.Request(f"{BASE_URL}/{endpoint}", data=data, headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req) as resp:
        return json.loads(resp.read().decode("utf-8"))

def get_json(endpoint):
    req = urllib.request.Request(f"{BASE_URL}/{endpoint}", headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req) as resp:
        return json.loads(resp.read().decode("utf-8"))

print("1. Fetching module inventory...")
modules = get_json("modules/summary")
id_map = {}
for m in modules:
    key = (m["plugin"], m["model"])
    id_map.setdefault(key, []).append(m["id"])

sibyl_id = id_map[("Leviathan", "Sibyl")][0]
undertow_id = id_map[("Leviathan", "Undertow")][0]
splits = id_map[("Fundamental", "Split")]
split_pitch_id = splits[0]
split_gate_id = splits[1]

merges = id_map[("Fundamental", "Merge")]
merge_pitch_id = merges[0]
merge_gate_id = merges[1]

vcos = id_map[("Fundamental", "VCO")]
lead_vco_id = vcos[0]
chord_vco_id = vcos[1]

vcfs = id_map[("Fundamental", "VCF")]
lead_vcf_id = vcfs[0]
chord_vcf_id = vcfs[1]

adsrs = id_map[("Fundamental", "ADSR")]
lead_adsr_id = adsrs[0]
bass_adsr_id = adsrs[1]
chord_adsr_id = adsrs[2]

vcas = id_map[("Fundamental", "VCA-1")]
lead_vca_id = vcas[0]
bass_vca_id = vcas[1]
chord_vca_id = vcas[2]

mixmaster_id = id_map[("MindMeldModular", "MixMaster")][0]

print(f"Resolved modules: Sibyl={sibyl_id}, Undertow={undertow_id}, MixMaster={mixmaster_id}")

print("2. Connecting all audio & CV cables...")
cables_to_connect = [
    # Sibyl -> Split modules
    {"outputModuleId": sibyl_id, "outputPortId": 0, "inputModuleId": split_pitch_id, "inputPortId": 0, "color": "cyan"},
    {"outputModuleId": sibyl_id, "outputPortId": 1, "inputModuleId": split_gate_id, "inputPortId": 0, "color": "yellow"},

    # Lead Voice (Channel 0 / output 0 from Split)
    {"outputModuleId": split_pitch_id, "outputPortId": 0, "inputModuleId": lead_vco_id, "inputPortId": 0, "color": "cyan"},
    {"outputModuleId": split_gate_id, "outputPortId": 0, "inputModuleId": lead_adsr_id, "inputPortId": 4, "color": "yellow"},
    {"outputModuleId": lead_vco_id, "outputPortId": 2, "inputModuleId": lead_vcf_id, "inputPortId": 3, "color": "blue"}, # Saw into VCF
    {"outputModuleId": lead_adsr_id, "outputPortId": 0, "inputModuleId": lead_vcf_id, "inputPortId": 0, "color": "orange"}, # Env to Cutoff CV
    {"outputModuleId": lead_adsr_id, "outputPortId": 0, "inputModuleId": lead_vca_id, "inputPortId": 0, "color": "orange"}, # Env to VCA CV
    {"outputModuleId": lead_vcf_id, "outputPortId": 0, "inputModuleId": lead_vca_id, "inputPortId": 1, "color": "blue"}, # VCF Lowpass to VCA
    {"outputModuleId": lead_vca_id, "outputPortId": 0, "inputModuleId": mixmaster_id, "inputPortId": 0, "color": "white"}, # Lead to MixMaster CH1

    # Bass Voice (Channel 1 / output 1 from Split)
    {"outputModuleId": split_pitch_id, "outputPortId": 1, "inputModuleId": undertow_id, "inputPortId": 0, "color": "cyan"},
    {"outputModuleId": split_gate_id, "outputPortId": 1, "inputModuleId": bass_adsr_id, "inputPortId": 4, "color": "yellow"},
    {"outputModuleId": undertow_id, "outputPortId": 1, "inputModuleId": bass_vca_id, "inputPortId": 1, "color": "purple"}, # Morph output
    {"outputModuleId": bass_adsr_id, "outputPortId": 0, "inputModuleId": bass_vca_id, "inputPortId": 0, "color": "orange"},
    {"outputModuleId": bass_vca_id, "outputPortId": 0, "inputModuleId": mixmaster_id, "inputPortId": 2, "color": "white"}, # Bass to MixMaster CH2

    # Chord Pitch Merge (Split outputs 2, 3, 4, 5 -> Merge inputs 0, 1, 2, 3)
    {"outputModuleId": split_pitch_id, "outputPortId": 2, "inputModuleId": merge_pitch_id, "inputPortId": 0, "color": "cyan"},
    {"outputModuleId": split_pitch_id, "outputPortId": 3, "inputModuleId": merge_pitch_id, "inputPortId": 1, "color": "cyan"},
    {"outputModuleId": split_pitch_id, "outputPortId": 4, "inputModuleId": merge_pitch_id, "inputPortId": 2, "color": "cyan"},
    {"outputModuleId": split_pitch_id, "outputPortId": 5, "inputModuleId": merge_pitch_id, "inputPortId": 3, "color": "cyan"},

    # Chord Gate Merge (Split outputs 2, 3, 4, 5 -> Merge inputs 0, 1, 2, 3)
    {"outputModuleId": split_gate_id, "outputPortId": 2, "inputModuleId": merge_gate_id, "inputPortId": 0, "color": "yellow"},
    {"outputModuleId": split_gate_id, "outputPortId": 3, "inputModuleId": merge_gate_id, "inputPortId": 1, "color": "yellow"},
    {"outputModuleId": split_gate_id, "outputPortId": 4, "inputModuleId": merge_gate_id, "inputPortId": 2, "color": "yellow"},
    {"outputModuleId": split_gate_id, "outputPortId": 5, "inputModuleId": merge_gate_id, "inputPortId": 3, "color": "yellow"},

    # Chords Poly Voice
    {"outputModuleId": merge_pitch_id, "outputPortId": 0, "inputModuleId": chord_vco_id, "inputPortId": 0, "color": "cyan"},
    {"outputModuleId": merge_gate_id, "outputPortId": 0, "inputModuleId": chord_adsr_id, "inputPortId": 4, "color": "yellow"},
    {"outputModuleId": chord_vco_id, "outputPortId": 1, "inputModuleId": chord_vcf_id, "inputPortId": 3, "color": "green"}, # Triangle into VCF
    {"outputModuleId": chord_adsr_id, "outputPortId": 0, "inputModuleId": chord_vcf_id, "inputPortId": 0, "color": "orange"},
    {"outputModuleId": chord_adsr_id, "outputPortId": 0, "inputModuleId": chord_vca_id, "inputPortId": 0, "color": "orange"},
    {"outputModuleId": chord_vcf_id, "outputPortId": 0, "inputModuleId": chord_vca_id, "inputPortId": 1, "color": "green"},
    {"outputModuleId": chord_vca_id, "outputPortId": 0, "inputModuleId": mixmaster_id, "inputPortId": 4, "color": "white"}, # Chords to MixMaster CH3
]

connected_count = 0
for c in cables_to_connect:
    try:
        res = post_json("cables", c)
        if res.get("ok"):
            connected_count += 1
    except Exception as e:
        pass

print(f"Cables established ({connected_count} newly connected or confirmed).")

print("3. Shaping synth parameters...")
def set_param(mid, pid, val):
    try:
        post_json(f"modules/{mid}/parameters", {"parameterId": pid, "value": val})
    except Exception:
        pass

# Lead Voice parameters: warm expressive jazz lead
set_param(lead_vco_id, 2, 0.0)      # VCO Frequency center
set_param(lead_vcf_id, 0, 0.44)     # VCF Cutoff ~1 kHz warm
set_param(lead_vcf_id, 2, 0.12)     # VCF Resonance
set_param(lead_vcf_id, 3, 0.40)     # Cutoff CV depth
set_param(lead_adsr_id, 0, 0.08)    # Attack (80ms soft horn/vibes onset)
set_param(lead_adsr_id, 1, 0.35)    # Decay
set_param(lead_adsr_id, 2, 0.72)    # Sustain
set_param(lead_adsr_id, 3, 0.28)    # Release
set_param(lead_vca_id, 0, 0.85)     # VCA Level

# Bass Voice parameters (Undertow + ADSR): woody upright pluck
set_param(undertow_id, 0, 0.472562) # Center frequency
set_param(undertow_id, 3, 0.32)     # Morph: adds woody bite to sine
set_param(undertow_id, 5, 0.45)     # Morph edge hardness
set_param(bass_adsr_id, 0, 0.015)   # Fast pluck attack (15ms)
set_param(bass_adsr_id, 1, 0.45)    # Woody acoustic decay
set_param(bass_adsr_id, 2, 0.35)    # Sustain
set_param(bass_adsr_id, 3, 0.20)    # Release
set_param(bass_vca_id, 0, 0.95)     # VCA Level

# Chord Voice parameters: velvety warm electric piano / Rhodes
set_param(chord_vco_id, 2, 0.0)     # VCO Frequency center
set_param(chord_vcf_id, 0, 0.46)    # VCF Cutoff
set_param(chord_vcf_id, 2, 0.08)    # VCF Resonance
set_param(chord_vcf_id, 3, 0.22)    # Cutoff CV depth
set_param(chord_adsr_id, 0, 0.035)  # Attack
set_param(chord_adsr_id, 1, 0.40)   # Decay
set_param(chord_adsr_id, 2, 0.65)   # Sustain
set_param(chord_adsr_id, 3, 0.30)   # Release
set_param(chord_vca_id, 0, 0.78)    # VCA Level

# MixMaster levels and Aux sends:
set_param(mixmaster_id, 0, 0.88)    # CH1 Lead Level
set_param(mixmaster_id, 1, 0.95)    # CH2 Bass Level
set_param(mixmaster_id, 2, 0.80)    # CH3 Chords Level

print("4. Composing 'Blue Comma' in 53-EDO...")

client = SibylClient(base_url=BASE_URL, module_id=sibyl_id)
status = client.get_status()
current_rev = status.get("revision", 0)

score = SibylScore(title="Blue Comma (53-EDO Jazz Blues)", bpm=120.0)

# Set swing in meta: 0.22 for authentic medium swing eighths
score.add_operation({"op": "set_meta", "path": "swing", "value": 0.22})

# Microtonal Pitch System: 53-EDO
score.add_operation({"op": "upsert_tuning", "id": "t53", "tuning": {"kind": "equal", "divisions": 53, "period": {"ratio": "2/1"}}})
score.add_operation({"op": "upsert_pitch_context", "id": "c53", "context": {"tuning": "t53", "anchor": {"pitchV": 0.0}}})
score.add_operation({"op": "set_default_pitch_context", "context_id": "c53"})

# 6 Tracks
score.add_operation({"op": "upsert_track", "id": "lead", "track": {"channel": 0, "defaultGate": 0.85, "defaultVelocity": 0.85}})
score.add_operation({"op": "upsert_track", "id": "bass", "track": {"channel": 1, "defaultGate": 0.75, "defaultVelocity": 0.88}})
score.add_operation({"op": "upsert_track", "id": "chord1", "track": {"channel": 2, "defaultGate": 0.80, "defaultVelocity": 0.75}})
score.add_operation({"op": "upsert_track", "id": "chord2", "track": {"channel": 3, "defaultGate": 0.80, "defaultVelocity": 0.72}})
score.add_operation({"op": "upsert_track", "id": "chord3", "track": {"channel": 4, "defaultGate": 0.80, "defaultVelocity": 0.70}})
score.add_operation({"op": "upsert_track", "id": "chord4", "track": {"channel": 5, "defaultGate": 0.80, "defaultVelocity": 0.68}})

def bb_to_step(bar, beat):
    return int((bar - 1) * 16 + round((beat - 1.0) * 4))

# ══════════════════════════════════════════════════════════════════════════════
# SECTION 1: INTRO (4 BARS = 64 STEPS)
# ══════════════════════════════════════════════════════════════════════════════
intro_lead = PatternBuilder(length=64, resolution="1/16", pitch_context="c53")
# I1: 1: 71 (1 beat = 4 steps gate), 2: 70 (1), 3: 62 (1), 4: 53 (1)
intro_lead.add_note(bb_to_step(1, 1.0), tuned_step=71, gate=3.8, velocity=0.82) # Bright E
intro_lead.add_note(bb_to_step(1, 2.0), tuned_step=70, gate=3.8, velocity=0.80) # Sweet E (comma sigh!)
intro_lead.add_note(bb_to_step(1, 3.0), tuned_step=62, gate=3.8, velocity=0.78) # D
intro_lead.add_note(bb_to_step(1, 4.0), tuned_step=53, gate=3.8, velocity=0.76) # C
# I2: 1: 53 (2), 3: 62 (1), 4: 70 (1)
intro_lead.add_note(bb_to_step(2, 1.0), tuned_step=53, gate=7.8, velocity=0.78)
intro_lead.add_note(bb_to_step(2, 3.0), tuned_step=62, gate=3.8, velocity=0.80)
intro_lead.add_note(bb_to_step(2, 4.0), tuned_step=70, gate=3.8, velocity=0.82)
# I3: 1: 84 (2), 3: 75 (0.5), 3.5: 70 (0.5), 4: 62 (1)
intro_lead.add_note(bb_to_step(3, 1.0), tuned_step=84, gate=7.8, velocity=0.85) # High G
intro_lead.add_note(bb_to_step(3, 3.0), tuned_step=75, gate=1.8, velocity=0.80) # F
intro_lead.add_note(bb_to_step(3, 3.5), tuned_step=70, gate=1.8, velocity=0.82) # Sweet E
intro_lead.add_note(bb_to_step(3, 4.0), tuned_step=62, gate=3.8, velocity=0.78) # D
# I4: 1: 70 (1), 2: 62 (0.5), 2.5: 53 (0.5), rest 2
intro_lead.add_note(bb_to_step(4, 1.0), tuned_step=70, gate=3.8, velocity=0.82)
intro_lead.add_note(bb_to_step(4, 2.0), tuned_step=62, gate=1.8, velocity=0.78)
intro_lead.add_note(bb_to_step(4, 2.5), tuned_step=53, gate=1.8, velocity=0.75)

intro_bass = PatternBuilder(length=64, resolution="1/16", pitch_context="c53")
# C pedal (step -53 = C2) on beat 1 of each intro bar
for b in range(1, 5):
    intro_bass.add_note(bb_to_step(b, 1.0), tuned_step=-53, gate=14.0, velocity=0.85)

# Chords Intro: subtle C7 pad [0, 17, 31, 44]
intro_c1 = PatternBuilder(length=64, resolution="1/16", pitch_context="c53")
intro_c2 = PatternBuilder(length=64, resolution="1/16", pitch_context="c53")
intro_c3 = PatternBuilder(length=64, resolution="1/16", pitch_context="c53")
intro_c4 = PatternBuilder(length=64, resolution="1/16", pitch_context="c53")
for b in range(1, 5):
    intro_c1.add_note(bb_to_step(b, 1.0), tuned_step=44, gate=15.0, velocity=0.65) # Bb
    intro_c2.add_note(bb_to_step(b, 1.0), tuned_step=31, gate=15.0, velocity=0.68) # G
    intro_c3.add_note(bb_to_step(b, 1.0), tuned_step=17, gate=15.0, velocity=0.70) # Just E
    intro_c4.add_note(bb_to_step(b, 1.0), tuned_step=0,  gate=15.0, velocity=0.72) # C

score.add_pattern("p_intro_lead", intro_lead)
score.add_pattern("p_intro_bass", intro_bass)
score.add_pattern("p_intro_c1", intro_c1)
score.add_pattern("p_intro_c2", intro_c2)
score.add_pattern("p_intro_c3", intro_c3)
score.add_pattern("p_intro_c4", intro_c4)

# ══════════════════════════════════════════════════════════════════════════════
# SECTION 2: HEAD (12 BARS = 192 STEPS)
# ══════════════════════════════════════════════════════════════════════════════
head_lead = PatternBuilder(length=192, resolution="1/16", pitch_context="c53")

# Bar 1 (C7): 1: 31 (1), 2: 17 (1), 3: 9 (0.5), 3.5: 13 (0.5), 4: 17 (1)
head_lead.add_note(bb_to_step(1, 1.0), tuned_step=31+53, gate=3.8, velocity=0.85)
head_lead.add_note(bb_to_step(1, 2.0), tuned_step=17+53, gate=3.8, velocity=0.82)
head_lead.add_note(bb_to_step(1, 3.0), tuned_step=9+53,  gate=1.8, velocity=0.80)
head_lead.add_note(bb_to_step(1, 3.5), tuned_step=13+53, gate=1.8, velocity=0.82)
head_lead.add_note(bb_to_step(1, 4.0), tuned_step=17+53, gate=3.8, velocity=0.85)

# Bar 2 (F7): 1: 39 (1), 2: 31 (0.5), 2.5: 22 (0.5), 3: 13 (1), 4: rest
head_lead.add_note(bb_to_step(2, 1.0), tuned_step=39+53, gate=3.8, velocity=0.86)
head_lead.add_note(bb_to_step(2, 2.0), tuned_step=31+53, gate=1.8, velocity=0.82)
head_lead.add_note(bb_to_step(2, 2.5), tuned_step=22+53, gate=1.8, velocity=0.80)
head_lead.add_note(bb_to_step(2, 3.0), tuned_step=13+53, gate=3.8, velocity=0.78)

# Bar 3 (C7): 1: 17 (2), 3: 9 (0.5), 3.5: 17 (0.5), 4: 31 (1)
head_lead.add_note(bb_to_step(3, 1.0), tuned_step=17+53, gate=7.8, velocity=0.84)
head_lead.add_note(bb_to_step(3, 3.0), tuned_step=9+53,  gate=1.8, velocity=0.80)
head_lead.add_note(bb_to_step(3, 3.5), tuned_step=17+53, gate=1.8, velocity=0.82)
head_lead.add_note(bb_to_step(3, 4.0), tuned_step=31+53, gate=3.8, velocity=0.85)

# Bar 4 (C7): 1: 31 (1), 2: 71 (0.5), 2.5: 70 (0.5), 3: 62 (1), 4: 53 (1)  <- Bright E (71) sighs to Sweet E (70)!
head_lead.add_note(bb_to_step(4, 1.0), tuned_step=31+53, gate=3.8, velocity=0.85)
head_lead.add_note(bb_to_step(4, 2.0), tuned_step=71,    gate=1.8, velocity=0.88) # Bright E (Pythagorean)
head_lead.add_note(bb_to_step(4, 2.5), tuned_step=70,    gate=1.8, velocity=0.86) # Sweet E (5:4 Just) Comma sigh!
head_lead.add_note(bb_to_step(4, 3.0), tuned_step=62,    gate=3.8, velocity=0.82) # D
head_lead.add_note(bb_to_step(4, 4.0), tuned_step=53,    gate=3.8, velocity=0.80) # C

# Bar 5 (F7): 1: 53 (2), 3: 92 (1), 4: 84 (1)
head_lead.add_note(bb_to_step(5, 1.0), tuned_step=53,    gate=7.8, velocity=0.85)
head_lead.add_note(bb_to_step(5, 3.0), tuned_step=92,    gate=3.8, velocity=0.88)
head_lead.add_note(bb_to_step(5, 4.0), tuned_step=84,    gate=3.8, velocity=0.86)

# Bar 6 (F#o7): 1: 80 (1), 2: 93 (1), 3: 106 (2)  <- Rising diminished arpeggio
head_lead.add_note(bb_to_step(6, 1.0), tuned_step=80,    gate=3.8, velocity=0.88)
head_lead.add_note(bb_to_step(6, 2.0), tuned_step=93,    gate=3.8, velocity=0.90)
head_lead.add_note(bb_to_step(6, 3.0), tuned_step=106,   gate=7.8, velocity=0.92)

# Bar 7 (C7): 1: 84 (1), 2: 70 (1), 3: 62 (1), 4: 53 (1)  <- Falling scale from the peak
head_lead.add_note(bb_to_step(7, 1.0), tuned_step=84,    gate=3.8, velocity=0.86)
head_lead.add_note(bb_to_step(7, 2.0), tuned_step=70,    gate=3.8, velocity=0.84)
head_lead.add_note(bb_to_step(7, 3.0), tuned_step=62,    gate=3.8, velocity=0.82)
head_lead.add_note(bb_to_step(7, 4.0), tuned_step=53,    gate=3.8, velocity=0.80)

# Bar 8 (A7): 1: 58 (1), 2: 71 (0.5), 2.5: 62 (0.5), 3: 58 (1), 4: rest <- C# = 58 bright 3rd
head_lead.add_note(bb_to_step(8, 1.0), tuned_step=58,    gate=3.8, velocity=0.88)
head_lead.add_note(bb_to_step(8, 2.0), tuned_step=71,    gate=1.8, velocity=0.86)
head_lead.add_note(bb_to_step(8, 2.5), tuned_step=62,    gate=1.8, velocity=0.82)
head_lead.add_note(bb_to_step(8, 3.0), tuned_step=58,    gate=3.8, velocity=0.84)

# Bar 9 (Dm7): 1: 62 (2), 3: 76 (2)  <- The comma F held exposed: 76 = 23 + 53!
head_lead.add_note(bb_to_step(9, 1.0), tuned_step=62,    gate=7.8, velocity=0.85)
head_lead.add_note(bb_to_step(9, 3.0), tuned_step=76,    gate=7.8, velocity=0.92) # THE COMMA F (step 23 + 53)

# Bar 10 (G7): 1: 75 (1), 2: 70 (0.5), 2.5: 62 (0.5), 3: 48 (2) <- 76 -> 75 comma exhales!
head_lead.add_note(bb_to_step(10, 1.0), tuned_step=75,   gate=3.8, velocity=0.88) # Comma resolution across barline!
head_lead.add_note(bb_to_step(10, 2.0), tuned_step=70,   gate=1.8, velocity=0.84)
head_lead.add_note(bb_to_step(10, 2.5), tuned_step=62,   gate=1.8, velocity=0.80)
head_lead.add_note(bb_to_step(10, 3.0), tuned_step=48,   gate=7.8, velocity=0.78)

# Bar 11 (Cmaj7): 1: 70 (1), 2: 62 (0.5), 2.5: 70 (0.5), 3: 84 (1), 4: 70 (1)
head_lead.add_note(bb_to_step(11, 1.0), tuned_step=70,   gate=3.8, velocity=0.85)
head_lead.add_note(bb_to_step(11, 2.0), tuned_step=62,   gate=1.8, velocity=0.82)
head_lead.add_note(bb_to_step(11, 2.5), tuned_step=70,   gate=1.8, velocity=0.84)
head_lead.add_note(bb_to_step(11, 3.0), tuned_step=84,   gate=3.8, velocity=0.88)
head_lead.add_note(bb_to_step(11, 4.0), tuned_step=70,   gate=3.8, velocity=0.85)

# Bar 12 (G7 turnaround): 1: 62 (1), 2: 75 (0.5), 2.5: 70 (0.5), 3: 62 (1), 4: rest
head_lead.add_note(bb_to_step(12, 1.0), tuned_step=62,   gate=3.8, velocity=0.84)
head_lead.add_note(bb_to_step(12, 2.0), tuned_step=75,   gate=1.8, velocity=0.86)
head_lead.add_note(bb_to_step(12, 2.5), tuned_step=70,   gate=1.8, velocity=0.82)
head_lead.add_note(bb_to_step(12, 3.0), tuned_step=62,   gate=3.8, velocity=0.80)

# Walking Bass Line for 12 bars (quarter notes, transposed to bass register: -53 steps = C2)
head_bass = PatternBuilder(length=192, resolution="1/16", pitch_context="c53")
bass_steps_12 = [
    [0, 0, 9, 13],       # Bar 1 (C7)
    [22, 22, 31, 35],    # Bar 2 (F7)
    [0, 0, 9, 17],       # Bar 3 (C7)
    [0, 31, 22, 13],     # Bar 4 (C7)
    [22, 22, 27, 31],    # Bar 5 (F7)
    [27, 30, 32, 35],    # Bar 6 (F#o7 - walking the diminished!)
    [0, 0, 9, 13],       # Bar 7 (C7)
    [40, 44, 48, 53],    # Bar 8 (A7)
    [9, 13, 18, 23],     # Bar 9 (Dm7 - climbs into the comma F: 23!)
    [31, 35, 40, 44],    # Bar 10 (G7)
    [0, 9, 17, 22],      # Bar 11 (Cmaj7)
    [31, 26, 22, 18]     # Bar 12 (G7 - walking down into top of form!)
]

for bar_idx, bar_notes in enumerate(bass_steps_12):
    bar_num = bar_idx + 1
    for beat_idx, pitch in enumerate(bar_notes):
        beat_num = beat_idx + 1.0
        p_bass = pitch - 53
        head_bass.add_note(bb_to_step(bar_num, beat_num), tuned_step=p_bass, gate=3.6, velocity=0.88 if beat_idx % 2 == 0 else 0.82)

head_c1 = PatternBuilder(length=192, resolution="1/16", pitch_context="c53")
head_c2 = PatternBuilder(length=192, resolution="1/16", pitch_context="c53")
head_c3 = PatternBuilder(length=192, resolution="1/16", pitch_context="c53")
head_c4 = PatternBuilder(length=192, resolution="1/16", pitch_context="c53")

chord_voicings = [
    [0, 17, 31, 44],    # Bar 1: C7
    [22, 39, 53, 66],   # Bar 2: F7
    [0, 17, 31, 44],    # Bar 3: C7
    [0, 17, 31, 44],    # Bar 4: C7
    [22, 39, 53, 66],   # Bar 5: F7
    [27, 40, 53, 66],   # Bar 6: F#o7
    [0, 17, 31, 44],    # Bar 7: C7
    [40, 58, 71, 84],   # Bar 8: A7
    [9, 23, 40, 53],    # Bar 9: Dm7 (comma F at 23!)
    [31, 48, 62, 75],   # Bar 10: G7
    [0, 17, 31, 48],    # Bar 11: Cmaj7
    [31, 48, 62, 75]    # Bar 12: G7
]

for bar_idx, voicing in enumerate(chord_voicings):
    bar_num = bar_idx + 1
    v4, v3, v2, v1 = voicing
    for beat, dur in [(1.0, 9.0), (3.5, 5.0)]:
        step_pos = bb_to_step(bar_num, beat)
        head_c1.add_note(step_pos, tuned_step=v1, gate=dur*0.8, velocity=0.74)
        head_c2.add_note(step_pos, tuned_step=v2, gate=dur*0.8, velocity=0.72)
        head_c3.add_note(step_pos, tuned_step=v3, gate=dur*0.8, velocity=0.70)
        head_c4.add_note(step_pos, tuned_step=v4, gate=dur*0.8, velocity=0.68)

score.add_pattern("p_head_lead", head_lead)
score.add_pattern("p_head_bass", head_bass)
score.add_pattern("p_head_c1", head_c1)
score.add_pattern("p_head_c2", head_c2)
score.add_pattern("p_head_c3", head_c3)
score.add_pattern("p_head_c4", head_c4)

# ══════════════════════════════════════════════════════════════════════════════
# SECTION 3: SOLO CHORUS 1 (12 BARS = 192 STEPS)
# ══════════════════════════════════════════════════════════════════════════════
solo1_lead = PatternBuilder(length=192, resolution="1/16", pitch_context="c53")

solo1_lead.add_note(bb_to_step(1, 1.0), tuned_step=53,    gate=1.8, velocity=0.82)
solo1_lead.add_note(bb_to_step(1, 1.5), tuned_step=53+9, gate=1.8, velocity=0.80)
solo1_lead.add_note(bb_to_step(1, 2.0), tuned_step=53+15, gate=1.8, velocity=0.86) # Blue note!
solo1_lead.add_note(bb_to_step(1, 2.5), tuned_step=53+16, gate=1.8, velocity=0.88) # Micro-bend to 16
solo1_lead.add_note(bb_to_step(1, 3.0), tuned_step=53+17, gate=3.8, velocity=0.90) # Sweet just 3rd!
solo1_lead.add_note(bb_to_step(1, 4.0), tuned_step=53+31, gate=3.8, velocity=0.85)

solo1_lead.add_note(bb_to_step(2, 1.0), tuned_step=53+39, gate=3.8, velocity=0.88)
solo1_lead.add_note(bb_to_step(2, 2.0), tuned_step=53+31, gate=1.8, velocity=0.82)
solo1_lead.add_note(bb_to_step(2, 2.5), tuned_step=53+22, gate=1.8, velocity=0.80)
solo1_lead.add_note(bb_to_step(2, 3.0), tuned_step=53+13, gate=3.8, velocity=0.84)
solo1_lead.add_note(bb_to_step(2, 4.0), tuned_step=53+22, gate=3.8, velocity=0.82)

solo1_lead.add_note(bb_to_step(3, 1.0), tuned_step=53+18, gate=3.8, velocity=0.88) # Bright third!
solo1_lead.add_note(bb_to_step(3, 2.0), tuned_step=53+31, gate=1.8, velocity=0.82)
solo1_lead.add_note(bb_to_step(3, 2.5), tuned_step=53+44, gate=1.8, velocity=0.85)
solo1_lead.add_note(bb_to_step(3, 3.0), tuned_step=53+53, gate=3.8, velocity=0.90) # High C
solo1_lead.add_note(bb_to_step(3, 4.0), tuned_step=53+44, gate=3.8, velocity=0.82)

solo1_lead.add_note(bb_to_step(4, 1.0), tuned_step=53+18, gate=1.8, velocity=0.88)
solo1_lead.add_note(bb_to_step(4, 1.5), tuned_step=53+17, gate=3.8, velocity=0.86) # Resolved!
solo1_lead.add_note(bb_to_step(4, 2.5), tuned_step=53+9,  gate=1.8, velocity=0.80)
solo1_lead.add_note(bb_to_step(4, 3.0), tuned_step=53+0,  gate=7.8, velocity=0.84)

solo1_lead.add_note(bb_to_step(5, 1.0), tuned_step=53+22, gate=1.8, velocity=0.86)
solo1_lead.add_note(bb_to_step(5, 1.5), tuned_step=53+39, gate=3.8, velocity=0.88)
solo1_lead.add_note(bb_to_step(5, 2.5), tuned_step=53+44, gate=1.8, velocity=0.85)
solo1_lead.add_note(bb_to_step(5, 3.0), tuned_step=53+53, gate=3.8, velocity=0.90)

solo1_lead.add_note(bb_to_step(6, 1.0), tuned_step=53+27, gate=1.8, velocity=0.88)
solo1_lead.add_note(bb_to_step(6, 1.5), tuned_step=53+40, gate=1.8, velocity=0.86)
solo1_lead.add_note(bb_to_step(6, 2.0), tuned_step=53+53, gate=1.8, velocity=0.90)
solo1_lead.add_note(bb_to_step(6, 2.5), tuned_step=53+66, gate=3.8, velocity=0.92)

solo1_lead.add_note(bb_to_step(7, 1.0), tuned_step=53+44, gate=1.8, velocity=0.86)
solo1_lead.add_note(bb_to_step(7, 1.5), tuned_step=53+31, gate=1.8, velocity=0.84)
solo1_lead.add_note(bb_to_step(7, 2.0), tuned_step=53+17, gate=1.8, velocity=0.86)
solo1_lead.add_note(bb_to_step(7, 2.5), tuned_step=53+13, gate=1.8, velocity=0.82)
solo1_lead.add_note(bb_to_step(7, 3.0), tuned_step=53+9,  gate=1.8, velocity=0.80)
solo1_lead.add_note(bb_to_step(7, 3.5), tuned_step=53+0,  gate=3.8, velocity=0.82)

solo1_lead.add_note(bb_to_step(8, 1.0), tuned_step=58,    gate=3.8, velocity=0.90)
solo1_lead.add_note(bb_to_step(8, 2.0), tuned_step=71,    gate=3.8, velocity=0.88)
solo1_lead.add_note(bb_to_step(8, 3.0), tuned_step=62,    gate=3.8, velocity=0.84)
solo1_lead.add_note(bb_to_step(8, 4.0), tuned_step=58,    gate=3.8, velocity=0.86)

solo1_lead.add_note(bb_to_step(9, 1.0), tuned_step=62,    gate=3.8, velocity=0.85)
solo1_lead.add_note(bb_to_step(9, 2.0), tuned_step=76,    gate=11.8, velocity=0.95) # Held exposed comma F!

solo1_lead.add_note(bb_to_step(10, 1.0), tuned_step=75,   gate=3.8, velocity=0.90)
solo1_lead.add_note(bb_to_step(10, 2.0), tuned_step=70,   gate=1.8, velocity=0.85)
solo1_lead.add_note(bb_to_step(10, 2.5), tuned_step=62,   gate=1.8, velocity=0.82)
solo1_lead.add_note(bb_to_step(10, 3.0), tuned_step=48,   gate=7.8, velocity=0.84)

solo1_lead.add_note(bb_to_step(11, 1.0), tuned_step=53+17, gate=3.8, velocity=0.88)
solo1_lead.add_note(bb_to_step(11, 2.0), tuned_step=53+31, gate=3.8, velocity=0.86)
solo1_lead.add_note(bb_to_step(11, 3.0), tuned_step=53+48, gate=7.8, velocity=0.90)

solo1_lead.add_note(bb_to_step(12, 1.0), tuned_step=53+31, gate=3.8, velocity=0.84)
solo1_lead.add_note(bb_to_step(12, 2.0), tuned_step=53+48, gate=1.8, velocity=0.86)
solo1_lead.add_note(bb_to_step(12, 2.5), tuned_step=53+44, gate=1.8, velocity=0.84)
solo1_lead.add_note(bb_to_step(12, 3.0), tuned_step=53+35, gate=3.8, velocity=0.88) # Ab altered tone!

score.add_pattern("p_solo1_lead", solo1_lead)

# ══════════════════════════════════════════════════════════════════════════════
# SECTION 4: SOLO CHORUS 2 (12 BARS = 192 STEPS)
# ══════════════════════════════════════════════════════════════════════════════
solo2_lead = PatternBuilder(length=192, resolution="1/16", pitch_context="c53")

solo2_lead.add_note(bb_to_step(1, 1.0), tuned_step=106,   gate=5.8, velocity=0.95) # High C (53 + 53)
solo2_lead.add_note(bb_to_step(1, 2.5), tuned_step=106-9, gate=1.8, velocity=0.88)
solo2_lead.add_note(bb_to_step(1, 3.0), tuned_step=106-14, gate=1.8, velocity=0.86)
solo2_lead.add_note(bb_to_step(1, 3.5), tuned_step=106-22, gate=3.8, velocity=0.84)

solo2_lead.add_note(bb_to_step(2, 1.0), tuned_step=53+39, gate=3.8, velocity=0.90)
solo2_lead.add_note(bb_to_step(2, 2.0), tuned_step=53+44, gate=1.8, velocity=0.86)
solo2_lead.add_note(bb_to_step(2, 2.5), tuned_step=53+53, gate=1.8, velocity=0.92)
solo2_lead.add_note(bb_to_step(2, 3.0), tuned_step=53+66, gate=3.8, velocity=0.94)

solo2_lead.add_note(bb_to_step(3, 1.0), tuned_step=71,    gate=1.8, velocity=0.92) # Bright E
solo2_lead.add_note(bb_to_step(3, 1.5), tuned_step=70,    gate=3.8, velocity=0.90) # Sweet E
solo2_lead.add_note(bb_to_step(3, 2.5), tuned_step=84,    gate=3.8, velocity=0.92) # G
solo2_lead.add_note(bb_to_step(3, 3.5), tuned_step=97,    gate=3.8, velocity=0.94) # Bb

solo2_lead.add_note(bb_to_step(4, 1.0), tuned_step=106,   gate=3.8, velocity=0.95)
solo2_lead.add_note(bb_to_step(4, 2.0), tuned_step=97,    gate=1.8, velocity=0.88)
solo2_lead.add_note(bb_to_step(4, 2.5), tuned_step=84,    gate=1.8, velocity=0.86)
solo2_lead.add_note(bb_to_step(4, 3.0), tuned_step=70,    gate=7.8, velocity=0.90)

solo2_lead.add_note(bb_to_step(5, 1.0), tuned_step=75,    gate=3.8, velocity=0.92)
solo2_lead.add_note(bb_to_step(5, 2.0), tuned_step=92,    gate=3.8, velocity=0.94)
solo2_lead.add_note(bb_to_step(5, 3.0), tuned_step=97,    gate=3.8, velocity=0.92)

solo2_lead.add_note(bb_to_step(6, 1.0), tuned_step=80,    gate=1.8, velocity=0.90)
solo2_lead.add_note(bb_to_step(6, 1.5), tuned_step=93,    gate=1.8, velocity=0.92)
solo2_lead.add_note(bb_to_step(6, 2.0), tuned_step=106,   gate=1.8, velocity=0.95)
solo2_lead.add_note(bb_to_step(6, 2.5), tuned_step=119,   gate=5.8, velocity=0.96) # High peak!

solo2_lead.add_note(bb_to_step(7, 1.0), tuned_step=106,   gate=3.8, velocity=0.92)
solo2_lead.add_note(bb_to_step(7, 2.0), tuned_step=84,    gate=3.8, velocity=0.88)
solo2_lead.add_note(bb_to_step(7, 3.0), tuned_step=70,    gate=3.8, velocity=0.86)
solo2_lead.add_note(bb_to_step(7, 4.0), tuned_step=53,    gate=3.8, velocity=0.84)

solo2_lead.add_note(bb_to_step(8, 1.0), tuned_step=58,    gate=1.8, velocity=0.92)
solo2_lead.add_note(bb_to_step(8, 1.5), tuned_step=66,    gate=1.8, velocity=0.94) # Eb altered
solo2_lead.add_note(bb_to_step(8, 2.0), tuned_step=71,    gate=3.8, velocity=0.90)
solo2_lead.add_note(bb_to_step(8, 3.0), tuned_step=58,    gate=3.8, velocity=0.88)

solo2_lead.add_note(bb_to_step(9, 1.0), tuned_step=62,    gate=3.8, velocity=0.86)
solo2_lead.add_note(bb_to_step(9, 2.0), tuned_step=76,    gate=7.8, velocity=0.95) # Comma F held!

solo2_lead.add_note(bb_to_step(10, 1.0), tuned_step=75,   gate=1.8, velocity=0.92) # Resolving comma
solo2_lead.add_note(bb_to_step(10, 1.5), tuned_step=53+35, gate=1.8, velocity=0.94) # Ab (88)
solo2_lead.add_note(bb_to_step(10, 2.0), tuned_step=53+45, gate=3.8, velocity=0.95) # A# / Bb+ (98)
solo2_lead.add_note(bb_to_step(10, 3.0), tuned_step=53+57, gate=3.8, velocity=0.96) # Db (110)

solo2_lead.add_note(bb_to_step(11, 1.0), tuned_step=106,  gate=7.8, velocity=0.96)
solo2_lead.add_note(bb_to_step(11, 3.0), tuned_step=84,   gate=3.8, velocity=0.88)
solo2_lead.add_note(bb_to_step(11, 4.0), tuned_step=70,   gate=3.8, velocity=0.86)

solo2_lead.add_note(bb_to_step(12, 1.0), tuned_step=62,   gate=3.8, velocity=0.85)
solo2_lead.add_note(bb_to_step(12, 2.0), tuned_step=75,   gate=1.8, velocity=0.88)
solo2_lead.add_note(bb_to_step(12, 2.5), tuned_step=70,   gate=1.8, velocity=0.86)
solo2_lead.add_note(bb_to_step(12, 3.0), tuned_step=62,   gate=3.8, velocity=0.84)

score.add_pattern("p_solo2_lead", solo2_lead)

# ══════════════════════════════════════════════════════════════════════════════
# SECTION 5: TAG (2 BARS = 32 STEPS)
# ══════════════════════════════════════════════════════════════════════════════
tag_lead = PatternBuilder(length=32, resolution="1/16", pitch_context="c53")
tag_lead.add_note(bb_to_step(1, 1.0), tuned_step=106, gate=15.0, velocity=0.85) # C5
tag_lead.add_note(bb_to_step(2, 1.0), tuned_step=107, gate=15.0, velocity=0.88) # C5+ (shifted 22.6 cents!)

tag_bass = PatternBuilder(length=32, resolution="1/16", pitch_context="c53")
tag_bass.add_note(bb_to_step(1, 1.0), tuned_step=-53, gate=15.0, velocity=0.88) # C2
tag_bass.add_note(bb_to_step(2, 1.0), tuned_step=-52, gate=15.0, velocity=0.90) # C2+

tag_c1 = PatternBuilder(length=32, resolution="1/16", pitch_context="c53")
tag_c2 = PatternBuilder(length=32, resolution="1/16", pitch_context="c53")
tag_c3 = PatternBuilder(length=32, resolution="1/16", pitch_context="c53")
tag_c4 = PatternBuilder(length=32, resolution="1/16", pitch_context="c53")

# T1: 0, 17, 31, 48
tag_c1.add_note(bb_to_step(1, 1.0), tuned_step=48, gate=15.0, velocity=0.74)
tag_c2.add_note(bb_to_step(1, 1.0), tuned_step=31, gate=15.0, velocity=0.72)
tag_c3.add_note(bb_to_step(1, 1.0), tuned_step=17, gate=15.0, velocity=0.70)
tag_c4.add_note(bb_to_step(1, 1.0), tuned_step=0,  gate=15.0, velocity=0.68)

# T2: 1, 18, 32, 49 (+1 step!)
tag_c1.add_note(bb_to_step(2, 1.0), tuned_step=49, gate=15.0, velocity=0.76)
tag_c2.add_note(bb_to_step(2, 1.0), tuned_step=32, gate=15.0, velocity=0.74)
tag_c3.add_note(bb_to_step(2, 1.0), tuned_step=18, gate=15.0, velocity=0.72)
tag_c4.add_note(bb_to_step(2, 1.0), tuned_step=1,  gate=15.0, velocity=0.70)

score.add_pattern("p_tag_lead", tag_lead)
score.add_pattern("p_tag_bass", tag_bass)
score.add_pattern("p_tag_c1", tag_c1)
score.add_pattern("p_tag_c2", tag_c2)
score.add_pattern("p_tag_c3", tag_c3)
score.add_pattern("p_tag_c4", tag_c4)

# ══════════════════════════════════════════════════════════════════════════════
# ARRANGEMENT SCENES (54 BARS TOTAL)
# ══════════════════════════════════════════════════════════════════════════════
score.add_operation({
    "op": "upsert_scene",
    "id": "intro",
    "scene": {
        "lengthBeats": 16.0,
        "repeats": 1,
        "description": "Intro: comma sigh over C pedal (steps 71->70->62->53)",
        "tracks": {
            "lead": "p_intro_lead", "bass": "p_intro_bass",
            "chord1": "p_intro_c1", "chord2": "p_intro_c2", "chord3": "p_intro_c3", "chord4": "p_intro_c4"
        }
    }
})

score.add_operation({
    "op": "upsert_scene",
    "id": "head",
    "scene": {
        "lengthBeats": 48.0,
        "repeats": 1,
        "description": "Head: 12-bar blues with bar 4 (71->70) & bar 9-10 (76->75) comma resolutions",
        "tracks": {
            "lead": "p_head_lead", "bass": "p_head_bass",
            "chord1": "p_head_c1", "chord2": "p_head_c2", "chord3": "p_head_c3", "chord4": "p_head_c4"
        }
    }
})

score.add_operation({
    "op": "upsert_scene",
    "id": "solo1",
    "scene": {
        "lengthBeats": 48.0,
        "repeats": 1,
        "description": "Solo Chorus 1: sweet (17) vs bright (18) thirds, blue-third zone (15-16)",
        "tracks": {
            "lead": "p_solo1_lead", "bass": "p_head_bass",
            "chord1": "p_head_c1", "chord2": "p_head_c2", "chord3": "p_head_c3", "chord4": "p_head_c4"
        }
    }
})

score.add_operation({
    "op": "upsert_scene",
    "id": "solo2",
    "scene": {
        "lengthBeats": 48.0,
        "repeats": 1,
        "description": "Solo Chorus 2: high-register altered dominants, comma bends, climax",
        "tracks": {
            "lead": "p_solo2_lead", "bass": "p_head_bass",
            "chord1": "p_head_c1", "chord2": "p_head_c2", "chord3": "p_head_c3", "chord4": "p_head_c4"
        }
    }
})

score.add_operation({
    "op": "upsert_scene",
    "id": "head_out",
    "scene": {
        "lengthBeats": 48.0,
        "repeats": 1,
        "description": "Head Out: dynamic reprise of the blues theme",
        "tracks": {
            "lead": "p_head_lead", "bass": "p_head_bass",
            "chord1": "p_head_c1", "chord2": "p_head_c2", "chord3": "p_head_c3", "chord4": "p_head_c4"
        }
    }
})

score.add_operation({
    "op": "upsert_scene",
    "id": "tag",
    "scene": {
        "lengthBeats": 8.0,
        "repeats": 1,
        "description": "Tag: Cmaj7 shifting +1 syntonic comma sharp to C+ (question mark)",
        "tracks": {
            "lead": "p_tag_lead", "bass": "p_tag_bass",
            "chord1": "p_tag_c1", "chord2": "p_tag_c2", "chord3": "p_tag_c3", "chord4": "p_tag_c4"
        }
    }
})

print(f"5. Committing score to Sibyl (2-Phase Commit, expected rev {current_rev})...")
receipt = score.two_phase_commit(client=client, expected_revision=current_rev, response_profile="receipt")
print("Commit receipt:", json.dumps(receipt, indent=2))

print("6. Starting transport...")
post_json(f"sibyl/{sibyl_id}/transport", {"running": True})

print("SUCCESS: 'Blue Comma' in 53-EDO is now playing live in VCV Rack!")
