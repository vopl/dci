
if(NOT COMMAND dciUtilsPch)
    function(dciUtilsPch target pch)

       if(COMMAND target_precompile_headers)
           target_precompile_headers(${target} PRIVATE ${pch})
       endif()

    endfunction()
endif()
