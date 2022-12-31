add_executable(gpmf-dump gpmf-dump.cpp)
target_link_libraries(gpmf-dump PRIVATE
    GoProTelem)

add_executable(gpmf2csv gpmf2csv.cpp)
target_link_libraries(gpmf2csv PRIVATE
    GoProTelem)