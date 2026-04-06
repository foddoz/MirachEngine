# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/download/assimp/248954a90c949291c541ba7ee69b327d01e5f212/assimp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-build"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-subbuild/assimp-populate-prefix"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-subbuild/assimp-populate-prefix/tmp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-subbuild/assimp-populate-prefix/src/assimp-populate-stamp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-subbuild/assimp-populate-prefix/src"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-subbuild/assimp-populate-prefix/src/assimp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-subbuild/assimp-populate-prefix/src/assimp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-subbuild/assimp-populate-prefix/src/assimp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
