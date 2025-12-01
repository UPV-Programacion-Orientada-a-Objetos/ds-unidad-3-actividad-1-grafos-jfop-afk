#ifndef GRAFODISPERSO_H
#define GRAFODISPERSO_H

#include "GrafoBase.h"
#include <iostream>

/**
 * @class GrafoDisperso
 * @brief Implementación de grafo usando representación CSR (Compressed Sparse Row)
 * 
 * Esta clase implementa una matriz de adyacencia dispersa usando el formato CSR
 * para optimizar el uso de memoria en grafos con millones de nodos.
 * 
 * Formato CSR (usando punteros manuales):
 * - row_ptr: Arreglo de punteros al inicio de cada fila (tamaño: numNodos + 1)
 * - col_indices: Arreglo de índices de columnas (vecinos) (tamaño: numAristas)
 * 
 * NOTA: Usa únicamente punteros y arreglos dinámicos (new/delete)
 */
class GrafoDisperso : public GrafoBase {
private:
    // Representación CSR (Compressed Sparse Row) con punteros manuales
    int* row_ptr;           // Punteros de inicio de fila
    int* col_indices;       // Índices de columnas (vecinos)
    
    // Metadatos del grafo
    int numNodos;           // Número total de nodos
    int numAristas;         // Número total de aristas
    int maxNodoId;          // ID máximo de nodo encontrado
    int capacidadAristas;   // Capacidad actual del arreglo de aristas
    
    // Estructura auxiliar para almacenar aristas temporalmente
    struct Arista {
        int origen;
        int destino;
    };
    
    /**
     * @brief Construye la estructura CSR a partir de lista de aristas
     * @param aristas Arreglo de aristas
     * @param numAristasTemp Número de aristas en el arreglo
     */
    void construirCSR(Arista* aristas, int numAristasTemp);
    
    /**
     * @brief Ordena arreglo de aristas por nodo origen (QuickSort)
     */
    void ordenarAristas(Arista* aristas, int inicio, int fin);
    
    /**
     * @brief Particiona arreglo para QuickSort
     */
    int particionar(Arista* aristas, int inicio, int fin);

public:
    /**
     * @brief Constructor por defecto
     */
    GrafoDisperso();

    /**
     * @brief Destructor - libera memoria dinámica
     */
    ~GrafoDisperso();

    /**
     * @brief Carga datos desde archivo Edge List
     * @param nombreArchivo Ruta al archivo (formato: nodoOrigen nodoDestino)
     * @return true si la carga fue exitosa
     */
    bool cargarDatos(const char* nombreArchivo) override;

    /**
     * @brief Búsqueda en anchura (BFS) implementada nativamente
     * @param nodoInicial Nodo desde donde iniciar
     * @param profundidadMaxima Profundidad máxima de exploración
     * @param tamResultado Referencia donde se guarda el tamaño del resultado
     * @return Arreglo dinámico con nodos visitados (debe liberarse con delete[])
     */
    int* BFS(int nodoInicial, int profundidadMaxima, int& tamResultado) override;

    /**
     * @brief Obtiene el grado de salida de un nodo
     * @param nodo ID del nodo
     * @return Número de aristas salientes
     */
    int obtenerGrado(int nodo) override;

    /**
     * @brief Obtiene vecinos de un nodo
     * @param nodo ID del nodo
     * @param tamVecinos Referencia donde se guarda el número de vecinos
     * @return Arreglo dinámico con IDs de vecinos (debe liberarse con delete[])
     */
    int* getVecinos(int nodo, int& tamVecinos) override;

    /**
     * @brief Obtiene número total de nodos
     */
    int getNumNodos() const override;

    /**
     * @brief Obtiene número total de aristas
     */
    int getNumAristas() const override;

    /**
     * @brief Encuentra el nodo con mayor grado
     * @return ID del nodo con más conexiones
     */
    int getNodoMayorGrado() override;

    /**
     * @brief Obtiene memoria estimada usada por la estructura CSR
     * @return Memoria en MB
     */
    double getMemoriaEstimadaMB() const;
};

#endif // GRAFODISPERSO_H
