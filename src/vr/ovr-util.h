#pragma once

namespace fract { namespace OVR {

class Initializer {
 public:
  Initializer() {
    if (!ovr_Initialize())
      throw OVRException("couldn't init ovr");
  }

  ~Initializer() {
    ovr_Shutdown();
  }
};

class HMD {
 public:
  HMD() {
    hmd_ = ovrHmd_Create(0);
    if (!hmd_)
      throw OVRException("no HMD detected");
  }

  ~HMD() {
    ovrHmd_Destroy(hmd_);
  }

  ovrHMD get() {
    return hmd_;
  }

 private:
  ovrHMD hmd_;
};



}}
