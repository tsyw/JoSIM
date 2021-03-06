# Project name
PROJECT(JoSIM)
MESSAGE("Current CMAKE Source dir: ${CMAKE_CURRENT_BINARY_DIR}")
# Welcoming MESSAGE
MESSAGE("Welcome to the JoSIM CMake file.")
MESSAGE("Please wait while we SET a few configuration options.")
MESSAGE("=====================================================")
# Identify the platform type
MESSAGE("Platform: ${CMAKE_SYSTEM_NAME}")
# IdentIFy the build type
IF(CMAKE_BUILD_TYPE MATCHES DEBUG 
    OR CMAKE_BUILD_TYPE MATCHES Debug
    OR CMAKE_BUILD_TYPE MATCHES debug)
    MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")
ELSE()
    # Default build type is release. Change to debug for debugging
    SET(CMAKE_BUILD_TYPE RELEASE)
    MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")
ENDIF()
# SpecIFy the minimum version for CMake
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
# The version number.
SET(JoSIM_VERSION_MAJOR 1)
SET(JoSIM_VERSION_MINOR 2)
# Ensure C++17
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
# See if OpenMP was specified
OPTION(USING_OPENMP "Allow parallel processing in JoSIM" OFF)
IF(USING_OPENMP)
    SET(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
    SET(OpenMP_CXX_LIB_NAMES "omp")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    SET(OpenMP_omp_LIBRARY "/usr/local/lib/libomp.a")
    ADD_DEFINITIONS(-DUSING_OPENMP)
ENDIF()
# Set which graphical interface to use (USING_FLTK, USING_MATPLOTLIB, USING_NONE)
OPTION(USING_FLTK "Using FLTK basic graphical interface" OFF)
OPTION(USING_MATPLOTLIB "Using Python based matplotlib graphical interface" OFF)
OPTION(USING_NONE "Using no plotting engine" ON)
IF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(WINDOWS TRUE)
    SET(PLATFORM "win")
    MESSAGE("=====================================================")
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/suitesparse)
    IF(USING_FLTK)
        MESSAGE("Graphical engine: FLTK")
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/Fl)
        ADD_DEFINITIONS(-DUSING_FLTK)
    ELSEIF(USING_MATLPOTLIB)
        MESSAGE("Graphical engine: Matplotlib")
        ADD_DEFINITIONS(-DUSING_MATPLOTLIB)
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/matplotlib)
    ELSEIF(USING_NONE)
        MESSAGE("Graphical engine: None")
        ADD_DEFINITIONS(-DUSING_NONE)
    ENDIF()
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(LINUX TRUE)
    SET(PLATFORM "linux")
    MESSAGE("=====================================================")
    # SpecIFy path to custom FindSuiteSparse.cmake file
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/lib/${PLATFORM}/")
    IF(USING_FLTK)
        MESSAGE("Graphical engine: FLTK")
        ADD_DEFINITIONS(-DUSING_FLTK)
        FIND_PACKAGE(FLTK REQUIRED)
        INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
    ELSEIF(USING_MATPLOTLIB)
        MESSAGE("Graphical engine: Matplotlib")
        ADD_DEFINITIONS(-DUSING_MATPLOTLIB)
        IF(EXISTS /usr/local/bin/python3.6)
            MESSAGE("Found Python 3.6")
            INCLUDE_DIRECTORIES(/usr/local/include/python3.6m)
            INCLUDE_DIRECTORIES(/usr/local/lib/python3.6/site-packages/numpy/core/include/)
        ELSE()
            FIND_PACKAGE(PythonInterp 2.7 REQUIRED)
            FIND_PACKAGE(PythonLibs 2.7 REQUIRED)
            INCLUDE_DIRECTORIES(/usr/lib64/python2.7/site-packages/numpy/core/include)
        ENDIF()
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/matplotlib)
    ELSEIF(USING_NONE)
        MESSAGE("Graphical engine: None")
        ADD_DEFINITIONS(-DUSING_NONE)
    ENDIF()
    FIND_PACKAGE(SuiteSparse REQUIRED)
	INCLUDE_DIRECTORIES(${SUITESPARSE_INCLUDE_DIRS})
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(MACOSX TRUE)
    SET(PLATFORM "mac")
    MESSAGE("=====================================================")
    IF(USING_FLTK)
        MESSAGE("Graphical engine: FLTK")
        ADD_DEFINITIONS(-DUSING_FLTK)
        FIND_PACKAGE(FLTK REQUIRED)
        INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
    ELSEIF(USING_MATPLOTLIB)
        MESSAGE("Graphical engine: Matplotlib")
        ADD_DEFINITIONS(-DUSING_MATPLOTLIB)
        IF(EXISTS /usr/local/Cellar/python/3.6.5)
            MESSAGE("Found Python 3.6")
            INCLUDE_DIRECTORIES(/usr/local/Cellar/python/3.6.5/Frameworks/Python.framework/Versions/3.6/include/python3.6m)
            INCLUDE_DIRECTORIES(/usr/local/lib/python3.6/site-packages/numpy/core/include)
        ELSE()
            MESSAGE("Found Python 2.7")
            INCLUDE_DIRECTORIES(/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7)
            INCLUDE_DIRECTORIES(/System/Library/Frameworks/Python.framework/Versions/2.7/Extras/lib/python/numpy/core/include)
        ENDIF()
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/matplotlib)
    ELSEIF(USING_NONE)
        MESSAGE("Graphical engine: None")
        ADD_DEFINITIONS(-DUSING_NONE)
    ENDIF()
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/suitesparse)
ENDIF()
# Include directory for JoSIM header files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/JoSIM)
# Add the source files for JoSIM
ADD_SUBDIRECTORY(src)

