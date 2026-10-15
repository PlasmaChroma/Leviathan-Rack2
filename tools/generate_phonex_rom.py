#!/usr/bin/env python3
"""Generate deterministic clean-room PHONEX phoneme and phrase data."""

import argparse
import json
import math
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "src" / "PhonexRomData.inc"
INPUT_DIR = ROOT / "tools" / "phonex_rom"
INPUTS = ("phonemes.json", "pronunciations.tsv", "bundled_phrases.tsv", "PROVENANCE.md")
PHONE_ORDER = "AA AE AH AO AW AY B CH D DH EH ER EY F G HH IH IY JH K L M N NG OW OY P R S SH T TH UH UW V W Y Z ZH SIL".split()
BUNDLE_ORDER = list("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + [
    "ZERO", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN",
    "EIGHT", "NINE", "HELLO", "READY", "SPEAK", "SPELL", "AGAIN",
    "CORRECT", "WRONG", "YES", "NO", "COMPUTER", "MACHINE", "ROBOT",
    "VOLTAGE", "CIRCUIT", "SIGNAL", "PITCH", "VOICE", "SYNTH", "GLITCH",
    "BEND", "ERROR", "WARNING", "START", "STOP", "ENTER", "LISTEN",
    "LEVIATHAN", "PHONEX",
]


def convolve(left, right):
    result = [0.0] * (len(left) + len(right) - 1)
    for i, a in enumerate(left):
        for j, b in enumerate(right):
            result[i + j] += a * b
    return result


def formants_to_reflection(formants):
    """Five pole pairs -> order-10 denominator -> step-down reflection K."""
    if len(formants) != 5:
        raise ValueError("formant anchor must contain exactly five [Hz, bandwidth] pairs")
    polynomial = [1.0]
    for frequency, bandwidth in formants:
        if not (0.0 < frequency < 5000.0 and bandwidth > 0.0):
            raise ValueError(f"invalid formant pair: {frequency}, {bandwidth}")
        radius = math.exp(-math.pi * bandwidth / 10000.0)
        angle = 2.0 * math.pi * frequency / 10000.0
        polynomial = convolve(polynomial, [1.0, -2.0 * radius * math.cos(angle), radius * radius])
    coefficients = polynomial[:]
    reflection = [0.0] * 10
    for order in range(10, 0, -1):
        k = coefficients[order]
        if not math.isfinite(k) or abs(k) >= 0.995:
            raise ValueError(f"unstable generated reflection coefficient K{order}: {k}")
        reflection[order - 1] = k
        denominator = 1.0 - k * k
        coefficients = [coefficients[0]] + [
            (coefficients[i] - k * coefficients[order - i]) / denominator
            for i in range(1, order)
        ]
    return reflection


def parse_tsv(path, columns):
    rows = []
    for line_number, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        fields = raw.split("\t")
        if len(fields) != columns:
            raise ValueError(f"{path.relative_to(ROOT)}:{line_number}: expected {columns} tab-separated fields")
        rows.append(tuple(field.strip() for field in fields))
    return rows


def cpp_float(value):
    if not math.isfinite(value):
        raise ValueError("nonfinite generated corpus value")
    text = format(float(value), ".9g")
    if "." not in text and "e" not in text:
        text += ".0"
    return text + "f"


def validate_script(script, context):
    for raw in script.split():
        symbol = raw[:-1] if raw[-1:] in ("0", "1", "2") else raw
        if symbol not in PHONE_ORDER:
            raise ValueError(f"{context}: unknown phone {raw}")


def render_frame(anchor):
    excitation = anchor["excitation"]
    if excitation not in ("Silence", "Unvoiced", "Voiced"):
        raise ValueError(f"invalid excitation {excitation}")
    energy = float(anchor["energy"])
    pitch = float(anchor.get("pitch", 0.0))
    if not (math.isfinite(energy) and 0.0 <= energy <= 1.0):
        raise ValueError(f"invalid energy {energy}")
    if excitation == "Voiced" and not (pitch > 0.0 and math.isfinite(pitch)):
        raise ValueError("voiced anchors require a finite positive pitch period")
    if "formants" in anchor:
        reflection = formants_to_reflection(anchor["formants"])
    else:
        reflection = [float(value) for value in anchor.get("reflection", [])]
        if len(reflection) != 10:
            raise ValueError("explicit reflection anchor must contain ten coefficients")
    if any(not math.isfinite(k) or abs(k) > 0.995 for k in reflection):
        raise ValueError("nonfinite or unstable clean reflection coefficient")
    coefficients = ", ".join(cpp_float(k) for k in reflection)
    return (f"LpcFrame{{{cpp_float(energy)}, {cpp_float(pitch)}, "
            f"{{{{{coefficients}}}}}, Excitation::{excitation}}}")


