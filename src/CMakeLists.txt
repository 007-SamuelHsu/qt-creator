# Not in the main CMakeLists.txt file because some tests fail if we have the flags set as default
list(APPEND DEFAULT_DEFINES
  QT_CREATOR
  QT_NO_JAVA_STYLE_ITERATORS
  QT_NO_CAST_TO_ASCII QT_RESTRICTED_CAST_FROM_ASCII
  QT_DISABLE_DEPRECATED_BEFORE=0x050900
  QT_USE_FAST_OPERATOR_PLUS
  QT_USE_FAST_CONCATENATION
)

add_library(app_version INTERFACE)
target_include_directories(app_version INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(TARGETS app_version EXPORT QtCreator)

add_subdirectory(libs)
add_subdirectory(share)
add_subdirectory(shared)
add_subdirectory(app)
add_subdirectory(plugins)
add_subdirectory(tools)

install(
  FILES
    ${PROJECT_SOURCE_DIR}/HACKING
    ${PROJECT_SOURCE_DIR}/LICENSE.GPL3-EXCEPT
    ${PROJECT_SOURCE_DIR}/README.md
  DESTINATION ./
  COMPONENT Devel EXCLUDE_FROM_ALL
)

install(
  DIRECTORY
    ${PROJECT_SOURCE_DIR}/doc
    ${PROJECT_SOURCE_DIR}/scripts
  DESTINATION ./
  COMPONENT Devel EXCLUDE_FROM_ALL
)

install(EXPORT QtCreator
  DESTINATION "lib/cmake/QtCreator"
  COMPONENT Devel EXCLUDE_FROM_ALL
  NAMESPACE QtCreator::
  FILE QtCreatorTargets.cmake
)

file(WRITE ${CMAKE_BINARY_DIR}/cmake/QtCreatorConfig.cmake "
include(CMakeFindDependencyMacro)
find_dependency(Qt5
  COMPONENTS Concurrent Core Network PrintSupport Qml Quick QuickWidgets Sql REQUIRED
)

if (NOT IDE_VERSION)
  include(\${CMAKE_CURRENT_LIST_DIR}/QtCreatorIDEBranding.cmake)
endif()

if (NOT DEFINED add_qtc_plugin)
  include(\${CMAKE_CURRENT_LIST_DIR}/QtCreatorAPI.cmake)
endif()

if (NOT TARGET QtCreator::Core)
  include(\${CMAKE_CURRENT_LIST_DIR}/QtCreatorTargets.cmake)
endif()
")

# Local build export
export(EXPORT QtCreator
  NAMESPACE QtCreator::
  FILE ${CMAKE_BINARY_DIR}/cmake/QtCreatorTargets.cmake)

file(COPY
  ${PROJECT_SOURCE_DIR}/cmake/QtCreatorIDEBranding.cmake
  ${PROJECT_SOURCE_DIR}/cmake/QtCreatorAPI.cmake
  DESTINATION ${CMAKE_BINARY_DIR}/cmake
)

# Devel package install
install(
  FILES
    ${PROJECT_SOURCE_DIR}/cmake/QtCreatorIDEBranding.cmake
    ${PROJECT_SOURCE_DIR}/cmake/QtCreatorAPI.cmake
    ${CMAKE_BINARY_DIR}/cmake/QtCreatorConfig.cmake
  DESTINATION lib/cmake/QtCreator
  COMPONENT Devel EXCLUDE_FROM_ALL
)
