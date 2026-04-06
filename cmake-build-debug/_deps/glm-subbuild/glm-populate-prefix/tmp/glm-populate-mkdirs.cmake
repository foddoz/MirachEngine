# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/download/glm/247d93788574cd7af455045b4ae5e6d8c4758fd1/glm"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/glm-build"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/glm-subbuild/glm-populate-prefix"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/glm-subbuild/glm-populate-prefix/tmp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/glm-subbuild/glm-populate-prefix/src"
  "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
