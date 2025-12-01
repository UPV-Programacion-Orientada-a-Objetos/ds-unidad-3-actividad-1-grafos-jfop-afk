# distutils: language = c++
# cython: language_level=3

from libcpp cimport bool

cdef extern from "GrafoDisperso.h":
    cdef cppclass GrafoDisperso:
        GrafoDisperso() except +
        bool cargarDatos(const char* nombreArchivo)
        int* BFS(int nodoInicial, int profundidadMaxima, int& tamResultado)
        int obtenerGrado(int nodo)
        int* getVecinos(int nodo, int& tamVecinos)
        int getNumNodos()
        int getNumAristas()
        int getNodoMayorGrado()
        double getMemoriaEstimadaMB()
