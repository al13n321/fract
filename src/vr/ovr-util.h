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

  // Array of ovrEye_Count (=2) elements.
  const ovrEyeType* GetEyeRenderOrder() {
    return hmd_->EyeRenderOrder;
  }

  void ConfigureTracking() {
    if (!ovrHmd_ConfigureTracking(hmd_, ovrTrackingCap_Idle, 0))
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

  // Array of ovrEye_Count (=2) .
  const ovrEyeRenderDesc* GetEyeRenderDesc() {
    return eye_render_desc_;
  }

  void BeginFrame() {
    frame_timing_ = ovrHmd_BeginFrame(hmd_, 0);
  }

  const ovrFrameTiming& GetFrameTiming() {
    return frame_timing_;
  }

  void GetEyePoses(ovrPosef out_eye_poses[2], bool monoscopic = false) {
    ovrVector3f offsets[2];
    auto a = eye_render_desc_[0].HmdToEyeViewOffset;
    auto b = eye_render_desc_[0].HmdToEyeViewOffset;
    if (monoscopic) {
      offsets[0] = offsets[1] = {(a.x+b.x)/2, (a.y+b.y)/2, (a.z+b.z)/2};
    } else {
      offsets[0] = a;
      offsets[1] = b;
    }
    ovrHmd_GetEyePoses(hmd_, 0, offsets, out_eye_poses, nullptr);
  }

  void EndFrame(ovrPosef render_pose[2], GL::Texture2D eye_texture[2]) {
    ovrTexture tex[2];
    for (int i = 0; i < 2; ++i) {
      ovrGLTexture t;
      t.OGL.Header.API = ovrRenderAPI_OpenGL;
      t.OGL.Header.TextureSize = t.OGL.Header.RenderViewport.Size =
        {eye_texture[i].size().x, eye_texture[i].size().y};
      t.OGL.Header.RenderViewport.Pos = {0, 0};
      t.OGL.TexId = eye_texture[i].name();
      tex[i] = t.Texture;
    }
    ovrHmd_EndFrame(hmd_, render_pose, tex);
  }

 private:
  ovrHmd hmd_;
  ovrEyeRenderDesc eye_render_desc_[2];
  ovrFrameTiming frame_timing_;
};



}}
