"""Extract the exact production draw methods for the isolated Eclipse2 probe."""
from pathlib import Path

root = Path(__file__).resolve().parents[3]
source = (root / "src/visual/VisualAssets.cpp").read_text(encoding="utf-8")
sections = []
for start, end in [
    ("EclipseKnob::SvgLayer::SvgLayer()", "EclipseKnob::ShadowWidget::ShadowWidget()"),
    ("void Eclipse2Knob::ProgressLedRingWidget::draw", "Eclipse2Knob::Eclipse2Knob()"),
]:
    begin = source.index(start)
    section = source[begin:source.index(end, begin)]
    if start.startswith("void Eclipse2Knob"):
        # Keep all candidates offline: inject only the experimental cache seams
        # into the source-extracted baseline, leaving the plugin renderer intact.
        a = section.index("\teclipse2_track::draw(")
        b = section.index("\t// Active track background glow", a)
        track = section[a:b]
        prelude = '''void drawTrackReference(const Widget::DrawArgs& args) {
 const Vec center(17,17); const float radiusPx=12.75f,largeRadiusPx=.51f;
 const float minAngle=-.83f*M_PI,maxAngle=.83f*M_PI;
 nvgSave(args.vg);
''' + track + '\n nvgRestore(args.vg);\n}\n'
        section = section[:a] + '\tif (!eclipse2_ring::drawTrack(args)) {\n' + track + '\t}\n' + section[b:]
        section = section.replace('const float x = center.x + radiusPx * std::sin(angle);\n\t\tconst float y = center.y - radiusPx * std::cos(angle);', '''const Vec direction = eclipse2_ring::cachedGeometry() && numLeds == 25
            ? eclipse2_ring::ledPosition(i,minAngle,maxAngle) : Vec(std::sin(angle),-std::cos(angle));
        const float x = center.x + radiusPx * direction.x;
        const float y = center.y + radiusPx * direction.y;''')
        section = section.replace('if (bloom > 0.001f) {\n\t\t\t\t// Glow aura', 'if (bloom > 0.001f && !eclipse2_ring::glow(args,x,y,litR,bloom)) {\n\t\t\t\t// Glow aura')
        section = prelude + section
    sections.append(section)
destination = root / "build/tools/eclipse2/reference.inc"
destination.parent.mkdir(parents=True, exist_ok=True)
destination.write_text("\n".join(sections), encoding="utf-8")
