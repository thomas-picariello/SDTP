cmake_minimum_required(VERSION 2.8.8)
cmake_policy(SET CMP0020 NEW)
#Set project name for the whole
set(PROJECT "SVoIP")
project(${PROJECT})

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

#Set variables with all the sources to compile
set(HEADERS
    svoip.h
    contactlistwindow.h
    editcontactwindow.h
    settingswindow.h
    responder.h
    contact.h
    starter.h
    messenger.h
)
set(SOURCES
    main.cpp
    svoip.cpp
    contactlistwindow.cpp
    editcontactwindow.cpp
    settingswindow.cpp
    responder.cpp
    contact.cpp
    starter.cpp
    messenger.cpp
)
set(FORMS
    contactlistwindow.ui
    editcontactwindow.ui
    settingswindow.ui
)

#Find the files alone:
#file(GLOB FORMS *.ui)
#file(GLOB HEADERS *.h)
#file(GLOB SOURCES *.cxx)

#process the ui files.
qt5_wrap_ui(UI_HEADERS ${FORMS})

#set sources groupes
source_group("UI Files" FILES ${FORMS})

add_executable(${PROJECT} ${SOURCES} ${HEADERS} ${UI_HEADERS})

qt5_use_modules(${PROJECT} Widgets Multimedia)

if (MSVC)
  set_target_properties(${PROJECT} PROPERTIES
    WIN32_EXECUTABLE YES
    LINK_FLAGS "/ENTRY:mainCRTStartup"
  )
endif ()
