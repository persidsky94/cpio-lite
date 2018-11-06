project(samples)

include_directories(../include)

add_executable(get_files_list src/get_files_list.cpp)
target_link_libraries(get_files_list ${CMAKE_DL_LIBS})

add_executable(archivate_files src/archivate_files.cpp)
target_link_libraries(archivate_files ${CMAKE_DL_LIBS})

add_executable(unpack_file src/unpack_file.cpp)
target_link_libraries(unpack_file ${CMAKE_DL_LIBS})

