workspace "Arithmetic Coder"
    architecture "x86_64"

    configurations { 
        "Debug", 
        "Release"
    }

    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    objdir "obj/%{cfg.buildcfg}/%{prj.name}"

    startproject "AC_CLI"

    filter "system:Windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "RELEASE" }
        optimize "On"

    -- AC_Core project cotains logic and algorithms for arithmetic coding.
    -- It also manages filesystem for input and output data.
    project "AC_Core"
        location "AC_Core"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"
        targetname "ac_core"

        files {
            "%{prj.name}/src/**.hpp",
            "%{prj.name}/src/**.cpp"
        }

    -- Simple Command-Line Interface for AC_Core. 
    -- There should be no critical logic here.
    project "AC_CLI"
        location "AC_CLI"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        targetname "ac"

        files {
            "%{prj.name}/src/**.hpp",
            "%{prj.name}/src/**.cpp",
            "%{prj.name}/vendor/CLI/CLI11.hpp"
        }

        includedirs {
            "AC_Core/src",
            "%{prj.name}/vendor"
        }

        links {
            "AC_Core"
        }


    group "Tests"

        -- Tests project for AC_Core. It produces executable with
        -- embeded CLI from Catch2. Run this program with no arguments
        -- to simply run all the tests.
        project "AC_Core_Tests"
            location "AC_Core_Tests"
            kind "ConsoleApp"
            language "C++"
            cppdialect "C++17"
            targetname "ac_core_tests"

            files {
                "%{prj.name}/src/**.hpp",
                "%{prj.name}/src/**.cpp",
                "%{prj.name}/vendor/catch2/catch.hpp"
            }

            includedirs {
                "AC_Core/src",
                "%{prj.name}/vendor"
            }

            links {
                "AC_Core"
            }

    group ""


-- Additional actions for maintaining project files.
newaction {
   trigger     = "clean",
   description = "Clean project from binaries and obj files",
   execute = function ()
      print("Removing bin/...")
	  os.rmdir("./bin")
	  print("Removing obj/...")
	  os.rmdir("./obj")
	  print("Done")
   end
}

newaction {
	trigger     = "reset",
	description = "Removes all files ignored by git",
	execute = function()
		print("Removing generated files...")
		os.execute("git clean -Xdf")
		print("Done")
	end
}