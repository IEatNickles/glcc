#ifndef GLCC_H
#define GLCC_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef void* glcc_Lib;
typedef uint32_t glcc_bool;
#define GLCC_TRUE 1
#define GLCC_FALSE 0

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WIN32)
    #define GLCC_WIN32
#elif defined(__linux__)
    #define GLCC_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define GLCC_DARWIN
#elif defined(__EMSCRIPTEN__)
    #define GLCC_JS
#else
    #error "No supported platform (OS) detected."
#endif

typedef void* EGLDisplay;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLConfig;
typedef void* EGLNativeWindowType;

#define EGL_PLATFORM_XCB_EXT              0x31DC
#define EGL_PLATFORM_X11_EXT              0x31D5
#define EGL_PLATFORM_WAYLAND_EXT          0x31D8

#ifdef GLCC_LINUX
  #ifndef GLCC_BUILD_X11
    #define GLCC_BUILD_X11 1
  #endif // GLCC_BUILD_X11
  #ifndef GLCC_BUILD_WAYLAND
    #define GLCC_BUILD_WAYLAND 0
  #else
    #error "No wayland support yet, use xwayland"
  #endif // GLCC_BUILD_WAYLAND
#endif // GLCC_LINUX

typedef enum {
  GLCC_PLATFORM_WIN32,
  GLCC_PLATFORM_X11,
  GLCC_PLATFORM_WAYLAND,
  GLCC_PLATFORM_COCOA,
} glcc_Platform;

typedef struct {
  struct {
    EGLContext context;
    EGLSurface surface;
  } egl;
} _glcc_Context;
typedef void* glcc_Context;

glcc_bool glcc_init(glcc_Platform platform, void *display);
glcc_Context glcc_create_context(void *window);
void glcc_destroy_context(glcc_Context context);
void glcc_make_current(glcc_Context context);
void glcc_swap_buffers(glcc_Context context);
void *glcc_get_proc_address(const char *name);

glcc_Lib _glcc_load_library(const char *name);
glcc_bool _glcc_unload_library(glcc_Lib lib);
void *_glcc_get_symbol(glcc_Lib lib, const char *name);

glcc_bool _glcc_init_egl();

glcc_bool _glcc_init_win32(void *display);
glcc_Context _glcc_create_context_win32(void *window);

glcc_bool _glcc_init_x11(void *display);
glcc_Context _glcc_create_context_x11(void *window);

glcc_bool _glcc_init_wayland(void *display);
glcc_Context _glcc_create_context_wayland(void *window);

glcc_bool _glcc_init_cocoa(void *display);
glcc_Context _glcc_create_context_cocoa(void *window);

typedef unsigned int EGLBoolean;

typedef void(*GLCC_FNPTR)(void);
typedef glcc_Context(*GLCC_CREATECONTEXTPROC)(void * /* window */);
typedef glcc_Context(*GLCC_DESTROYCONTEXTPROC)(glcc_Context);
typedef void(*GLCC_MAKECURRENTPROC)(glcc_Context);
typedef void(*GLCC_SWAPBUFFERSPROC)(glcc_Context);
typedef GLCC_FNPTR(*GLCC_GETPROCADDRESSPROC)(const char *);

typedef EGLDisplay(*PFN_eglGetPlatformDisplay)(uint32_t, void*, const intptr_t*);
typedef EGLContext(*PFN_eglCreateContext)(EGLDisplay, EGLConfig, EGLContext, const int32_t*);
typedef EGLSurface(*PFN_eglCreateWindowSurface)(EGLDisplay, EGLConfig, EGLSurface, const int32_t*);
typedef EGLSurface(*PFN_eglMakeCurrent)(EGLDisplay, EGLSurface, EGLSurface, EGLContext);
typedef EGLSurface(*PFN_eglSwapBuffers)(EGLDisplay, EGLSurface);
typedef GLCC_FNPTR(*PFN_eglGetProcAddress)(const char *);
typedef EGLBoolean(*PFN_eglInitialize)(EGLDisplay, int32_t*, int32_t*);
typedef EGLBoolean(*PFN_eglBindAPI)(unsigned int);
typedef EGLBoolean(*PFN_eglChooseConfig)(EGLDisplay, const int32_t*, EGLConfig*, int32_t, int32_t*);
typedef int32_t(*PFN_eglGetError)();