def render():
    for name in INPUTS:
        path = INPUT_DIR / name
        if not path.is_file():
            raise ValueError(f"missing corpus source: {path.relative_to(ROOT)}")
    if not (INPUT_DIR / "PROVENANCE.md").read_text(encoding="utf-8").strip():
        raise ValueError("PHONEX corpus provenance must not be empty")
    document = json.loads((INPUT_DIR / "phonemes.json").read_text(encoding="utf-8"))
    phones = document.get("phonemes", [])
    symbols = [phone.get("symbol") for phone in phones]
    if symbols != PHONE_ORDER:
        raise ValueError("phoneme inventory/order does not match the frozen 40-symbol contract")
    rendered_phones = []
    for phone in phones:
        duration = int(phone.get("duration", 0))
        anchors = phone.get("anchors", [])
        if not (1 <= duration <= 12 and 1 <= len(anchors) <= 3):
            raise ValueError(f"{phone['symbol']}: invalid duration or anchor count")
        frames = [render_frame(anchor) for anchor in anchors]
        while len(frames) < 3:
            frames.append("LpcFrame{}")
        rendered_phones.append(
            f"\tPhonePrototype{{{duration}u, {len(anchors)}u, "
            f"{{{{{', '.join(frames)}}}}}}}")
    pronunciations = parse_tsv(INPUT_DIR / "pronunciations.tsv", 2)
    pronunciation_words = set()
    for word, script in pronunciations:
        if word in pronunciation_words:
            raise ValueError(f"duplicate pronunciation: {word}")
        pronunciation_words.add(word)
        validate_script(script, f"pronunciation {word}")
    missing = [word for word in BUNDLE_ORDER if word not in pronunciation_words]
    if missing:
        raise ValueError(f"missing required pronunciations: {', '.join(missing)}")
    bundles = parse_tsv(INPUT_DIR / "bundled_phrases.tsv", 3)
    if len(bundles) != 64:
        raise ValueError(f"bundled bank must contain exactly 64 entries, found {len(bundles)}")
    names = []
    scripts = []
    for expected_index, (index_text, name, script) in enumerate(bundles):
        if int(index_text) != expected_index or name != BUNDLE_ORDER[expected_index]:
            raise ValueError(f"bundled index {expected_index} violates frozen ordering")
        validate_script(script, f"bundled phrase {name}")
        names.append(name)
        scripts.append(script)
    lines = [
        "// Generated by tools/generate_phonex_rom.py. Do not edit.", "",
        "namespace phonex {", "constexpr unsigned kRomSchemaVersion = 1u;",
        "const std::array<StringView, kPhoneCount> kGeneratedPhoneSymbols {{",
        "\t" + ", ".join(f'\"{symbol}\"' for symbol in PHONE_ORDER), "}};",
        "const std::array<PhonePrototype, kPhoneCount> kGeneratedPhonePrototypes {{",
        ",\n".join(rendered_phones), "}};",
        "const std::array<StringView, kBundledPhraseCount> kGeneratedPhraseNames {{",
        "\t" + ", ".join(f'\"{name}\"' for name in names), "}};",
        "const std::array<StringView, kBundledPhraseCount> kGeneratedPhraseScripts {{",
        "\t" + ",\n\t".join(f'\"{script}\"' for script in scripts), "}};",
        f"const std::array<StringView, {len(pronunciations)}> kGeneratedDictionaryWords {{{{",
        "\t" + ",\n\t".join(f'\"{word}\"' for word, _ in pronunciations), "}};",
        f"const std::array<StringView, {len(pronunciations)}> kGeneratedDictionaryScripts {{{{",
        "\t" + ",\n\t".join(f'\"{script}\"' for _, script in pronunciations), "}};",
        "} // namespace phonex", "",
    ]
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    try:
        generated = render()
    except (ValueError, KeyError, TypeError, json.JSONDecodeError) as error:
        print(error, file=sys.stderr)
        return 1
    if args.check:
        if not OUTPUT.is_file() or OUTPUT.read_text(encoding="utf-8") != generated:
            print(f"stale generated file: {OUTPUT.relative_to(ROOT)}", file=sys.stderr)
            return 1
        print("PHONEX ROM: 40 phones, 64 phrases; deterministic and current")
        return 0
    OUTPUT.write_text(generated, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
