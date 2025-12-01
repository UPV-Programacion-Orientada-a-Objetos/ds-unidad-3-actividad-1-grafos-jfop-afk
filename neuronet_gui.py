"""
NeuroNet: Sistema de Análisis y Visualización de Grafos Masivos
Interfaz Gráfica de Usuario (GUI) con Tkinter

Este módulo proporciona la interfaz visual para interactuar con el motor
de grafos C++ a través del wrapper Cython.
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox, scrolledtext
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import networkx as nx
import time

try:
    from grafo_wrapper import PyGrafoDisperso
except ImportError:
    print("ERROR: No se pudo importar el módulo grafo_wrapper.")
    print("Por favor, compile primero el módulo con: python setup.py build_ext --inplace")
    exit(1)


class NeuroNetGUI:
    """Interfaz gráfica principal de NeuroNet"""
    
    def __init__(self, root):
        self.root = root
        self.root.title("NeuroNet: Análisis de Redes Masivas")
        self.root.geometry("1200x800")
        
        # Instancia del grafo C++
        self.grafo = None
        
        # Variables de control
        self.archivo_cargado = False
        
        # Configurar interfaz
        self.crear_widgets()
        
    def crear_widgets(self):
        """Crea todos los widgets de la interfaz"""
        
        # ===== PANEL SUPERIOR: Control y Métricas =====
        panel_superior = ttk.Frame(self.root, padding="10")
        panel_superior.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N))
        
        # Botón de carga
        ttk.Button(
            panel_superior,
            text="📁 Cargar Dataset",
            command=self.cargar_dataset
        ).grid(row=0, column=0, padx=5, pady=5)
        
        # Etiqueta de archivo
        self.label_archivo = ttk.Label(panel_superior, text="Ningún archivo cargado")
        self.label_archivo.grid(row=0, column=1, padx=10, pady=5, sticky=tk.W)
        
        # Frame de métricas
        frame_metricas = ttk.LabelFrame(panel_superior, text="Métricas del Grafo", padding="10")
        frame_metricas.grid(row=1, column=0, columnspan=3, padx=5, pady=10, sticky=(tk.W, tk.E))
        
        self.label_nodos = ttk.Label(frame_metricas, text="Nodos: -")
        self.label_nodos.grid(row=0, column=0, padx=10, sticky=tk.W)
        
        self.label_aristas = ttk.Label(frame_metricas, text="Aristas: -")
        self.label_aristas.grid(row=0, column=1, padx=10, sticky=tk.W)
        
        self.label_memoria = ttk.Label(frame_metricas, text="Memoria: -")
        self.label_memoria.grid(row=0, column=2, padx=10, sticky=tk.W)
        
        self.label_tiempo = ttk.Label(frame_metricas, text="Tiempo de carga: -")
        self.label_tiempo.grid(row=0, column=3, padx=10, sticky=tk.W)
        
        self.label_mayor_grado = ttk.Label(frame_metricas, text="Nodo mayor grado: -")
        self.label_mayor_grado.grid(row=1, column=0, columnspan=2, padx=10, pady=5, sticky=tk.W)
        
        # ===== PANEL CENTRAL: Controles de Análisis =====
        panel_central = ttk.LabelFrame(self.root, text="Análisis BFS", padding="10")
        panel_central.grid(row=1, column=0, padx=10, pady=10, sticky=(tk.W, tk.E))
        
        ttk.Label(panel_central, text="Nodo Inicial:").grid(row=0, column=0, padx=5, pady=5, sticky=tk.W)
        self.entry_nodo_inicial = ttk.Entry(panel_central, width=15)
        self.entry_nodo_inicial.grid(row=0, column=1, padx=5, pady=5)
        self.entry_nodo_inicial.insert(0, "0")
        
        ttk.Label(panel_central, text="Profundidad Máxima:").grid(row=0, column=2, padx=5, pady=5, sticky=tk.W)
        self.entry_profundidad = ttk.Entry(panel_central, width=15)
        self.entry_profundidad.grid(row=0, column=3, padx=5, pady=5)
        self.entry_profundidad.insert(0, "2")
        
        ttk.Button(
            panel_central,
            text="🔍 Ejecutar BFS",
            command=self.ejecutar_bfs
        ).grid(row=0, column=4, padx=10, pady=5)
        
        # ===== PANEL INFERIOR: Visualización y Logs =====
        panel_inferior = ttk.Frame(self.root)
        panel_inferior.grid(row=2, column=0, padx=10, pady=10, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Configurar peso de filas/columnas para expansión
        self.root.rowconfigure(2, weight=1)
        self.root.columnconfigure(0, weight=1)
        panel_inferior.rowconfigure(0, weight=1)
        panel_inferior.columnconfigure(0, weight=1)
        panel_inferior.columnconfigure(1, weight=1)
        
        # Canvas de visualización
        frame_canvas = ttk.LabelFrame(panel_inferior, text="Visualización del Subgrafo")
        frame_canvas.grid(row=0, column=0, padx=5, pady=5, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        self.fig, self.ax = plt.subplots(figsize=(6, 5))
        self.canvas = FigureCanvasTkAgg(self.fig, master=frame_canvas)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # Área de logs
        frame_logs = ttk.LabelFrame(panel_inferior, text="Logs del Sistema")
        frame_logs.grid(row=0, column=1, padx=5, pady=5, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        self.text_logs = scrolledtext.ScrolledText(frame_logs, width=50, height=20, state='disabled')
        self.text_logs.pack(fill=tk.BOTH, expand=True)
        
        self.agregar_log("Sistema NeuroNet iniciado.")
        self.agregar_log("Esperando carga de dataset...")
    
    def agregar_log(self, mensaje):
        """Agrega un mensaje al área de logs"""
        self.text_logs.config(state='normal')
        self.text_logs.insert(tk.END, f"[{time.strftime('%H:%M:%S')}] {mensaje}\n")
        self.text_logs.see(tk.END)
        self.text_logs.config(state='disabled')
    
    def cargar_dataset(self):
        """Carga un dataset desde archivo"""
        archivo = filedialog.askopenfilename(
            title="Seleccionar Dataset",
            filetypes=[("Archivos de texto", "*.txt"), ("Todos los archivos", "*.*")]
        )
        
        if not archivo:
            return
        
        self.agregar_log(f"Cargando dataset: {archivo}")
        
        try:
            # Crear instancia del grafo C++
            self.grafo = PyGrafoDisperso()
            
            # Medir tiempo de carga
            inicio = time.time()
            exito = self.grafo.cargar_datos(archivo)
            fin = time.time()
            
            if not exito:
                messagebox.showerror("Error", "No se pudo cargar el dataset")
                self.agregar_log("ERROR: Fallo en la carga del dataset")
                return
            
            tiempo_carga = (fin - inicio) * 1000  # Convertir a ms
            
            # Actualizar métricas
            num_nodos = self.grafo.get_num_nodos()
            num_aristas = self.grafo.get_num_aristas()
            memoria_mb = self.grafo.get_memoria_estimada_mb()
            
            self.label_archivo.config(text=f"Archivo: {archivo.split('/')[-1]}")
            self.label_nodos.config(text=f"Nodos: {num_nodos:,}")
            self.label_aristas.config(text=f"Aristas: {num_aristas:,}")
            self.label_memoria.config(text=f"Memoria: {memoria_mb:.2f} MB")
            self.label_tiempo.config(text=f"Tiempo de carga: {tiempo_carga:.2f} ms")
            
            self.archivo_cargado = True
            self.agregar_log(f"Dataset cargado exitosamente: {num_nodos:,} nodos, {num_aristas:,} aristas")
            
            # Calcular nodo con mayor grado
            self.agregar_log("Calculando nodo con mayor grado...")
            nodo_mayor = self.grafo.get_nodo_mayor_grado()
            grado_mayor = self.grafo.obtener_grado(nodo_mayor)
            self.label_mayor_grado.config(text=f"Nodo mayor grado: {nodo_mayor} (grado: {grado_mayor})")
            self.agregar_log(f"Nodo con mayor grado: {nodo_mayor} (grado: {grado_mayor})")
            
        except Exception as e:
            messagebox.showerror("Error", f"Error al cargar dataset: {str(e)}")
            self.agregar_log(f"ERROR: {str(e)}")
    
    def ejecutar_bfs(self):
        """Ejecuta BFS y visualiza el resultado"""
        if not self.archivo_cargado or self.grafo is None:
            messagebox.showwarning("Advertencia", "Primero debe cargar un dataset")
            return
        
        try:
            nodo_inicial = int(self.entry_nodo_inicial.get())
            profundidad = int(self.entry_profundidad.get())
            
            if nodo_inicial < 0 or nodo_inicial >= self.grafo.get_num_nodos():
                messagebox.showerror("Error", f"Nodo inicial fuera de rango (0-{self.grafo.get_num_nodos()-1})")
                return
            
            if profundidad < 1:
                messagebox.showerror("Error", "La profundidad debe ser al menos 1")
                return
            
            self.agregar_log(f"Ejecutando BFS desde nodo {nodo_inicial}, profundidad {profundidad}...")
            
            # Ejecutar BFS
            inicio = time.time()
            nodos_visitados = self.grafo.bfs(nodo_inicial, profundidad)
            fin = time.time()
            
            tiempo_bfs = (fin - inicio) * 1000
            self.agregar_log(f"BFS completado: {len(nodos_visitados)} nodos encontrados en {tiempo_bfs:.3f} ms")
            
            # Visualizar subgrafo
            self.visualizar_subgrafo(nodos_visitados, nodo_inicial)
            
        except ValueError:
            messagebox.showerror("Error", "Por favor ingrese valores numéricos válidos")
        except Exception as e:
            messagebox.showerror("Error", f"Error al ejecutar BFS: {str(e)}")
            self.agregar_log(f"ERROR: {str(e)}")
    
    def visualizar_subgrafo(self, nodos_visitados, nodo_inicial):
        """Visualiza el subgrafo resultante del BFS"""
        self.agregar_log("Generando visualización...")
        
        # Crear grafo NetworkX (solo para visualización)
        G = nx.DiGraph()
        
        # Agregar nodos
        G.add_nodes_from(nodos_visitados)
        
        # Agregar aristas (obtener vecinos de cada nodo visitado)
        for nodo in nodos_visitados:
            vecinos = self.grafo.get_vecinos(nodo)
            for vecino in vecinos:
                if vecino in nodos_visitados:
                    G.add_edge(nodo, vecino)
        
        # Limpiar canvas anterior
        self.ax.clear()
        
        # Configurar layout
        if len(nodos_visitados) > 100:
            # Para grafos grandes, usar layout simple
            pos = nx.spring_layout(G, k=0.5, iterations=20)
        else:
            # Para grafos pequeños, usar layout más detallado
            pos = nx.spring_layout(G, k=1, iterations=50)
        
        # Colores: nodo inicial en rojo, otros en azul
        node_colors = ['red' if nodo == nodo_inicial else 'lightblue' for nodo in G.nodes()]
        
        # Dibujar grafo
        nx.draw(
            G, pos, ax=self.ax,
            node_color=node_colors,
            node_size=300,
            with_labels=True,
            font_size=8,
            font_weight='bold',
            arrows=True,
            edge_color='gray',
            alpha=0.7
        )
        
        self.ax.set_title(f"Subgrafo BFS desde nodo {nodo_inicial}\n{len(nodos_visitados)} nodos, {G.number_of_edges()} aristas")
        self.canvas.draw()
        
        self.agregar_log("Visualización completada")


def main():
    """Función principal"""
    root = tk.Tk()
    app = NeuroNetGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
