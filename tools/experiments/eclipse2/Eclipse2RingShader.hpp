#pragma once
#include "visual/VisualAssets.hpp"
#include "visual/AdaptiveGlSurface.hpp"
#include "GlLifecycleUtils.hpp"
#include "Eclipse2RingExperiment.hpp"
#include <array>

namespace eclipse2_ring {

inline const char* vertexSource() { return R"GLSL(
#version 120
attribute vec2 aPos;
varying vec2 p;
void main() { p=aPos*17.0; gl_Position=vec4(aPos.x,-aPos.y,0.0,1.0); }
)GLSL"; }
inline const char* fragmentSource() { return R"GLSL(
#version 120
varying vec2 p;
uniform vec4 uLeds[25];
uniform vec4 uArc;
uniform vec4 uEnds;
uniform vec4 uActiveEnds;
uniform float uBloom;
uniform float uAa;
const float R=12.75;
const float r=0.51;
void over(inout vec4 a, vec4 c, float coverage) {
 float alpha=clamp(c.a*coverage,0.0,1.0);
 a=vec4(c.rgb*alpha+a.rgb*(1.0-alpha),alpha+a.a*(1.0-alpha));
}
float disk(float d,float radius) { return clamp((radius-d)/uAa+0.5,0.0,1.0); }
float stroke(float d,float radius,float width) { return disk(abs(d-radius),width*0.5); }
float arc(float theta,float radius,vec2 range,vec4 ends) {
 return theta>=range.x&&theta<=range.y ? abs(radius-R) : min(length(p-ends.xy),length(p-ends.zw));
}
void main() {
 float radius=length(p),theta=atan(p.x,-p.y);
 float d=arc(theta,radius,uArc.xy,uEnds);
 vec4 a=vec4(0.0);
 over(a,vec4(3.0,2.0,2.0,96.0)/255.0,disk(d,r*2.2));
 over(a,vec4(0.0,0.0,0.0,245.0)/255.0,disk(d,r*1.7));
 over(a,vec4(14.0,12.0,11.0,230.0)/255.0,disk(d,r*1.2));
 over(a,vec4(255.0,220.0,150.0,16.0)/255.0,disk(d,r*0.9));
 if(uArc.w-uArc.z>0.008&&uBloom>0.001) {
  float ad=arc(theta,radius,uArc.zw,uActiveEnds);
  over(a,vec4(1.0,175.0/255.0,40.0/255.0,36.0/255.0*uBloom),disk(ad,r*3.25));
  over(a,vec4(1.0,215.0/255.0,95.0/255.0,76.0/255.0*uBloom),disk(ad,r*2.1));
 }
 int nearest=int(clamp(floor((theta-uArc.x)/(uArc.y-uArc.x)*24.0+0.5),0.0,24.0));
 for(int offset=-1;offset<=1;++offset) {
  int i=nearest+offset;
  if(i<0||i>=25) continue;
  float distance=length(p-uLeds[i].xy);
  if(distance>2.0+uAa) continue;
  if(uLeds[i].z>0.5) {
   float litR=r*0.88;
   if(uBloom>0.001) {
    float t=clamp((distance-litR*0.4)/(litR*2.8),0.0,1.0);
    vec4 glow=vec4(1.0,235.0/255.0,140.0/255.0,1.0-t);
    glow.a*=uBloom; over(a,glow,disk(distance,litR*3.2));
   }
   over(a,vec4(255.0,252.0,200.0,255.0)/255.0,disk(distance,litR));
   over(a,vec4(1.0),disk(distance,litR*0.55));
   over(a,vec4(255.0,200.0,50.0,245.0)/255.0,stroke(distance,litR,0.35));
  } else {
   over(a,vec4(142.0,124.0,72.0,118.0)/255.0,disk(distance,r));
   over(a,vec4(80.0,70.0,40.0,96.0)/255.0,stroke(distance,r,0.3));
  }
 }
 gl_FragColor=a;
}
)GLSL"; }

struct Ring : Eclipse2Knob::ProgressLedRingWidget {
	visual_assets::AdaptiveGlSurface surface;
	gl_lifecycle::ContextLease lease;
	GLuint program=0, vbo=0;
	bool failed=false;
	float oldMin=100.f, oldMax=100.f, oldValue=-1.f, oldBloom=-1.f, oldCenter=-1.f;
	bool oldBipolar=false;
	std::array<GLfloat,100> leds{};
	GLint ledLocation=-1, arcLocation=-1, endsLocation=-1, activeEndsLocation=-1, bloomLocation=-1, aaLocation=-1;
	float bloom=0.f;
	void release() {
		gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Program,program);
		gl_lifecycle::retireObject(lease,gl_lifecycle::ObjectKind::Buffer,vbo);
		program=vbo=0;lease.reset();failed=false;
	}
	~Ring() override { release(); }
	void onContextCreate(const ContextCreateEvent& e) override {
		surface.reset(false);program=vbo=0;lease.reset();failed=false;
		ProgressLedRingWidget::onContextCreate(e);
	}
	void onContextDestroy(const ContextDestroyEvent& e) override {
		surface.reset(true);release();ProgressLedRingWidget::onContextDestroy(e);
	}
	static GLuint compile(GLenum kind,const char* text) {
		GLuint shader=glCreateShader(kind);glShaderSource(shader,1,&text,nullptr);glCompileShader(shader);
		GLint ok=0;glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
		if(!ok) { char log[1024]{};glGetShaderInfoLog(shader,sizeof(log),nullptr,log);WARN("Eclipse2 ring shader: %s",log);glDeleteShader(shader);return 0; }
		return shader;
	}
	bool initialize() {
		if(program&&vbo) return true;
		if(failed) return false;
		GLuint vs=compile(GL_VERTEX_SHADER,vertexSource()), fs=compile(GL_FRAGMENT_SHADER,fragmentSource());
		if(vs&&fs) {
			program=glCreateProgram();glAttachShader(program,vs);glAttachShader(program,fs);glBindAttribLocation(program,0,"aPos");glLinkProgram(program);
			GLint ok=0;glGetProgramiv(program,GL_LINK_STATUS,&ok);
			if(!ok) {glDeleteProgram(program);program=0;}
		}
		if(vs) glDeleteShader(vs);if(fs) glDeleteShader(fs);
		if(!program) {failed=true;return false;}
		glGenBuffers(1,&vbo);glBindBuffer(GL_ARRAY_BUFFER,vbo);
		const GLfloat vertices[]={-1,-1,1,-1,-1,1,1,1};glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
		ledLocation=glGetUniformLocation(program,"uLeds");arcLocation=glGetUniformLocation(program,"uArc");
		endsLocation=glGetUniformLocation(program,"uEnds");activeEndsLocation=glGetUniformLocation(program,"uActiveEnds");
		bloomLocation=glGetUniformLocation(program,"uBloom");aaLocation=glGetUniformLocation(program,"uAa");
		return vbo!=0;
	}
	void render(Vec active,int viewportY) {
		if(!initialize()) return;
		glViewport(0,viewportY,int(active.x),int(active.y));glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);
		glUseProgram(program);glUniform4fv(ledLocation,25,leds.data());
		const float lo=std::min(bipolar?centerNorm:0.f,valueNorm), hi=std::max(bipolar?centerNorm:0.f,valueNorm);
		const float a=minAngle+lo*(maxAngle-minAngle), b=minAngle+hi*(maxAngle-minAngle);
		glUniform4f(arcLocation,minAngle,maxAngle,a,b);
		glUniform4f(endsLocation,leds[0],leds[1],leds[96],leds[97]);
		glUniform4f(activeEndsLocation,12.75f*std::sin(a),-12.75f*std::cos(a),12.75f*std::sin(b),-12.75f*std::cos(b));
		glUniform1f(bloomLocation,bloom);glUniform1f(aaLocation,34.f/active.x);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);glEnableVertexAttribArray(0);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,nullptr);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	}
	bool prepareUpdate(const DrawArgs& args,visual_assets::AdaptiveGlSurface::Update& update) {
		if(mode()!=Mode::Shader || box.size.x!=34.f || box.size.y!=34.f || numLeds!=25 || minAngle<=-M_PI || maxAngle>=M_PI || minAngle>=maxAngle || !APP || !APP->scene || !APP->window) {
			return false;
		}
		if(!gl_lifecycle::resourceContextMatches(lease,args.vg)) {
			release();surface.reset(false);lease=gl_lifecycle::acquireResourceContext(args.vg);
		}
		if(failed) return false;
		if(program&&!gl_lifecycle::isValidProgramBufferPair(program,vbo)) {program=vbo=0;surface.markDirty();}
		const float raw=clamp(settings::haloBrightness,0.f,1.5f);
		if(oldMin!=minAngle||oldMax!=maxAngle||oldValue!=valueNorm||oldBloom!=raw||oldCenter!=centerNorm||oldBipolar!=bipolar) {
			const float lo=std::min(bipolar?centerNorm:0.f,valueNorm),hi=std::max(bipolar?centerNorm:0.f,valueNorm);
			for(int i=0;i<25;++i) {
				float n=float(i)/24.f;
				if(oldMin!=minAngle||oldMax!=maxAngle) {float a=minAngle+n*(maxAngle-minAngle);leds[i*4]=12.75f*std::sin(a);leds[i*4+1]=-12.75f*std::cos(a);}
				leds[i*4+2]=bipolar ? (n>=lo&&n<=hi&&std::fabs(valueNorm-centerNorm)>.005f) : (valueNorm>0.f&&n<=valueNorm);
			}
			float ramp=clamp((raw-.5f)/.5f,0.f,1.f);bloom=(raw+2.f*raw*(1.f-raw))*(1.f+1.4f*ramp*ramp);
			oldMin=minAngle;oldMax=maxAngle;oldValue=valueNorm;oldBloom=raw;oldCenter=centerNorm;oldBipolar=bipolar;surface.markDirty();
		}
		float transform[6];nvgCurrentTransform(args.vg,transform);
		float zoom=std::max(std::hypot(transform[0],transform[1]),std::hypot(transform[2],transform[3]));
		visual_assets::AdaptiveGlSurfacePolicy policy;policy.vertexAttributeCount=1;policy.shaderOnlyState=true;policy.maxDensity=4.f;policy.retainPeakCapacity=true;
		// Pure GL callback only: the guard restores the active outer FBO and NanoVG
		// state. Preparation stays inside Draw, including module-level telemetry.
		update.surface=&surface;update.logicalSize=box.size;update.zoom=zoom;update.pixelRatio=APP->window->pixelRatio;
		update.policy=policy;update.validate=true;update.user=this;
		update.callback=[](void* self,Vec active,int y){static_cast<Ring*>(self)->render(active,y);};
		return true;
	}
	void draw(const DrawArgs& args) override {
		visual_assets::AdaptiveGlSurface::Update update;
		if(!prepareUpdate(args,update)) {ProgressLedRingWidget::draw(args);return;}
		visual_assets::AdaptiveGlSurface::renderBatch(args.vg,&update,1);
		if(!surface.isDirty()&&!failed&&surface.draw(args,box.size)) return;
		ProgressLedRingWidget::draw(args);
	}
};
}