struct {
  glcc_Platform platform;
  struct {
    glcc_Lib handle;
    EGLDisplay display;
    PFN_eglGetPlatformDisplay GetPlatformDisplay;
    PFN_eglCreateContext CreateContext;
    PFN_eglCreateWindowSurface CreateWindowSurface;
    PFN_eglMakeCurrent MakeCurrent;
    PFN_eglSwapBuffers SwapBuffers;
    PFN_eglGetProcAddress GetProcAddress;
    PFN_eglInitialize Initialize;
    PFN_eglBindAPI BindAPI;
    PFN_eglChooseConfig ChooseConfig;
    PFN_eglGetError GetError;
  } egl;

  GLCC_CREATECONTEXTPROC create_context;
  GLCC_DESTROYCONTEXTPROC destroy_context;
  GLCC_MAKECURRENTPROC make_current;
  GLCC_SWAPBUFFERSPROC swap_buffers;
  GLCC_GETPROCADDRESSPROC get_proc_address;
} _glcc;

#define eglGetPlatformDisplay _glcc.egl.GetPlatformDisplay
#define eglCreateContext _glcc.egl.CreateContext
#define eglCreateWindowSurface _glcc.egl.CreateWindowSurface
#define eglMakeCurrent _glcc.egl.MakeCurrent
#define eglSwapBuffers _glcc.egl.SwapBuffers
#define eglGetProcAddress _glcc.egl.GetProcAddress
#define eglInitialize _glcc.egl.Initialize
#define eglBindAPI _glcc.egl.BindAPI
#define eglChooseConfig _glcc.egl.ChooseConfig
#define eglGetError _glcc.egl.GetError

#ifdef GLCC_IMPLEMENTATION
glcc_bool glcc_init(glcc_Platform platform, void *display) {
  _glcc.platform = platform;
  switch (platform) {
#ifdef GLCC_WIN32
  case GLCC_PLATFORM_WIN32:   return _glcc_init_win32(display);
#elifdef GLCC_LINUX
#if GLCC_BUILD_X11
  case GLCC_PLATFORM_X11:     return _glcc_init_x11(display);
#endif
#if GLCC_BUILD_WAYLAND
  case GLCC_PLATFORM_WAYLAND: return _glcc_init_wayland(display);
#endif
#elifdef GLCC_DARWIN
  case GLCC_PLATFORM_COCOA:   return _glcc_init_cocoa(display);
#endif
  }
  return GLCC_FALSE;
}

glcc_Context glcc_create_context(void *window) {
  return _glcc.create_context(window);
}

void *glcc_get_proc_address(const char *name) {
  return _glcc.get_proc_address(name);
}

void glcc_make_current(glcc_Context ctx) {
  _glcc.make_current(ctx);
}

void glcc_swap_buffers(glcc_Context ctx) {
  _glcc.swap_buffers(ctx);
}

#ifdef GLCC_WIN32
glcc_bool _glcc_init_win32(void *display) {
  _glcc.create_context = _glcc_create_context_win32;
  return GLCC_FALSE;
}

glcc_Context _glcc_create_context_win32(void *window) {
  return 0;
}
#endif // GLCC_WIN32

#ifdef GLCC_LINUX
#include <dlfcn.h>

glcc_Lib _glcc_load_library(const char *name) {
  return dlopen(name, RTLD_LAZY);
}

glcc_bool _glcc_unload_library(glcc_Lib lib) {
  return dlclose(lib);
}

void *_glcc_get_symbol(glcc_Lib lib, const char *name) {
  return dlsym(lib, name);
}

#if GLCC_BUILD_X11
void _glcc_make_current_x11(glcc_Context ctx) {
  _glcc_Context *c = (_glcc_Context*)ctx;
  eglMakeCurrent(_glcc.egl.display, c->egl.surface, c->egl.surface, c->egl.context);
}

void _glcc_swap_buffers_x11(glcc_Context ctx) {
  _glcc_Context *c = (_glcc_Context*)ctx;
  eglSwapBuffers(_glcc.egl.display, c->egl.surface);
}

glcc_bool _glcc_init_x11(void *display) {
  if (!_glcc_init_egl()) return GLCC_FALSE;

  _glcc.create_context = _glcc_create_context_x11;
  _glcc.make_current = _glcc_make_current_x11;
  _glcc.swap_buffers = _glcc_swap_buffers_x11;
  _glcc.get_proc_address = eglGetProcAddress;

  _glcc.egl.display = eglGetPlatformDisplay(EGL_PLATFORM_X11_EXT, display, NULL);
  if (_glcc.egl.display == NULL) {
    printf("EGL error: failed to get display 0x%x\n", eglGetError());
    return GLCC_FALSE;
  }
  int major, minor;
  if (!eglInitialize(_glcc.egl.display, &major, &minor)) {
    printf("EGL error: failed to initizliaze 0x%x\n", eglGetError());
    return GLCC_FALSE;
  }
  if (major * 10 + minor < 15) {
    printf("EGL version 1.5+ is required, supplied version: {}.{}", major, minor);
    return GLCC_FALSE;
  }
  return GLCC_TRUE;
}

