cmake_minimum_required(VERSION 3.16.3...3.19.7 FATAL_ERROR)

project(ExtraMarkups)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://www.slicer.org/wiki/Documentation/Nightly/Extensions/ExtraMarkups")
set(EXTENSION_CATEGORY "Testing")
set(EXTENSION_CONTRIBUTORS "SET ([Surgeon] [Hobbyist developer])")
set(EXTENSION_DESCRIPTION "Set of derived custom markups.")
set(EXTENSION_ICONURL "https://www.example.com/Slicer/Extensions/ExtraMarkups.png")
set(EXTENSION_SCREENSHOTURLS "https://www.example.com/Slicer/Extensions/ExtraMarkups/Screenshots/1.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(Ring)
add_subdirectory(Label)
add_subdirectory(Disk)
add_subdirectory(Sphere)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
