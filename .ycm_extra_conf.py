import os
import subprocess

def FlagsForFile(filename, **_kwargs):
  return {
    'flags': subprocess.check_output(['make', '{}/.ycm_debug_flags'.format(filename)]).split(),
    'do_cache': True
  }
