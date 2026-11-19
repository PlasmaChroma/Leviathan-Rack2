// Compile the renderer in this translation unit to exercise its private path
// conversion without adding a public API solely for tests. Rack supplies NanoVG.
#include "../src/common/visual/PlasmaConduit.cpp"
#include <iostream>
#include <stdexcept>
using namespace visual_assets;
using Command=panel_svg::SvgPathCommand;
void require(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
bool near(Vec a,Vec b){return (a-b).norm()<1e-4f;}
int main(){try {
    panel_svg::SvgPathMatch path;
    auto add=[&](Command::Type type,Vec p1={},Vec p2={},Vec p3={}){
        Command c;c.type=type;c.p1=p1;c.p2=p2;c.p3=p3;path.commands.push_back(c);
    };
    add(Command::MoveTo,{1,2});add(Command::LineTo,{3,4});
    add(Command::QuadTo,{5,6},{7,8});
    add(Command::BezierTo,{9,10},{11,12},{13,14});
    add(Command::MoveTo,{20,20});
    add(Command::BezierTo,{22,24},{18,24},{20,20}); // Nonzero loop, equal endpoints.
    std::vector<PlasmaConduitSegment> segments;
    require(appendConduitSegments(path,&segments)&&segments.size()==4,"all open segment types retained");
    require(near(segments[0].start,mm2px(Vec(1,2)))&&near(segments[0].end,mm2px(Vec(3,4))),"legacy line geometry unchanged");
    require(segments[1].type==Command::QuadTo&&near(segments[1].control1,mm2px(Vec(5,6)))&&near(segments[1].end,mm2px(Vec(7,8))),"quadratic control and endpoint");
    require(segments[2].type==Command::BezierTo&&near(segments[2].control2,mm2px(Vec(11,12)))&&near(segments[2].start,segments[1].end),"cubic controls and continuity");
    require(near(segments[3].start,mm2px(Vec(20,20)))&&near(segments[3].end,segments[3].start),"disjoint move and returning curve");
    add(Command::Close);
    require(!appendConduitSegments(path,&segments)&&segments.size()==4,"unsupported path rolls back without truncation");
    path.commands.clear();add(Command::LineTo,{2,3});
    require(!appendConduitSegments(path,&segments)&&segments.size()==4,"missing move rejected");
    std::vector<panel_svg::SvgPathMatch> actual;
    require(panel_svg::findPathsInGroupsWithIdSubstringMm(
        "res/iris.panel.svg", kPlasmaConduitAnchorGroupId, &actual), "Iris conduit lookup");
    const auto guide = std::find_if(actual.begin(), actual.end(), [](const panel_svg::SvgPathMatch& item) {
        return item.id == "iris_freq_guide";
    });
    require(guide != actual.end(), "Iris frequency guide found");
    segments.clear();
    require(appendConduitSegments(*guide, &segments) && segments.size() == 1, "continuous frequency guide");
    const auto& line = segments.front();
    require(line.type == Command::LineTo && line.end.x > line.start.x &&
        std::abs(line.end.y - line.start.y) < 1e-4f, "frequency guide runs horizontally to the right");
    std::cout << "PASS plasma conduit geometry and Iris panel routing\n";
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
