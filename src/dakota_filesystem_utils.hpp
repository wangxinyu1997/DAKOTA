/*  _______________________________________________________________________

    DAKOTA: Design Analysis Kit for Optimization and Terascale Applications
    Copyright 2014 Sandia Corporation.
    This software is distributed under the GNU Lesser General Public License.
    For more information, see the README file in the top Dakota directory.
    _______________________________________________________________________ */

//- Description:  Filesystem_utils (to be replaced by Boost.Filesystem)
//-
//- Version: $Id$

#ifndef DAKOTA_FILESYSTEM_UTILS_H
#define DAKOTA_FILESYSTEM_UTILS_H

#include "dakota_system_defs.hpp"
#include "dakota_global_defs.hpp"
#include "dakota_data_types.hpp"
#include <boost/algorithm/string.hpp> 
#include <boost/filesystem/operations.hpp>
#include <string>
#include <vector>

#ifdef __SUNPRO_CC
#include <stdlib.h>  // for putenv (eventually, setenv)
#endif


namespace Dakota {

#ifdef DEBUG_LEGACY_WORKDIR
int rec_cp(const char *from, const char *todir, int copy,
           int flatten, int replace);
int rec_rmdir(const char*);
#endif


/// Portability adapter for getcwd
/* Portability adapter for getcwd: return the string in OS-native
   format.  TODO: change paths throughout code to use bfs::path where
   possible, since Windows (and Cygwin) use wchar_t instead of
   char_t. */
inline std::string get_cwd()
{
  // Get the native path and return as a string, using locale-specific
  // conversion from wchar to char if needed.
  return boost::filesystem::current_path().string();
}


/// Utility function for executable file search algorithms
inline std::vector<std::string> get_pathext()
{
  // Get the possible filename extensions from the system environment variable

  char* env_ext_str_list = std::getenv("PATHEXT");

  // Create a SringArray of viable extensions for use in executable
  // detection algorithms

  StringArray ext_list;

  if ( env_ext_str_list )
    boost::split( ext_list, env_ext_str_list, boost::is_any_of(";") );

  ext_list.push_back("");  // Backward compatibility:  Some users may
                           // already be explicit and specify filename.ext
  return ext_list;
}


#ifdef DEBUG_LEGACY_WORKDIR
// WJB:  MAJOR OVERHAUL required to make this function a member of 
//       WorkdirHelper (logically belongs there, but worth the effort?)
void workdir_adjust(const std::string& workdir);
#endif


/// Utility function from borrowed from boost/test
inline void putenv_impl(const char* name_and_value)
{
  if ( putenv( (char*)name_and_value) ) {
    Cerr << "\nError: putenv(" << name_and_value
         << ") failed in putenv_impl()" << std::endl;
    abort_handler(-1);
  }
}

} // namespace Dakota

#endif // DAKOTA_FILESYSTEM_UTILS_H

