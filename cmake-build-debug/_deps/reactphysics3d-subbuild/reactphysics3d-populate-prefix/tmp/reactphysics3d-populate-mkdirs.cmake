# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/download/reactphysics3d/ff4072d3c72e1ee92428ade193a89f67d65269da/reactphysics3d"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/reactphysics3d-build"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/tmp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src/reactphysics3d-populate-stamp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src/reactphysics3d-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src/reactphysics3d-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src/reactphysics3d-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