#define EGL_CONTEXT_CLIENT_VERSION        0x3098
#define EGL_CONTEXT_MAJOR_VERSION         0x3098
#define EGL_CONTEXT_MINOR_VERSION         0x30FB
#define EGL_CONTEXT_OPENGL_PROFILE_MASK   0x30FD

#define EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT 0x00000001
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT 0x00000002

#define EGL_SURFACE_TYPE                  0x3033
#define EGL_WINDOW_BIT                    0x0004
#define EGL_RENDERABLE_TYPE               0x3040
#define EGL_OPENGL_BIT                    0x0008
#define EGL_RED_SIZE                      0x3024
#define EGL_GREEN_SIZE                    0x3023
#define EGL_BLUE_SIZE                     0x3022
#define EGL_ALPHA_SIZE                    0x3021
#define EGL_DEPTH_SIZE                    0x3025
#define EGL_STENCIL_SIZE                  0x3026
#define EGL_COLOR_BUFFER_TYPE             0x303F
#define EGL_RGB_BUFFER                    0x308E
#define EGL_CONFORMANT                    0x3042

#define EGL_RENDER_BUFFER                 0x3086
#define EGL_BACK_BUFFER                   0x3084

#define EGL_OPENGL_API                    0x30A2

#define EGL_BAD_ACCESS                    0x3002
#define EGL_BAD_ALLOC                     0x3003
#define EGL_BAD_ATTRIBUTE                 0x3004
#define EGL_BAD_CONFIG                    0x3005
#define EGL_BAD_CONTEXT                   0x3006
#define EGL_BAD_CURRENT_SURFACE           0x3007
#define EGL_BAD_DISPLAY                   0x3008
#define EGL_BAD_MATCH                     0x3009
#define EGL_BAD_NATIVE_PIXMAP             0x300A
#define EGL_BAD_NATIVE_WINDOW             0x300B
#define EGL_BAD_PARAMETER                 0x300C
#define EGL_BAD_SURFACE                   0x300D

const char *egl_error_string(int err) {
  switch (err) {
  case EGL_BAD_ACCESS: return "EGL_BAD_ACCESS";
  case EGL_BAD_ALLOC: return "EGL_BAD_ALLOC";
  case EGL_BAD_ATTRIBUTE: return "EGL_BAD_ATTRIBUTE";
  case EGL_BAD_CONFIG: return "EGL_BAD_CONFIG";
  case EGL_BAD_CONTEXT: return "EGL_BAD_CONTEXT";
  case EGL_BAD_CURRENT_SURFACE: return "EGL_BAD_CURRENT_SURFACE";
  case EGL_BAD_DISPLAY: return "EGL_BAD_DISPLAY";
  case EGL_BAD_MATCH: return "EGL_BAD_MATCH";
  case EGL_BAD_NATIVE_PIXMAP: return "EGL_BAD_NATIVE_PIXMAP";
  case EGL_BAD_NATIVE_WINDOW: return "EGL_BAD_NATIVE_WINDOW";
  case EGL_BAD_PARAMETER: return "EGL_BAD_PARAMETER";
  case EGL_BAD_SURFACE: return "EGL_BAD_SURFACE";
  }
  return "UNDEFINED_ERROR";
}

#define EGL_NONE 0x3038

glcc_Context _glcc_create_context_x11(void *window) {
  if (_glcc.egl.display == 0) {
    printf("EGL error: EGL has not been initialized\n");
    return NULL;
  }

  _glcc_Context *ctx = calloc(1, sizeof(_glcc_Context));
  if (!eglBindAPI(EGL_OPENGL_API)) {
    printf("EGL error: failed to bind OpenGL API %s\n", egl_error_string(eglGetError()));
    return NULL;
  }

  int attribs[] = {
    EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
    EGL_CONTEXT_MAJOR_VERSION, 4,
    EGL_CONTEXT_MINOR_VERSION, 3,
    EGL_NONE,
  };
  ctx->egl.context = eglCreateContext(_glcc.egl.display, NULL, NULL, attribs);
  if (ctx->egl.context == NULL) {
    printf("EGL error: failed to create contex %s\n", egl_error_string(eglGetError()));
    return NULL;
  }

  int config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_CONFORMANT, EGL_OPENGL_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    EGL_NONE,
  };

  EGLConfig configs[64];
  int num_configs;
  if (!eglChooseConfig(_glcc.egl.display, config_attribs, configs, 64, &num_configs) || num_configs < 1) {
    printf("EGL error: no configs available %s\n", egl_error_string(eglGetError()));
    return NULL;
  }

  int surface_attribs[] = {
    EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
    EGL_NONE,
  };
  for (int i = 0; i < num_configs; ++i) {
    ctx->egl.surface = eglCreateWindowSurface(_glcc.egl.display, configs[i], (EGLNativeWindowType)window, surface_attribs);
    if (ctx->egl.surface != NULL) break;
  }
  if (ctx->egl.surface == NULL) {
    printf("EGL error: failed to create surface %s\n", egl_error_string(eglGetError()));
    return NULL;
  }

  return (glcc_Context)ctx;
}
#endif // GLCC_BUILD_X11