# Add the source to the executable for compilation
ADD_EXECUTABLE(JoSIM ${JOSIM_SRC})
IF(USING_OPENMP)
    TARGET_LINK_LIBRARIES(JoSIM ${OpenMP_omp_LIBRARY})
    MESSAGE("Parellization is ENABLED")
ELSE()
    MESSAGE("Parellization is DISABLED")
ENDIF()
# IF Darwin system FLTK can be installed from source and found but SuiteSparse is harder.
# We therefore provide SuiteSparse libraries as Darwin is not as fragmented and libraries can be precompiled
IF(MACOSX)
    IF(USING_FLTK)
        TARGET_LINK_LIBRARIES(JoSIM ${FLTK_LIBRARIES})
    ELSEIF(USING_MATPLOTLIB)
        IF(EXISTS /usr/local/Cellar/python/3.6.5)
            MESSAGE("Finding Python 3.6 library:")
            TARGET_LINK_LIBRARIES(JoSIM /usr/local/Cellar/python/3.6.5/Frameworks/Python.framework/Versions/3.6/lib/libpython3.6.dylib)
            MESSAGE("Found: /usr/local/Cellar/python/3.6.5/Frameworks/Python.framework/Versions/3.6/lib/libpython3.6.dylib")
        ELSE()
            MESSAGE("Finding Python 2.7 library:")
            TARGET_LINK_LIBRARIES(JoSIM /usr/lib/libpython2.7.dylib)
            MESSAGE("Found: /usr/lib/libpython2.7.dylib")
        ENDIF()
    ENDIF()
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libamd.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libbtf.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libcamd.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libccolamd.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libcholmod.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libcolamd.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libcxsparse.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libklu.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libldl.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libspqr.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libsuitesparseconfig.a)
    TARGET_LINK_LIBRARIES(JoSIM ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM}/libumfpack.a)
ELSEIF(LINUX)
    IF(USING_FLTK)
        TARGET_LINK_LIBRARIES(JoSIM ${FLTK_LIBRARIES})
    ELSEIF(USING_MATPLOTLIB)
        IF(EXISTS /usr/local/bin/python3.6)
            MESSAGE("Finding Python 3.6 library:")
            TARGET_LINK_LIBRARIES(JoSIM /usr/local/lib/libpython3.6m.a)
            MESSAGE("Found: /usr/local/lib/libpython3.6m.a")
        ELSE()
            TARGET_INCLUDE_DIRECTORIES(JoSIM PRIVATE ${PYTHON_INCLUDE_DIRS})
            TARGET_LINK_LIBRARIES(JoSIM ${PYTHON_LIBRARIES})
        ENDIF()
    ENDIF()
    TARGET_LINK_LIBRARIES(JoSIM ${SUITESPARSE_LIBRARIES}) 
ENDIF()
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_${PLATFORM}_${CMAKE_BUILD_TYPE})
SET(EXECUTABLE_NAME ${PROJECT_NAME}_${PLATFORM}_${CMAKE_BUILD_TYPE})
MESSAGE("Setting executable name to: ${EXECUTABLE_NAME}")
MESSAGE("=====================================================")
MESSAGE("Configuration done.")
MESSAGE("Now run make")