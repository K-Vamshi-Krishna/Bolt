message( STATUS "Setting up Boost SuperBuild... Shiny!" )
include( ExternalProject )

set( ext.Boost_VERSION "1.50.0" CACHE STRING "Boost version to download/use" )
mark_as_advanced( ext.Boost_VERSION )
string( REPLACE "." "_" ext.Boost_Version_Underscore ${ext.Boost_VERSION} )

message( STATUS "ext.Boost_VERSION: " ${ext.Boost_VERSION} )

# Purely for debugging the file downloading URLs
# file( DOWNLOAD "http://downloads.sourceforge.net/project/boost/boost/1.49.0/boost_1_49_0.7z" 
		# "${CMAKE_CURRENT_BINARY_DIR}/download/boost-${ext.Boost_VERSION}/boost_1_49_0.7z" SHOW_PROGRESS STATUS fileStatus LOG fileLog )
# message( STATUS "status: " ${fileStatus} )
# message( STATUS "log: " ${fileLog} )

set( Boost.Command b2 --with-program_options )

if( Bolt_BUILD64 )
	list( APPEND Boost.Command address-model=64 )
else( )
	list( APPEND Boost.Command address-model=32 )
endif( )

if( MSVC )
	if( MSVC_VERSION VERSION_LESS 1700 )
		list( APPEND Boost.Command toolset=msvc-10.0 )
	else( )
		list( APPEND Boost.Command toolset=msvc-11.0 )
	endif( )
endif( )

list( APPEND Boost.Command link=static stage )

#set( ext.Boost_URL "http://downloads.sourceforge.net/project/boost/boost/${ext.Boost_VERSION}/boost_${ext.Boost_Version_Underscore}.zip" CACHE STRING "URL to download Boost from" )
set( ext.Boost_URL "http://see-srv/share/code/externals/boost/boost_${ext.Boost_Version_Underscore}.zip" CACHE STRING "URL to download Boost from" )
mark_as_advanced( ext.Boost_URL )

# Below is a fancy CMake command to download, build and install Boost on the users computer
ExternalProject_Add(
    Boost
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/boost
    URL ${ext.Boost_URL}
	URL_MD5 c32608561de9184d1c940c9977d04339
    UPDATE_COMMAND "bootstrap.bat"
#    PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ${Boost.Command}
	BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
)

set_property( TARGET Boost PROPERTY FOLDER "Externals")

ExternalProject_Get_Property( Boost source_dir )
ExternalProject_Get_Property( Boost binary_dir )
set( Boost_INCLUDE_DIRS ${source_dir} )
set( Boost_LIBRARIES debug;${binary_dir}/stage/lib/libboost_program_options-vc110-mt-gd-1_50.lib;optimized;${binary_dir}/stage/lib/libboost_program_options-vc110-mt-1_50.lib )

set( Boost_FOUND TRUE )

# Can't get packages to download from github because the cmake file( download ... ) does not understand https protocal
# Gitorious is problematic because it apparently only offers .tar.gz files to download, which windows doesn't support by default
# Also, both repo's do not like to append a filetype to the URL, instead they use some forwarding script.  ExternalProject_Add wants a filetype.
# set( BOOST_BUILD_PROJECTS program_options CACHE STRING "* seperated Boost modules to be built")
# ExternalProject_Add(
   # Boost
   # URL http://gitorious.org/boost/cmake/archive-tarball/cmake-${ext.Boost_VERSION}.tar.gz
   # LIST_SEPARATOR *
   # CMAKE_ARGS 	-DENABLE_STATIC=ON 
				# -DENABLE_SHARED=OFF 
				# -DENABLE_DEBUG=OFF 
				# -DENABLE_RELEASE=ON 
				# -DENABLE_SINGLE_THREADED=OFF 
				# -DENABLE_MULTI_THREADED=ON 
				# -DENABLE_STATIC_RUNTIME:BOOL=OFF 
				# -DENABLE_DYNAMIC_RUNTIME=ON 
				# -DWITH_PYTHON:BOOL=OFF 
				# -DBUILD_PROJECTS=${BOOST_BUILD_PROJECTS} ${CMAKE_ARGS}
# )