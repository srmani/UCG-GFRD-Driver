# Locate MPI
#-------------
find_package(MPI)
if(MPI_FOUND)
   include_directories(${MPI_INCLUDE_PATH})
else()
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../STUBS_MPI/mpi.h ${CMAKE_CURRENT_BINARY_DIR}/../STUBS_MPI/mpi.h COPYONLY)
endif()

set( mdi_include_path ${CMAKE_CURRENT_BINARY_DIR}/../UCG-GFRD-Driver/mdi/MDI_Library/ )
set( mdi_lib_path ${CMAKE_CURRENT_BINARY_DIR}/../UCG-GFRD-Driver/mdi/MDI_Library/ )

# Link to MDI
#-------------
link_directories( ${mdi_lib_path} )
include_directories( ${mdi_include_path} )

message(STATUS "building unit tests")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/in.lammps DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data.lammps DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cg_force_pair.table DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB files "test_*.cpp")

foreach(file ${files})
	string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${file})
	add_executable(${file_without_ext} ${file})
	target_link_libraries(${file_without_ext} mdi
                      ${MPI_LIBRARIES})
        set( driver_params -np 1 "./${file_without_ext}" )
	set( lammps_params -np 4 "/home/travis/build/srmani/UCG-GFRD-Driver/lammps/src/lmp_mpi" )

	message(${driver_params})
	message(${lammps_params})

	add_test(NAME ${file_without_ext} COMMAND "mpirun" ${driver_params} -mdi "-role DRIVER -name driver -method MPI" : ${lammps_params} -mdi "-role ENGINE -name LAMMPS -method MPI" -in in.lammps)

endforeach()
