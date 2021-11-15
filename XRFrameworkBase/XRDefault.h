#pragma once

#if defined(_WIN32) || defined(_WIN64)

#define XRExport __declspec(dllexport)
#define XRImport extern __declspec(dllimport)

#ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseExport __declspec(dllexport)
#define XRBaseSupply __declspec(dllexport)
#else
#define XRBaseExport __declspec(dllimport)
#define XRBaseSupply __declspec(dllimport)
#endif

#ifdef XRRENDERENGINEGL_EXPORTS
#define XRRenderExport extern "C" __declspec(dllexport)
#define XRRenderAPI(ReturnType, ApiName) extern "C" __declspec(dllexport) ReturnType ApiName
#else
#define XRRenderExport extern "C" __declspec(dllimport)
#define XRRenderAPI(ReturnType, ApiName) extern ReturnType (*ApiName)
#endif

#ifdef XRSOURCEBUILDSYSTEMGLSL_EXPORTS
#define XRShaderBuildExport extern "C" __declspec(dllexport)
#define XRShaderBuildAPI(ReturnType, ApiName) extern "C" __declspec(dllexport) ReturnType ApiName
#else
#define XRShaderBuildExport extern "C" __declspec(dllimport)
#define XRShaderBuildAPI(ReturnType, ApiName) extern __declspec(dllimport) ReturnType (*ApiName)
#endif

#elif defined(__APPLE__)

#define XRExport __attribute__((visibility("default")))
#define XRImport extern __attribute__((visibility("default")))

#define XRBaseExport __attribute__((visibility("default")))
#define XRBaseHidden __attribute__((visibility("hidden")))

#ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseSupply __attribute__((visibility("default")))
#else // #ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseSupply extern __attribute__((visibility("default")))
#endif // #ifdef XRFRAMEWORKBASE_EXPORTS

#define XRRenderExport extern "C" __attribute__((visibility("default")))

#else
#error "Not considered yet"
#endif

constexpr int const PAGE_SIZE = 4'096;


#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <thread>
#include <functional>

#include <string>
#include <memory>

#include <assert.h>
#include <errno.h>