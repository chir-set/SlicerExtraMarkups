cmake_minimum_required(VERSION 3.16.3...3.19.7 FATAL_ERROR)

project(ExtraMarkups)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://github.com/chir-set/ExtraMarkups/")
set(EXTENSION_CATEGORY "Utilities")
set(EXTENSION_CONTRIBUTORS "Saleem Edah-Tally ([Surgeon] [Hobbyist developer])")
set(EXTENSION_DESCRIPTION "Set of custom markups.")
set(EXTENSION_ICONURL "https://github.com/chir-set/ExtraMarkups/blob/main/ExtraMarkups.png")
set(EXTENSION_SCREENSHOTURLS "https://github.com/chir-set/ExtraMarkups/blob/main/Label/Label_0.png https://github.com/chir-set/ExtraMarkups/blob/main/Shape/Cone_0.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(Label)
add_subdirectory(Shape)
add_subdirectory(StenosisMeasurement3D)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
