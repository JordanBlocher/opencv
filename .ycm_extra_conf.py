# This file is NOT licensed under the GPLv3, which is the license for the rest
# of YouCompleteMe.
#
# Here's the license text for this file:
#
# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>

import os
import ycm_core

qt4 = [
'-DQT_NO_DEBUG_OUTPUT',
'-DQT_USE_FAST_CONCATENATION',
'-DQT_USE_FAST_OPERATOR_PLUS',
'-D_GNU_SOURCE',
'-DQT_NO_STL',
'-DQT_NO_CAST_TO_ASCII',
'-D_REENTRANT',
'-DKDE_DEPRECATED_WARNINGS',
'-I/usr/include',
'-I/usr/include/qt4',
'-I/usr/include/qt4/Qt',
'-I/usr/include/qt4/Qt3Support',
'-I/usr/include/qt4/QtCore',
'-I/usr/include/qt4/QtDBus',
'-I/usr/include/qt4/QtDeclarative',
'-I/usr/include/qt4/QtDesigner',
'-I/usr/include/qt4/QtDesigner',
'-I/usr/include/qt4/QtGui',
'-I/usr/include/qt4/QtHelp',
'-I/usr/include/qt4/QtNetwork',
'-I/usr/include/qt4/QtOpenGL',
'-I/usr/include/qt4/QtScript',
'-I/usr/include/qt4/QtScriptTools',
'-I/usr/include/qt4/QtSql',
'-I/usr/include/qt4/QtSvg',
'-I/usr/include/qt4/QtTest',
'-I/usr/include/qt4/QtUiTools',
'-I/usr/include/qt4/QtWebKit',
'-I/usr/include/qt4/QtXml',
'-I/usr/include/qt4/QtXmlPatterns',
'-I/usr/share/qt4/mkspecs/default'
]

cv2 = [
'-I/usr/local/include/opencv',
'-I/usr/local/include'
]

# Defines the GL_CODE_COMPLETE_ONLY_FLAG macro, this means do the following in the C++ code
# #ifdef GL_CODE_COMPLETE_ONLY_FLAG
#     #include <GL/gl.h>
#     #include <GL/glext.h>
# #else
#     #include <GL/glew.h>
#     #include <GL/gl.h>
# #endif
gl = [
'-DGL_GLEXT_PROTOTYPES'
]

# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
'-Wall',
'-DYOU_COMPLETE_ME',
'-Wextra',
'-Werror',
'-Wno-long-long',
'-Wno-variadic-macros',
'-DNDEBUG',
'-DUSE_CLANG_COMPLETER',
'-std=c++11',
'-I/home/frags/repos/cs674/include'
] + cv2

# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# Most projects will NOT need to set this to anything; you can just change the
# 'flags' list of compilation flags. Notice that YCM itself uses that approach.
compilation_database_folder = ''

if compilation_database_folder:
  database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
  database = None


def DirectoryOfThisScript():
  return os.path.dirname( os.path.abspath( __file__ ) )


def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
  if not working_directory:
    return list( flags )
  new_flags = []
  make_next_absolute = False
  path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ]
  for flag in flags:
    new_flag = flag

    if make_next_absolute:
      make_next_absolute = False
      if not flag.startswith( '/' ):
        new_flag = os.path.join( working_directory, flag )

    for path_flag in path_flags:
      if flag == path_flag:
        make_next_absolute = True
        break

      if flag.startswith( path_flag ):
        path = flag[ len( path_flag ): ]
        new_flag = path_flag + os.path.join( working_directory, path )
        break

    if new_flag:
      new_flags.append( new_flag )
  return new_flags


def FlagsForFile( filename ):
  if database:
    # Bear in mind that compilation_info.compiler_flags_ does NOT return a
    # python list, but a "list-like" StringVec object
    compilation_info = database.GetCompilationInfoForFile( filename )
    final_flags = MakeRelativePathsInFlagsAbsolute(
      compilation_info.compiler_flags_,
      compilation_info.compiler_working_dir_ )

    # NOTE: This is just for YouCompleteMe; it's highly likely that your project
    # does NOT need to remove the stdlib flag. DO NOT USE THIS IN YOUR
    # ycm_extra_conf IF YOU'RE NOT 100% YOU NEED IT.
#    try:
#      final_flags.remove( '-stdlib=libc++' )
#    except ValueError:
#      pass
  else:
    relative_to = DirectoryOfThisScript()
    final_flags = MakeRelativePathsInFlagsAbsolute( flags, relative_to )

  return {
    'flags': final_flags,
    'do_cache': True
  }
