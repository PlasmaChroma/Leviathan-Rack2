#include "recovered_kernels.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <iomanip>
int main(int argc,char**argv){
 try{
  using namespace arbhar_re;std::uint64_t checks=0;double maxProbeError=0;
  auto check=[&](bool ok,const char*why){++checks;if(!ok)throw std::runtime_error(why);};
  for(int i=0;i<=20000;++i){float x=(i-10000)*.001f;
   check(std::isfinite(rationalClip(x)),"clip finite");
   check(std::abs(rationalClip(x)+rationalClip(-x))<1e-6f,"clip odd symmetry");
  }
  check(std::abs(rationalClip(6.f)-14.f/13.f)<2e-7f,"clip does not saturate at unity");
  check(rationalClip(10.f)==rationalClip(6.f),"clipped input domain");
  check(rationalClip(std::numeric_limits<float>::quiet_NaN())==0.f,"recommended NaN safety");
  for(int i=0;i<=1000;++i){float t=i/1000.f;
   check(std::abs(cubic4(1,1,1,1,t)-1)<2e-6,"cubic DC");
   check(std::abs(cubic4(-1,0,1,2,t)-t)<2e-6,"cubic linear ramp");
   auto dw=equalPowerMix(t);check(std::abs(dw.first*dw.first+dw.second*dw.second-1)<3e-6,"mix power");
  }
  check(squaredControl(0)==0,"control zero");check(squaredControl(.5f)==25000,"control midpoint");check(squaredControl(1)==100000,"control full");
  check(durationSamplesFromControl(0)==128,"duration floor");check(durationSamplesFromControl(25000)==36000,"duration midpoint");check(durationSamplesFromControl(100000)==144000,"duration ceiling");
  const auto bank=makeWindowBank();
  for(int r=0;r<101;++r){
   check(bank[r][0]==0 && bank[r][514]==0,"window endpoints");
   for(float x:bank[r])check(std::isfinite(x)&&x>=0&&x<=1.000001f,"window finite bounded");
  }
  check(std::abs(bank[0][256]-.9f)<1e-6,"square endpoint gain");
  check(std::abs(bank[50][256]-1.f)<1e-6,"Gaussian centre");
  std::size_t probes=0;
  if(argc>1){
   std::ifstream in(argv[1]);if(!in)throw std::runtime_error("cannot open follow probe CSV");
   std::string line;std::getline(in,line);
   while(std::getline(in,line)){for(char &c:line)if(c==',')c=' ';std::istringstream ss(line);int mode,raw;double a,b;
    if(!(ss>>mode>>raw>>a>>b))throw std::runtime_error("malformed probe row");
    double err=std::abs(followSpeed(float(raw),mode)-a);maxProbeError=std::max(maxProbeError,err);check(err<1e-5,"follow instruction-probe agreement");++probes;
   }
   check(probes==12288,"all follow probes loaded");
  }
  if(argc>2){std::ofstream out(argv[2],std::ios::binary);if(!out)throw std::runtime_error("cannot write bank");out.write(reinterpret_cast<const char*>(bank.data()),sizeof(bank));}
  std::cout<<std::setprecision(12)<<"{\"checks\":"<<checks<<",\"failures\":0,\"follow_probe_rows\":"<<probes<<",\"max_follow_probe_error\":"<<maxProbeError<<",\"window_bank_bytes\":"<<sizeof(bank)<<",\"scope\":\"native invariants plus one restricted instruction-text probe fixture; no full firmware or hardware run\"}\n";
 }catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}
}