#if GLCC_BUILD_WAYLAND
glcc_bool _glcc_init_wayland(void *display) {
  _glcc.create_context = _glcc_create_context_wayland;
  return GLCC_FALSE;
}

glcc_Context _glcc_create_context_wayland(void *window) {
  return 0;
}
#endif // GLCC_BUILD_WAYLAND
#endif // GLCC_LINUX

#ifdef GLCC_DARWIN
glcc_bool _glcc_init_cocoa(void *display) {
  _glcc.create_context = _glcc_create_context_cocoa;
  return GLCC_FALSE;
}

glcc_Context _glcc_create_context_cocoa(void *window) {
  return 0;
}
#endif // GLCC_DARWIN

glcc_bool _glcc_init_egl() {
  if (_glcc.egl.handle) return GLCC_TRUE;

  // stolen from GLFW
  const char *so_names[] = {
#if defined(_WIN32)
    "libEGL.dll",
    "EGL.dll",
#elif defined(__APPLE__)
    "libEGL.dylib",
#elif defined(__CYGWIN__)
    "libEGL-1.so",
#elif defined(__OpenBSD__) || defined(__NetBSD__)
    "libEGL.so",
#else
    "libEGL.so.1",
#endif
    NULL
  };

  for (int i = 0; so_names[i]; ++i) {
    _glcc.egl.handle = _glcc_load_library(so_names[i]);
  }

  if (!_glcc.egl.handle) return GLCC_FALSE;

  _glcc.egl.GetPlatformDisplay = _glcc_get_symbol(_glcc.egl.handle, "eglGetPlatformDisplay");
  if (!_glcc.egl.GetPlatformDisplay) return GLCC_FALSE;
  _glcc.egl.CreateContext = _glcc_get_symbol(_glcc.egl.handle, "eglCreateContext");
  if (!_glcc.egl.CreateContext) return GLCC_FALSE;
  _glcc.egl.CreateWindowSurface = _glcc_get_symbol(_glcc.egl.handle, "eglCreateWindowSurface");
  if (!_glcc.egl.CreateWindowSurface) return GLCC_FALSE;
  _glcc.egl.MakeCurrent = _glcc_get_symbol(_glcc.egl.handle, "eglMakeCurrent");
  if (!_glcc.egl.MakeCurrent) return GLCC_FALSE;
  _glcc.egl.SwapBuffers = _glcc_get_symbol(_glcc.egl.handle, "eglSwapBuffers");
  if (!_glcc.egl.SwapBuffers) return GLCC_FALSE;
  _glcc.egl.GetProcAddress = _glcc_get_symbol(_glcc.egl.handle, "eglGetProcAddress");
  if (!_glcc.egl.GetProcAddress) return GLCC_FALSE;
  _glcc.egl.Initialize = _glcc_get_symbol(_glcc.egl.handle, "eglInitialize");
  if (!_glcc.egl.Initialize) return GLCC_FALSE;
  _glcc.egl.BindAPI = _glcc_get_symbol(_glcc.egl.handle, "eglBindAPI");
  if (!_glcc.egl.BindAPI) return GLCC_FALSE;
  _glcc.egl.ChooseConfig = _glcc_get_symbol(_glcc.egl.handle, "eglChooseConfig");
  if (!_glcc.egl.ChooseConfig) return GLCC_FALSE;
  _glcc.egl.GetError = _glcc_get_symbol(_glcc.egl.handle, "eglGetError");
  if (!_glcc.egl.GetError) return GLCC_FALSE;

  return GLCC_TRUE;
}
#endif // GLCC_IMPLEMENTATION

#if defined(__cplusplus)
}
#endif

#endif // GLCC_H
