// Copyright 2026 Team Deepiri. Apache-2.0.
// Minimal OpenXR loader smoke test (no UE). Validates Linux dev environment.

#include <openxr/openxr.h>

#include <cstdio>
#include <cstring>
#include <vector>

static const char* ResultString(XrResult r)
{
	switch (r)
	{
		case XR_SUCCESS: return "XR_SUCCESS";
		case XR_ERROR_RUNTIME_FAILURE: return "XR_ERROR_RUNTIME_FAILURE";
		case XR_ERROR_RUNTIME_UNAVAILABLE: return "XR_ERROR_RUNTIME_UNAVAILABLE";
		default: return "XR_RESULT_UNKNOWN";
	}
}

int main()
{
	XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
	XrApplicationInfo appInfo{};
	std::strncpy(appInfo.applicationName, "MistspireXRSandbox", XR_MAX_APPLICATION_NAME_SIZE - 1);
	appInfo.applicationVersion = 1;
	std::strncpy(appInfo.engineName, "MistspireNative", XR_MAX_ENGINE_NAME_SIZE - 1);
	appInfo.engineVersion = 1;
	appInfo.apiVersion = XR_CURRENT_API_VERSION;
	createInfo.applicationInfo = appInfo;

	std::vector<const char*> extensions;
#if defined(XR_USE_GRAPHICS_API_VULKAN)
	extensions.push_back(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.enabledExtensionNames = extensions.data();
#endif

	XrInstance instance = XR_NULL_HANDLE;
	const XrResult result = xrCreateInstance(&createInfo, &instance);
	if (result != XR_SUCCESS)
	{
		std::fprintf(stderr, "xrCreateInstance failed: %s\n", ResultString(result));
		std::fprintf(stderr, "Set XR_RUNTIME_JSON or install Monado/SteamVR OpenXR.\n");
		return 1;
	}

	XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	XrSystemId systemId = XR_NULL_SYSTEM_ID;
	const XrResult sysResult = xrGetSystem(instance, &systemInfo, &systemId);
	if (sysResult != XR_SUCCESS)
	{
		std::fprintf(stderr, "xrGetSystem failed: %s (no HMD/runtime?)\n", ResultString(sysResult));
		xrDestroyInstance(instance);
		return 2;
	}

	std::printf("Mistspire xr-sandbox OK — OpenXR instance + HMD system id=%llu\n",
		static_cast<unsigned long long>(systemId));

	xrDestroyInstance(instance);
	return 0;
}
