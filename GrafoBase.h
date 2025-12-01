#ifndef GRAFOBASE_H
#define GRAFOBASE_H

#include <iostream>

/**
 * @class GrafoBase
 * @brief Clase abstracta que define la interfaz para grafos masivos
 * 
 * Esta clase base utiliza POO avanzada con métodos virtuales puros
 * para permitir diferentes implementaciones de grafos (dispersos, densos, etc.)
 * 
 * NOTA: Usa únicamente punteros y arreglos dinámicos (new/delete)
 */
class GrafoBase {
public:
    /**
     * @brief Destructor virtual para correcta gestión de memoria en herencia
     */
    virtual ~GrafoBase() {}

    /**
     * @brief Carga datos desde un archivo formato Edge List
     * @param nombreArchivo Ruta al archivo de datos (formato: nodoOrigen nodoDestino)
     * @return true si la carga fue exitosa, false en caso contrario
     */
    virtual bool cargarDatos(const char* nombreArchivo) = 0;

    /**
     * @brief Ejecuta búsqueda en anchura (BFS) desde un nodo inicial
     * @param nodoInicial ID del nodo desde donde iniciar la búsqueda
     * @param profundidadMaxima Profundidad máxima de exploración
     * @param resultado Puntero a arreglo donde se almacenarán los nodos visitados
     * @param tamResultado Referencia donde se guardará el tamaño del resultado
     * @return Puntero a arreglo dinámico con nodos visitados (debe liberarse con delete[])
     */
    virtual int* BFS(int nodoInicial, int profundidadMaxima, int& tamResultado) = 0;

    /**
     * @brief Obtiene el grado (número de conexiones) de un nodo
     * @param nodo ID del nodo a consultar
     * @return Grado del nodo (número de aristas salientes)
     */
    virtual int obtenerGrado(int nodo) = 0;

    /**
     * @brief Obtiene la lista de vecinos de un nodo
     * @param nodo ID del nodo a consultar
     * @param tamVecinos Referencia donde se guardará el número de vecinos
     * @return Puntero a arreglo dinámico con IDs de vecinos (debe liberarse con delete[])
     */
    virtual int* getVecinos(int nodo, int& tamVecinos) = 0;

    /**
     * @brief Obtiene el número total de nodos en el grafo
     * @return Cantidad de nodos
     */
    virtual int getNumNodos() const = 0;

    /**
     * @brief Obtiene el número total de aristas en el grafo
     * @return Cantidad de aristas
     */
    virtual int getNumAristas() const = 0;

    /**
     * @brief Encuentra el nodo con mayor grado (más conexiones)
     * @return ID del nodo con mayor grado
     */
    virtual int getNodoMayorGrado() = 0;
};

#endif // GRAFOBASE_H
