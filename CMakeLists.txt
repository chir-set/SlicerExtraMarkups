cmake_minimum_required(VERSION 3.16.3...3.19.7 FATAL_ERROR)

project(ExtraMarkups)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://github.com/chir-set/SlicerExtraMarkups/")
set(EXTENSION_CATEGORY "Utilities")
set(EXTENSION_CONTRIBUTORS "Saleem Edah-Tally ([Surgeon] [Hobbyist developer])")
set(EXTENSION_DESCRIPTION "This extension adds a <i>Label</i> markups node that draws an arrow with a text label, and a <i>Shape</i> markups node that can draw defined primitive shapes (circle, cone...). The result is controlled by placing usual control points in slice views and 3D views. Each object can be saved in scene and reloaded.")
set(EXTENSION_ICONURL "https://github.com/chir-set/SlicerExtraMarkups/raw/main/ExtraMarkups.png")
set(EXTENSION_SCREENSHOTURLS "https://github.com/chir-set/SlicerExtraMarkups/raw/main/Label/Label_0.png https://github.com/chir-set/SlicerExtraMarkups/raw/main/Shape/Cone_0.png https://github.com/chir-set/SlicerExtraMarkups/raw/main/Shape/Cylinder_0.png https://github.com/chir-set/SlicerExtraMarkups/raw/main/Shape/Disk_0.png https://github.com/chir-set/SlicerExtraMarkups/raw/main/Shape/Ring_0.png https://github.com/chir-set/SlicerExtraMarkups/raw/main/Shape/Sphere_0.png https://github.com/chir-set/SlicerExtraMarkups/raw/main/Shape/Tube_0.png https://github.com/chir-set/SlicerExtraMarkups/raw/main/Shape/Arc_0.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(Label)
add_subdirectory(Shape)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
