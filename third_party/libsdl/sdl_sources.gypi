# Copyright 2015 Google Inc.
#
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
# This file includes all of the general files for building sdl
# The parent gyp/gypi file must define
#       'src_dir'     e.g. ../third_party/libsdl/src
#
{
  'variables': {
    'sdl_sources': [
      '<(src_dir)/src/SDL_assert.c',
      '<(src_dir)/src/SDL_error.c',
      '<(src_dir)/src/SDL_hints.c',
      '<(src_dir)/src/SDL_log.c',
      '<(src_dir)/src/atomic/SDL_atomic.c',
      '<(src_dir)/src/atomic/SDL_spinlock.c',
      '<(src_dir)/src/audio/SDL_audio.c',
      '<(src_dir)/src/audio/SDL_audiocvt.c',
      '<(src_dir)/src/audio/SDL_audiodev.c',
      '<(src_dir)/src/audio/SDL_audiotypecvt.c',
      '<(src_dir)/src/audio/SDL_mixer.c',
      '<(src_dir)/src/audio/SDL_wave.c',
      '<(src_dir)/src/cpuinfo/SDL_cpuinfo.c',
      '<(src_dir)/src/dynapi/SDL_dynapi.c',
      '<(src_dir)/src/events/SDL_clipboardevents.c',
      '<(src_dir)/src/events/SDL_dropevents.c',
      '<(src_dir)/src/events/SDL_events.c',
      '<(src_dir)/src/events/SDL_gesture.c',
      '<(src_dir)/src/events/SDL_keyboard.c',
      '<(src_dir)/src/events/SDL_mouse.c',
      '<(src_dir)/src/events/SDL_quit.c',
      '<(src_dir)/src/events/SDL_touch.c',
      '<(src_dir)/src/events/SDL_windowevents.c',
      '<(src_dir)/src/file/SDL_rwops.c',
      '<(src_dir)/src/haptic/SDL_haptic.c',
      '<(src_dir)/src/joystick/SDL_gamecontroller.c',
      '<(src_dir)/src/joystick/SDL_joystick.c',
      '<(src_dir)/src/power/SDL_power.c',
      '<(src_dir)/src/render/SDL_d3dmath.c',
      '<(src_dir)/src/render/SDL_render.c',
      '<(src_dir)/src/render/SDL_yuv_mmx.c',
      '<(src_dir)/src/render/SDL_yuv_sw.c',
      '<(src_dir)/src/render/direct3d/SDL_render_d3d.c',
      '<(src_dir)/src/render/direct3d11/SDL_render_d3d11.c',
      '<(src_dir)/src/render/opengl/SDL_render_gl.c',
      '<(src_dir)/src/render/opengl/SDL_shaders_gl.c',
      '<(src_dir)/src/render/opengles/SDL_render_gles.c',
      '<(src_dir)/src/render/opengles2/SDL_render_gles2.c',
      '<(src_dir)/src/render/opengles2/SDL_shaders_gles2.c',
      '<(src_dir)/src/render/psp/SDL_render_psp.c',
      '<(src_dir)/src/render/software/SDL_blendfillrect.c',
      '<(src_dir)/src/render/software/SDL_blendline.c',
      '<(src_dir)/src/render/software/SDL_blendpoint.c',
      '<(src_dir)/src/render/software/SDL_drawline.c',
      '<(src_dir)/src/render/software/SDL_drawpoint.c',
      '<(src_dir)/src/render/software/SDL_render_sw.c',
      '<(src_dir)/src/render/software/SDL_rotate.c',
      '<(src_dir)/src/stdlib/SDL_getenv.c',
      '<(src_dir)/src/stdlib/SDL_iconv.c',
      '<(src_dir)/src/stdlib/SDL_malloc.c',
      '<(src_dir)/src/stdlib/SDL_qsort.c',
      '<(src_dir)/src/stdlib/SDL_stdlib.c',
      '<(src_dir)/src/stdlib/SDL_string.c',
      '<(src_dir)/src/thread/SDL_thread.c',
      '<(src_dir)/src/timer/SDL_timer.c',
      '<(src_dir)/src/video/SDL_RLEaccel.c',
      '<(src_dir)/src/video/SDL_blit.c',
      '<(src_dir)/src/video/SDL_blit_0.c',
      '<(src_dir)/src/video/SDL_blit_1.c',
      '<(src_dir)/src/video/SDL_blit_A.c',
      '<(src_dir)/src/video/SDL_blit_N.c',
      '<(src_dir)/src/video/SDL_blit_auto.c',
      '<(src_dir)/src/video/SDL_blit_copy.c',
      '<(src_dir)/src/video/SDL_blit_slow.c',
      '<(src_dir)/src/video/SDL_bmp.c',
      '<(src_dir)/src/video/SDL_clipboard.c',
      '<(src_dir)/src/video/SDL_egl.c',
      '<(src_dir)/src/video/SDL_fillrect.c',
      '<(src_dir)/src/video/SDL_pixels.c',
      '<(src_dir)/src/video/SDL_rect.c',
      '<(src_dir)/src/video/SDL_shape.c',
      '<(src_dir)/src/video/SDL_stretch.c',
      '<(src_dir)/src/video/SDL_surface.c',
      '<(src_dir)/src/video/SDL_video.c',
      '<(src_dir)/src/video/dummy/SDL_nullevents.c',
      '<(src_dir)/src/video/dummy/SDL_nullframebuffer.c',
      '<(src_dir)/src/video/dummy/SDL_nullvideo.c',
      '<(src_dir)/src/SDL.c',
    ],
  },
}
