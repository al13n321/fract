#pragma once

#include "gl-util/glfw-util.h"
#include "OVR.h"
#include "OVR_CAPI_GL.h"
#include "util/vec.h"
#include "gl-util/texture2d.h"

namespace fract { namespace ovr {

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

  ovrHmd get() {
    return hmd_;
  }

  ivec2 GetTextureSize(ovrEyeType eye, float pixelsPerDisplayPixel) {
    ovrSizei res = ovrHmd_GetFovTextureSize(
      hmd_, eye, hmd_->DefaultEyeFov[eye], pixelsPerDisplayPixel);
    return ivec2(res.w, res.h);
  }

  ivec2 GetWindowPos() {
    return ivec2(hmd_->WindowsPos.x, hmd_->WindowsPos.y);
  }

  ivec2 GetResolution() {
    return ivec2(hmd_->Resolution.w, hmd_->Resolution.h);
  }

  void StartTracking() {
    if (!ovrHmd_ConfigureTracking(hmd_, ovrTrackingCap_Idle, 0))
      throw OVRException("failed to configure tracking");
  }

  void StopTracking() {
    if (!ovrHmd_ConfigureTracking(hmd_, 0, 0))
      throw OVRException("failed to configure tracking");
  }

  // Uses current window.
  void ConfigureRendering() {
    ovrGLConfig config;
    memset(&config, 0, sizeof(config));
    config.OGL.Header = {
      ovrRenderAPI_OpenGL, // API
      hmd_->Resolution,    // RTSize
      0                    // Multisample 
    };
    
    int caps =
      ovrDistortionCap_Chromatic |
      ovrDistortionCap_TimeWarp |
      ovrDistortionCap_Vignette |
      //ovrDistortionCap_FlipInput |
      //ovrDistortionCap_Overdrive |
      ovrDistortionCap_HqDistortion;
    if (!ovrHmd_ConfigureRendering(
        hmd_, &config.Config, caps, hmd_->DefaultEyeFov, eye_render_desc_))
      throw OVRException("failed to configure rendering");
  }

  // Array of length ovrEye_Count. Which means two, unless you're an octopus
  // ... oh, wait ... *opens wikipedia* ... no, two even if you are.
  const ovrEyeRenderDesc* GetEyeRenderDesc() {
    return eye_render_desc_;
  }

  void BeginFrame() {
    frame_timing_ = ovrHmd_BeginFrame(hmd_, 0);
  }

  const ovrFrameTiming& GetFrameTiming() {
    return frame_timing_;
  }

  void GetEyePoses(
      std::initializer_list<ovrPosef*> out_eye_poses,
      bool monoscopic = false) {
    assert(out_eye_poses.size() == 2);
    ovrVector3f offsets[2];
    auto a = eye_render_desc_[0].HmdToEyeViewOffset;
    auto b = eye_render_desc_[0].HmdToEyeViewOffset;
    if (monoscopic) {
      offsets[0] = offsets[1] = {(a.x+b.x)/2, (a.y+b.y)/2, (a.z+b.z)/2};
    } else {
      offsets[0] = a;
      offsets[1] = b;
    }
    ovrHmd_GetEyePoses(hmd_, 0, offsets, last_poses_, nullptr);

    int i = 0;
    for (auto out: out_eye_poses) {
      *out = last_poses_[i++];
    }
  }

  // Assumes you rendered eye textures using pose from
  // the last call to GetEyePoses().
  void EndFrame(std::initializer_list<GL::Texture2D*> eye_textures) {
    ovrTexture tex[2];
    int i = 0;
    for (auto texture: eye_textures) {
      ovrGLTexture t;
      t.OGL.Header.API = ovrRenderAPI_OpenGL;
      t.OGL.Header.TextureSize = t.OGL.Header.RenderViewport.Size =
        {texture->size().x, texture->size().y};
      t.OGL.Header.RenderViewport.Pos = {0, 0};
      t.OGL.TexId = texture->name();
      tex[i] = t.Texture;
      ++i;
    }
    ovrHmd_EndFrame(hmd_, last_poses_, tex);
  }

 private:
  ovrHmd hmd_;
  ovrEyeRenderDesc eye_render_desc_[2];
  ovrFrameTiming frame_timing_;
  ovrPosef last_poses_[2];
};



}}
