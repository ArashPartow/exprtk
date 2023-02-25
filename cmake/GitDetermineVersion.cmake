# **************************************************************
# *         C++ Mathematical Expression Toolkit Library        *
# *                                                            *
# * Author: Arash Partow (1999-2023)                           *
# * URL: https://www.partow.net/programming/exprtk/index.html  *
# *                                                            *
# * Copyright notice:                                          *
# * Free use of the Mathematical Expression Toolkit Library is *
# * permitted under the guidelines and in accordance with the  *
# * most current version of the MIT License.                   *
# * http://www.opensource.org/licenses/MIT                     *
# *                                                            *
# **************************************************************

# Used to determine the version of a repository using "git describe", if git
# is found. On success sets following variables in caller's scope:
#   ${var_prefix}_VERSION
#   ${var_prefix}_VERSION_MAJOR
#   ${var_prefix}_VERSION_MINOR
#   ${var_prefix}_VERSION_PATCH
#   ${var_prefix}_VERSION_PATCH_EXTRA
#   ${var_prefix}_VERSION_FULL
#
# If git is not found, or git describe cannot be run successfully, then these
# variables are left unchanged and status message is printed.
#
# Arguments are:
#   source_dir : Source directory
#   git_command : git executable
#   var_prefix : prefix for variables e.g. "Project".
function(determine_version source_dir git_command var_prefix)
  if ("$Format:$" STREQUAL "")
    # We are in an exported tarball and should use the shipped version
    # information. Just return here to avoid the warning message at the end of
    # this function.
    return()
  elseif (NOT ${CMAKE_PROJECT_NAME}_GIT_DESCRIBE} AND
    EXISTS ${git_command} AND
    EXISTS ${source_dir}/.git)
    execute_process(
      COMMAND ${git_command} describe --tags
      WORKING_DIRECTORY ${source_dir}
      RESULT_VARIABLE result
      OUTPUT_VARIABLE output
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE)
    if (NOT result EQUAL 0)
      # git describe failed (bad return code).
      set(output "")
    endif ()
  else ()
    # note, output may be set to empty if ${CMAKE_PROJECT_NAME}_GIT_DESCRIBE is not defined.
    set(output "${CMAKE_PROJECT_NAME}_GIT_DESCRIBE")
  endif ()

  # check the output is not empty
  if (NOT output)
    message(STATUS
      "Could not use git to determine source version")
    return()
  endif ()
  extract_version_components(${source_dir} "${output}" tmp)
  if (DEFINED tmp_VERSION)
    foreach (suffix VERSION VERSION_MAJOR VERSION_MINOR VERSION_PATCH
      VERSION_PATCH_EXTRA VERSION_FULL)
      set(${var_prefix}_${suffix} ${tmp_${suffix}} PARENT_SCOPE)
    endforeach ()
  else ()
    message(STATUS "Could not use git to determine source version")
  endif ()
endfunction()

# Extracts components from a version string. See determine_version() for usage.
function(extract_version_components source_dir version_string var_prefix)
  string(REGEX MATCH "^v?(([0-9]+)\\.([0-9]+)\\.([0-9]+)-?(.*))$"
    version_matches "${version_string}")
  if (CMAKE_MATCH_0)
    # note, we don't use CMAKE_MATCH_0 for `full` since it may or may not have
    # the `v` prefix.
    set(full ${CMAKE_MATCH_1})
    set(major ${CMAKE_MATCH_2})
    set(minor ${CMAKE_MATCH_3})
    set(patch ${CMAKE_MATCH_4})
    set(patch_extra ${CMAKE_MATCH_5})

    set(${var_prefix}_VERSION "${major}.${minor}.${patch}" PARENT_SCOPE)
    set(${var_prefix}_VERSION_MAJOR ${major} PARENT_SCOPE)
    set(${var_prefix}_VERSION_MINOR ${minor} PARENT_SCOPE)
    set(${var_prefix}_VERSION_PATCH ${patch} PARENT_SCOPE)
    set(${var_prefix}_VERSION_PATCH_EXTRA ${patch_extra} PARENT_SCOPE)
    set(${var_prefix}_VERSION_FULL ${full} PARENT_SCOPE)
  endif ()
endfunction()
