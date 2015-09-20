#pragma once

#include <iostream>
#include <algorithm>
#include "gl-util/glfw-util.h"
#include "OVR.h"
#include "OVR_CAPI_GL.h"
#include "util/vec.h"
#include "util/mat.h"
#include "util/quat.h"
#include "gl-util/texture2d.h"

namespace fract { namespace ovr {

// Mapping OVR vector, matrix and quaternion types to ours.

template<typename T>
struct TypeMapping {};

template<typename T>
inline typename TypeMapping<T>::Type conv(T x);

#define CONVERSION(From, To, code) \
  template<> \
  struct TypeMapping<From> { typedef To Type; }; \
  template<>\
  inline To conv(From a) code

CONVERSION(ovrVector2i, ivec2, { return ivec2(a.x, a.y); })
CONVERSION(ovrSizei, ivec2, { return ivec2(a.w, a.h); })
CONVERSION(ovrQuatf, fquat, { return fquat(a.w, a.x, a.y, a.z); })
CONVERSION(ovrVector3f, fvec3, { return fvec3(a.x, a.y, a.z); })
CONVERSION(ovrMatrix4f, fmat4, {
  return fmat4(
    a.M[0][0], a.M[0][1], a.M[0][2], a.M[0][3],
    a.M[1][0], a.M[1][1], a.M[1][2], a.M[1][3],
    a.M[2][0], a.M[2][1], a.M[2][2], a.M[2][3],
    a.M[3][0], a.M[3][1], a.M[3][2], a.M[3][3]);  
})

#undef CONVERSION


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
    ovrHmd_SetEnabledCaps(hmd_,
      ovrHmdCap_LowPersistence |
      ovrHmdCap_DynamicPrediction);
  }

  ~HMD() {
    ovrHmd_Destroy(hmd_);
  }

  ovrHmd get() {
    return hmd_;
  }

  // Unlike ovrHmd_GetFovTextureSize, this method can't be called before
  // ConfigureRendering(), since it depends on monoscopic flag.
  ivec2 GetTextureSize(ovrEyeType eye, float pixelsPerDisplayPixel) {
    return conv(ovrHmd_GetFovTextureSize(
      hmd_, eye, fov_[eye], pixelsPerDisplayPixel));
  }

  ivec2 GetWindowPos() {
    return conv(hmd_->WindowsPos);
  }

  ivec2 GetResolution() {
    return conv(hmd_->Resolution);
  }

  bool IsMonoscopic() const {
    return monoscopic_;
  }

  void StartTracking() {
    int flags =
      ovrTrackingCap_Orientation |
      ovrTrackingCap_MagYawCorrection |
      ovrTrackingCap_Position;
    if (!ovrHmd_ConfigureTracking(hmd_, flags, 0))
      throw OVRException("failed to configure tracking");
  }

  void StopTracking() {
    if (!ovrHmd_ConfigureTracking(hmd_, 0, 0))
      throw OVRException("failed to configure tracking");
  }

  void ConfigureRendering(glfw::Window *window, bool monoscopic) {
    ovrGLConfig config;
    memset(&config, 0, sizeof(config));
    config.OGL.Header = {
      ovrRenderAPI_OpenGL, // API
      hmd_->Resolution,    // RTSize
      8                    // Multisample 
    };
#ifdef WIN32
    config.OGL.Window = window->GetHWND();
#else
    static bool warned = false;
    if (!warned) {
      warned = true;
      std::cerr <<
        "ConfigureRendering implementation for your platform uses current "
        "window for rendering. This is likely to cause freezing or crashes if "
        "monoscopic is changed in config at runtime. Consider fixing this in "
        __FILE__ ":" << __LINE__ << ". It's ceratinly easily fixable on Linux."
        << std::endl;
    }
#endif
    
    int caps =
      ovrDistortionCap_Chromatic |
      ovrDistortionCap_TimeWarp |
      ovrDistortionCap_Vignette |
      //ovrDistortionCap_Overdrive |
      ovrDistortionCap_HqDistortion;
    fov_[0] = hmd_->DefaultEyeFov[0];
    fov_[1] = hmd_->DefaultEyeFov[1];
    monoscopic_ = monoscopic;
    if (monoscopic) {
      float hfov = std::max(std::max(std::max(
        fov_[0].LeftTan, fov_[0].RightTan), fov_[1].LeftTan), fov_[1].RightTan);
      float upfov = std::max(fov_[0].UpTan, fov_[1].UpTan);
      float downfov = std::max(fov_[0].DownTan, fov_[1].DownTan);
      fov_[0] = fov_[1] = {upfov, downfov, hfov, hfov};
    }
    if (!ovrHmd_ConfigureRendering(
        hmd_, &config.Config, caps, fov_, eye_render_desc_))
      throw OVRException("failed to configure rendering");
  }

  void FreeRendering() {
    if (!ovrHmd_ConfigureRendering(
        hmd_, nullptr, 0, fov_, eye_render_desc_))
      throw OVRException("failed to unconfigure rendering");
  }

  void GetEyeRenderDescs(std::initializer_list<ovrEyeRenderDesc*> out_descs) {
    assert(out_descs.size() == 2);
    int i = 0;
    for (auto out: out_descs) {
      *out = eye_render_desc_[i++];
    }
  }

  void BeginFrame() {
    frame_timing_ = ovrHmd_BeginFrame(hmd_, 0);
  }

  const ovrFrameTiming& GetFrameTiming() {
    return frame_timing_;
  }

  void GetEyePoses(std::initializer_list<ovrPosef*> out_eye_poses) {
    assert(out_eye_poses.size() == 2);
    ovrVector3f offsets[2];
    auto a = eye_render_desc_[0].HmdToEyeViewOffset;
    auto b = eye_render_desc_[1].HmdToEyeViewOffset;
    if (monoscopic_) {
      offsets[0] = offsets[1] = {(a.x+b.x)/2, (a.y+b.y)/2, (a.z+b.z)/2};
    } else {
      offsets[0] = a;
      offsets[1] = b;
    }
    ovrHmd_GetEyePoses(hmd_, 0, offsets, last_poses_, nullptr);

    int i = 0;
    for (auto out: out_eye_poses) {
      if (monoscopic_)
        last_poses_[i].Position = {0, 0, 0};
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
  ovrFovPort fov_[2];
  bool monoscopic_{false};
};

inline fmat4 ProjectionMatrix(
    ovrFovPort fov, float znear = 1e-3, float zfar = 100) {
  return conv(ovrMatrix4f_Projection(fov, znear, zfar, true));
}


}}
