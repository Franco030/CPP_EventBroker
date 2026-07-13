# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-src")
  file(MAKE_DIRECTORY "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-src")
endif()
file(MAKE_DIRECTORY
  "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-build"
  "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-subbuild/cpp_orm-populate-prefix"
  "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-subbuild/cpp_orm-populate-prefix/tmp"
  "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-subbuild/cpp_orm-populate-prefix/src/cpp_orm-populate-stamp"
  "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-subbuild/cpp_orm-populate-prefix/src"
  "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-subbuild/cpp_orm-populate-prefix/src/cpp_orm-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-subbuild/cpp_orm-populate-prefix/src/cpp_orm-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/edwin/CPPLearning/cpp_event_broker/build/_deps/cpp_orm-subbuild/cpp_orm-populate-prefix/src/cpp_orm-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
