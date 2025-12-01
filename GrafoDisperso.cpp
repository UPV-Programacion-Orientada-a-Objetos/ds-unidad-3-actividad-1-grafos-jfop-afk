#include "GrafoDisperso.h"
#include <iostream>
#include <fstream>

using namespace std;

// Constructor
GrafoDisperso::GrafoDisperso() : row_ptr(nullptr), col_indices(nullptr), 
                                  numNodos(0), numAristas(0), maxNodoId(0), capacidadAristas(0) {
    cout << "[C++ Core] Inicializando GrafoDisperso..." << endl;
}

// Destructor
GrafoDisperso::~GrafoDisperso() {
    cout << "[C++ Core] Liberando memoria de GrafoDisperso..." << endl;
    
    if (row_ptr != nullptr) {
        delete[] row_ptr;
        row_ptr = nullptr;
    }
    
    if (col_indices != nullptr) {
        delete[] col_indices;
        col_indices = nullptr;
    }
}

// Carga datos desde archivo Edge List
bool GrafoDisperso::cargarDatos(const char* nombreArchivo) {
    cout << "[C++ Core] Cargando dataset '" << nombreArchivo << "'..." << endl;
    
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "[C++ Core] Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        return false;
    }

    // Arreglo dinámico temporal para aristas
    capacidadAristas = 1000; // Capacidad inicial
    Arista* aristas = new Arista[capacidadAristas];
    int numAristasTemp = 0;
    
    int origen, destino;
    char linea[256];
    
    // Leer archivo línea por línea
    while (archivo.getline(linea, 256)) {
        // Ignorar líneas vacías o comentarios
        if (linea[0] == '\0' || linea[0] == '#') {
            continue;
        }
        
        // Parsear manualmente la línea
        int i = 0;
        origen = 0;
        destino = 0;
        
        // Leer primer número (origen)
        while (linea[i] == ' ' || linea[i] == '\t') i++; // Saltar espacios
        while (linea[i] >= '0' && linea[i] <= '9') {
            origen = origen * 10 + (linea[i] - '0');
            i++;
        }
        
        // Leer segundo número (destino)
        while (linea[i] == ' ' || linea[i] == '\t') i++; // Saltar espacios
        while (linea[i] >= '0' && linea[i] <= '9') {
            destino = destino * 10 + (linea[i] - '0');
            i++;
        }
        
        // Expandir arreglo si es necesario
        if (numAristasTemp >= capacidadAristas) {
            capacidadAristas *= 2;
            Arista* nuevoArreglo = new Arista[capacidadAristas];
            
            // Copiar datos al nuevo arreglo
            for (int j = 0; j < numAristasTemp; j++) {
                nuevoArreglo[j] = aristas[j];
            }
            
            delete[] aristas;
            aristas = nuevoArreglo;
        }
        
        // Agregar arista
        aristas[numAristasTemp].origen = origen;
        aristas[numAristasTemp].destino = destino;
        numAristasTemp++;
        
        // Actualizar maxNodoId
        if (origen > maxNodoId) maxNodoId = origen;
        if (destino > maxNodoId) maxNodoId = destino;
    }
    
    archivo.close();
    
    numAristas = numAristasTemp;
    numNodos = maxNodoId + 1; // Asumiendo IDs desde 0
    
    cout << "[C++ Core] Carga completa. Nodos: " << numNodos 
         << " | Aristas: " << numAristas << endl;
    
    // Construir estructura CSR
    construirCSR(aristas, numAristasTemp);
    
    // Liberar arreglo temporal
    delete[] aristas;
    
    cout << "[C++ Core] Estructura CSR construida. Memoria estimada: " 
         << getMemoriaEstimadaMB() << " MB." << endl;
    
    return true;
}

// Particiona arreglo para QuickSort
int GrafoDisperso::particionar(Arista* aristas, int inicio, int fin) {
    int pivote = aristas[fin].origen;
    int i = inicio - 1;
    
    for (int j = inicio; j < fin; j++) {
        if (aristas[j].origen <= pivote) {
            i++;
            // Intercambiar aristas[i] y aristas[j]
            Arista temp = aristas[i];
            aristas[i] = aristas[j];
            aristas[j] = temp;
        }
    }
    
    // Intercambiar aristas[i+1] y aristas[fin]
    Arista temp = aristas[i + 1];
    aristas[i + 1] = aristas[fin];
    aristas[fin] = temp;
    
    return i + 1;
}

// Ordena arreglo de aristas por nodo origen (QuickSort)
void GrafoDisperso::ordenarAristas(Arista* aristas, int inicio, int fin) {
    if (inicio < fin) {
        int pi = particionar(aristas, inicio, fin);
        ordenarAristas(aristas, inicio, pi - 1);
        ordenarAristas(aristas, pi + 1, fin);
    }
}

// Construye la estructura CSR a partir de lista de aristas
void GrafoDisperso::construirCSR(Arista* aristas, int numAristasTemp) {
    // Ordenar aristas por nodo origen
    ordenarAristas(aristas, 0, numAristasTemp - 1);
    
    // Inicializar row_ptr
    row_ptr = new int[numNodos + 1];
    for (int i = 0; i <= numNodos; i++) {
        row_ptr[i] = 0;
    }
    
    // Inicializar col_indices
    col_indices = new int[numAristas];
    
    // Contar aristas por nodo origen
    for (int i = 0; i < numAristasTemp; i++) {
        row_ptr[aristas[i].origen + 1]++;
    }
    
    // Acumular para obtener punteros de inicio
    for (int i = 1; i <= numNodos; i++) {
        row_ptr[i] += row_ptr[i - 1];
    }
    
    // Llenar col_indices
    for (int i = 0; i < numAristasTemp; i++) {
        col_indices[i] = aristas[i].destino;
    }
}

