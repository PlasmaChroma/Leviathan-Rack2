#pragma once
#include "plugin.hpp"
#include <chrono>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <vector>

using Clock = std::chrono::steady_clock;
static void require(bool ok, const char* why) {
 if (!ok) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}
static double us(Clock::time_point start) {
 return std::chrono::duration<double, std::micro>(Clock::now() - start).count();
}
static void report(const char* label, std::vector<double> values) {
 if (values.empty()) { std::printf("%s,unavailable\n", label); return; }
 std::sort(values.begin(), values.end());
 std::printf("%s,n=%zu,median_us=%.3f,p95_us=%.3f\n", label, values.size(), values[values.size()/2], values[size_t((values.size()-1)*.95)]);
}
struct Queries {
 struct Slot { GLuint id=0; bool pending=false; bool measured=false; };
 Slot slots[640];
 bool supported=GLEW_ARB_timer_query;
 std::vector<double> results;
 int skipped=0;
 Queries() { if(supported) for(auto& s:slots) glGenQueries(1,&s.id); }
 void collect() {
  if(!supported) return;
  for(auto& s:slots) if(s.pending) {
   GLint ready=0; glGetQueryObjectiv(s.id,GL_QUERY_RESULT_AVAILABLE,&ready);
   if(ready) {
    GLuint64 ns=0; glGetQueryObjectui64v(s.id,GL_QUERY_RESULT,&ns);
    if(s.measured) results.push_back(double(ns)*.001);
    s.pending=false;
   }
  }
 }
 Slot* begin(int frame) {
  if(!supported) return nullptr;
  collect(); auto& s=slots[frame%640];
  if(s.pending) { if(frame>=100) ++skipped; return nullptr; }
  s.measured=frame>=100; glBeginQuery(GL_TIME_ELAPSED,s.id); return &s;
 }
 void end(Slot* s) { if(s) { glEndQuery(GL_TIME_ELAPSED); s->pending=true; } }
 void finish() {
  glFlush(); auto start=Clock::now();
  while(us(start)<5000000) {
   collect(); bool pending=false; for(auto& s:slots) pending|=s.pending;
   if(!pending) break;
   std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  for(auto& s:slots) if(s.pending && s.measured) ++skipped;
 }
 ~Queries() { if(supported) for(auto& s:slots) glDeleteQueries(1,&s.id); }
};
