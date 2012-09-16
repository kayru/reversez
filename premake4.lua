if not _ACTION then _ACTION="vs2010" end
	
solution "ReverseZ"

	language "C++"

	location ( "build/" .. _ACTION )
	flags {"NoManifest", "ExtraWarnings", "StaticRuntime", "NoMinimalRebuild", "FloatFast", "EnableSSE2" }
	optimization_flags = { "OptimizeSpeed" }
	targetdir("bin")

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"_CRT_SECURE_NO_DEPRECATE",
		"_STATIC_CPPLIB",
		"RUSH_USING_NAMESPACE",
	}

	local config_list = {
		"DX9-Release",
		"DX9-Debug",
		"DX11-Release",
		"DX11-Debug",
	}
	local platform_list = {
		"x32", "x64"
	}

	configurations(config_list)
	platforms(platform_list)

	-- Common include directories
	includedirs
	{
		"librush",
		"$(DXSDK_DIR)/Include",
	}
	
-- CONFIGURATIONS

configuration "*-Release"
	defines { "NDEBUG" }
	flags { "Symbols", optimization_flags }

configuration "*-Debug"
	defines { "_DEBUG" }
	flags { "Symbols" }

configuration "DX9-*"
	defines {
		"RUSH_DX9"
	}
	links { 
		"d3d9", 
		"d3dx9", 
		"dxerr" 
	}

configuration "DX11-*"
	defines {
		"RUSH_DX11"
	}
	links { 
		"d3d11", 
		"d3dx11",
		"dxerr",
		"dxgi",
		"dxguid",
		"d3dcompiler"
	}

configuration "x32"
	libdirs { "$(DXSDK_DIR)/Lib/x86" }

configuration "x64"
	libdirs { "$(DXSDK_DIR)/Lib/x64" }

configuration "Windows"
	defines { "RUSH_PLATFORM_WINDOWS" }
	buildoptions { "/MP /wd4100 /wd4505 /wd4995"  }

--  give each configuration/platform a unique output directory

for _, config in ipairs(config_list) do
	for _, plat in ipairs(platform_list) do
		configuration { config, plat }
		objdir    ( "build/" .. _ACTION .. "/tmp/"  .. config  .. "-" .. plat )
	end
end

-- SUBPROJECTS

project "ReverseZ"
	flags {"NoPCH"}
	kind "WindowedApp"
	files {
		"source/**.*", 
		"librush/Rush/**.h",
		"librush/Rush/**.cpp",
	}