// Cola manual para BFS
struct Nodo {
    int valor;
    int profundidad;
    Nodo* siguiente;
};

class Cola {
private:
    Nodo* frente;
    Nodo* final;
    
public:
    Cola() : frente(nullptr), final(nullptr) {}
    
    ~Cola() {
        while (!estaVacia()) {
            desencolar();
        }
    }
    
    void encolar(int valor, int profundidad) {
        Nodo* nuevo = new Nodo;
        nuevo->valor = valor;
        nuevo->profundidad = profundidad;
        nuevo->siguiente = nullptr;
        
        if (final == nullptr) {
            frente = final = nuevo;
        } else {
            final->siguiente = nuevo;
            final = nuevo;
        }
    }
    
    void desencolar() {
        if (frente == nullptr) return;
        
        Nodo* temp = frente;
        frente = frente->siguiente;
        
        if (frente == nullptr) {
            final = nullptr;
        }
        
        delete temp;
    }
    
    int getFrenteValor() const {
        return frente->valor;
    }
    
    int getFrenteProfundidad() const {
        return frente->profundidad;
    }
    
    bool estaVacia() const {
        return frente == nullptr;
    }
};

// BFS nativo (sin librerías externas)
int* GrafoDisperso::BFS(int nodoInicial, int profundidadMaxima, int& tamResultado) {
    cout << "[C++ Core] Ejecutando BFS nativo desde nodo " << nodoInicial 
         << ", profundidad máxima: " << profundidadMaxima << "..." << endl;
    
    tamResultado = 0;
    
    // Validar nodo inicial
    if (nodoInicial < 0 || nodoInicial >= numNodos) {
        cerr << "[C++ Core] Error: Nodo inicial fuera de rango" << endl;
        return nullptr;
    }
    
    // Arreglo de visitados (inicializado en false = 0)
    bool* visitados = new bool[numNodos];
    for (int i = 0; i < numNodos; i++) {
        visitados[i] = false;
    }
    
    // Arreglo temporal para resultado (tamaño máximo = numNodos)
    int* resultadoTemp = new int[numNodos];
    
    // Cola para BFS
    Cola cola;
    cola.encolar(nodoInicial, 0);
    visitados[nodoInicial] = true;
    
    while (!cola.estaVacia()) {
        int nodoActual = cola.getFrenteValor();
        int profundidad = cola.getFrenteProfundidad();
        cola.desencolar();
        
        resultadoTemp[tamResultado++] = nodoActual;
        
        // No explorar más allá de la profundidad máxima
        if (profundidad >= profundidadMaxima) {
            continue;
        }
        
        // Obtener vecinos usando CSR
        int inicio_vecinos = row_ptr[nodoActual];
        int fin_vecinos = row_ptr[nodoActual + 1];
        
        for (int i = inicio_vecinos; i < fin_vecinos; i++) {
            int vecino = col_indices[i];
            
            if (!visitados[vecino]) {
                visitados[vecino] = true;
                cola.encolar(vecino, profundidad + 1);
            }
        }
    }
    
    // Crear arreglo del tamaño exacto
    int* resultado = new int[tamResultado];
    for (int i = 0; i < tamResultado; i++) {
        resultado[i] = resultadoTemp[i];
    }
    
    // Liberar memoria temporal
    delete[] visitados;
    delete[] resultadoTemp;
    
    cout << "[C++ Core] Nodos encontrados: " << tamResultado << "." << endl;
    
    return resultado;
}

// Obtiene el grado de un nodo
int GrafoDisperso::obtenerGrado(int nodo) {
    if (nodo < 0 || nodo >= numNodos) {
        return 0;
    }
    
    return row_ptr[nodo + 1] - row_ptr[nodo];
}

// Obtiene vecinos de un nodo
int* GrafoDisperso::getVecinos(int nodo, int& tamVecinos) {
    tamVecinos = 0;
    
    if (nodo < 0 || nodo >= numNodos) {
        return nullptr;
    }
    
    int inicio = row_ptr[nodo];
    int fin = row_ptr[nodo + 1];
    tamVecinos = fin - inicio;
    
    if (tamVecinos == 0) {
        return nullptr;
    }
    
    int* vecinos = new int[tamVecinos];
    for (int i = 0; i < tamVecinos; i++) {
        vecinos[i] = col_indices[inicio + i];
    }
    
    return vecinos;
}

// Obtiene número de nodos
int GrafoDisperso::getNumNodos() const {
    return numNodos;
}

// Obtiene número de aristas
int GrafoDisperso::getNumAristas() const {
    return numAristas;
}

// Encuentra el nodo con mayor grado
int GrafoDisperso::getNodoMayorGrado() {
    int nodoMax = -1;
    int gradoMax = 0;
    
    for (int i = 0; i < numNodos; i++) {
        int grado = obtenerGrado(i);
        if (grado > gradoMax) {
            gradoMax = grado;
            nodoMax = i;
        }
    }
    
    cout << "[C++ Core] Nodo con mayor grado: " << nodoMax 
         << " (grado: " << gradoMax << ")" << endl;
    
    return nodoMax;
}

// Calcula memoria estimada en MB
double GrafoDisperso::getMemoriaEstimadaMB() const {
    long long bytes = 0;
    
    // row_ptr: (numNodos + 1) * sizeof(int)
    bytes += (numNodos + 1) * sizeof(int);
    
    // col_indices: numAristas * sizeof(int)
    bytes += numAristas * sizeof(int);
    
    return bytes / (1024.0 * 1024.0);
}
