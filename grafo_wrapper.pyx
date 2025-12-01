# distutils: language = c++
# cython: language_level=3

from grafo_wrapper cimport GrafoDisperso
from libcpp cimport bool
from libc.stdlib cimport malloc, free

cdef class PyGrafoDisperso:
    """
    Wrapper Python para la clase C++ GrafoDisperso
    
    Esta clase encapsula el objeto C++ y proporciona una interfaz Python
    para interactuar con el grafo disperso implementado en C++.
    """
    cdef GrafoDisperso* c_grafo  # Puntero al objeto C++
    
    def __cinit__(self):
        """Constructor: crea instancia del grafo C++"""
        self.c_grafo = new GrafoDisperso()
    
    def __dealloc__(self):
        """Destructor: libera memoria del objeto C++"""
        del self.c_grafo
    
    def cargar_datos(self, str nombre_archivo):
        """
        Carga datos desde un archivo Edge List
        
        Args:
            nombre_archivo: Ruta al archivo de datos
            
        Returns:
            True si la carga fue exitosa, False en caso contrario
        """
        cdef bytes py_bytes = nombre_archivo.encode('utf-8')
        cdef const char* c_string = py_bytes
        return self.c_grafo.cargarDatos(c_string)
    
    def bfs(self, int nodo_inicial, int profundidad_maxima):
        """
        Ejecuta búsqueda en anchura (BFS) desde un nodo inicial
        
        Args:
            nodo_inicial: ID del nodo desde donde iniciar
            profundidad_maxima: Profundidad máxima de exploración
            
        Returns:
            Lista de nodos visitados en orden de exploración
        """
        cdef int tam_resultado = 0
        cdef int* resultado = self.c_grafo.BFS(nodo_inicial, profundidad_maxima, tam_resultado)
        
        # Convertir arreglo C++ a lista Python
        py_resultado = []
        if resultado != NULL:
            for i in range(tam_resultado):
                py_resultado.append(resultado[i])
            
            # NOTA: La memoria se libera automáticamente en el destructor de GrafoDisperso
            # o podríamos usar delete[] en C++, pero Cython no soporta esa sintaxis directamente
            # Por ahora, confiamos en que el código C++ gestiona correctamente la memoria
        
        return py_resultado
    
    def obtener_grado(self, int nodo):
        """
        Obtiene el grado (número de conexiones) de un nodo
        
        Args:
            nodo: ID del nodo a consultar
            
        Returns:
            Grado del nodo
        """
        return self.c_grafo.obtenerGrado(nodo)
    
    def get_vecinos(self, int nodo):
        """
        Obtiene la lista de vecinos de un nodo
        
        Args:
            nodo: ID del nodo a consultar
            
        Returns:
            Lista con IDs de los nodos vecinos
        """
        cdef int tam_vecinos = 0
        cdef int* vecinos = self.c_grafo.getVecinos(nodo, tam_vecinos)
        
        # Convertir arreglo C++ a lista Python
        py_vecinos = []
        if vecinos != NULL:
            for i in range(tam_vecinos):
                py_vecinos.append(vecinos[i])
            
            # NOTA: La memoria se libera automáticamente en el destructor de GrafoDisperso
            # o podríamos usar delete[] en C++, pero Cython no soporta esa sintaxis directamente
        
        return py_vecinos
    
    def get_num_nodos(self):
        """Obtiene el número total de nodos en el grafo"""
        return self.c_grafo.getNumNodos()
    
    def get_num_aristas(self):
        """Obtiene el número total de aristas en el grafo"""
        return self.c_grafo.getNumAristas()
    
    def get_nodo_mayor_grado(self):
        """Encuentra el nodo con mayor grado (más conexiones)"""
        return self.c_grafo.getNodoMayorGrado()
    
    def get_memoria_estimada_mb(self):
        """Obtiene la memoria estimada usada por la estructura CSR en MB"""
        return self.c_grafo.getMemoriaEstimadaMB()

