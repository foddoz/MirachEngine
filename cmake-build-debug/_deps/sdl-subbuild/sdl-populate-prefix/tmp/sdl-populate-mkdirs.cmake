# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/download/sdl/c36bd7f276274a93380f372abcd6d4cccdd20c88/sdl"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/sdl-build"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/sdl-subbuild/sdl-populate-prefix"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/sdl-subbuild/sdl-populate-prefix/tmp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/sdl-subbuild/sdl-populate-prefix/src/sdl-populate-stamp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/sdl-subbuild/sdl-populate-prefix/src"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/sdl-subbuild/sdl-populate-prefix/src/sdl-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/sdl-subbuild/sdl-populate-prefix/src/sdl-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/sdl-subbuild/sdl-populate-prefix/src/sdl-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
