from distutils.core import setup, Extension

setup(
      ext_modules=[Extension('_vdwo', ['vdwo-python.i','src/vdwo.cpp'],
                             include_dirs=['/usr/include/openbabel-2.0','src'],
                             libraries = ['openbabel', 'gomp'],
                             extra_compile_args=['-std=c++17', '-fopenmp'],
                             swig_opts=['-c++', '-modern'])],
      py_modules=['vdwo'],
)

