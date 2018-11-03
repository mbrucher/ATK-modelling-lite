
function(create_Python_test name cwd)
  add_test(NAME ${name} COMMAND ${PYTHON_EXECUTABLE} -m nose --exe WORKING_DIRECTORY ${cwd})
endfunction(create_Python_test)

create_Python_test(PyModelling ${CMAKE_CURRENT_LIST_DIR})

file(GLOB entries *.py)
add_custom_target(PyModelling_test SOURCES ${entries})
set_target_properties (PyModelling_test PROPERTIES
    FOLDER Tests/Python
)
