from setuptools import setup, Extension
from Cython.Build import cythonize
import sys

# Configuración para compilador C++
extra_compile_args = []
extra_link_args = []

if sys.platform == 'win32':
    # Windows con MSVC
    extra_compile_args = ['/std:c++14', '/EHsc']
else:
    # Linux/Mac con GCC/Clang
    extra_compile_args = ['-std=c++14']

# Definir extensión Cython
extensions = [
    Extension(
        name="grafo_wrapper",
        sources=[
            "grafo_wrapper.pyx",
            "GrafoDisperso.cpp"
        ],
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    )
]

setup(
    name="NeuroNet",
    version="1.0",
    description="Sistema híbrido C++/Python para análisis de grafos masivos",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
    zip_safe=False,
)
