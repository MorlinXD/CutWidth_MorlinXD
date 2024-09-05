#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <limits>

// Estructura del Grafo
struct Grafo {
    int numNodos;
    int numAristas;
    std::vector<std::vector<int>> adjList;

    // Constructor
    Grafo(int nodos, int aristas) : numNodos(nodos), numAristas(aristas) {
        adjList.resize(nodos + 1); // Crear lista de adyacencia para nodos numerados de 1 a nodos
    }

    // Método para agregar una arista
    void addArista(int u, int v) {
        adjList[u].push_back(v);
        adjList[v].push_back(u); // Si el grafo es no dirigido
    }

    // Método para imprimir el grafo
    void imprimirGrafo() const {
        std::cout << "Grafo:" << std::endl;
        for (int i = 1; i <= numNodos; ++i) {
            std::cout << "Nodo " << i << ":";
            for (int vecino : adjList[i]) {
                std::cout << " " << vecino;
            }
            std::cout << std::endl;
        }
    }

    // Método para calcular el cutwidth de una permutación dada
    int calcularCutwidth(const std::vector<int>& perm) const {
        std::vector<int> posicion(numNodos + 1);
        for (int i = 0; i < perm.size(); ++i) {
            posicion[perm[i]] = i; // Guardamos la posición del nodo en la permutación
        }

        int maxCutwidth = 0;
        for (int i = 1; i <= numNodos; ++i) {
            int cutwidth = 0;
            for (int vecino : adjList[i]) {
                if (posicion[i] < posicion[vecino]) {
                    cutwidth++; // Esta arista cruza el corte entre los dos nodos
                }
            }
            maxCutwidth = std::max(maxCutwidth, cutwidth);
        }
        return maxCutwidth;
    }

    // Método para generar y evaluar permutaciones
    void evaluarPermutaciones(int numPermutaciones, int numMuestras) const {
        std::vector<int> nodos(numNodos);
        for (int i = 0; i < numNodos; ++i) {
            nodos[i] = i + 1;
        }

        std::random_device rd;
        std::mt19937 g(rd());

        std::vector<std::vector<int>> mejoresPermutacionesMuestras;
        
        for (int muestra = 1; muestra <= numMuestras; ++muestra) {
            std::vector<int> mejorPermutacionMuestra;
            int mejorCutwidthMuestra = std::numeric_limits<int>::max();

            std::cout << "\n=============================" << std::endl;
            std::cout << "Muestra " << muestra << ":\n";
            std::cout << "=============================" << std::endl;

            // Evaluar un lote de permutaciones
            for (int p = 1; p <= numPermutaciones; ++p) {
                std::shuffle(nodos.begin(), nodos.end(), g); // Generar una permutación aleatoria
                int cutwidth = calcularCutwidth(nodos);

                // Mostrar la permutación actual y su cutwidth
                std::cout << "Permutacion " << p << ": ";
                for (int nodo : nodos) {
                    std::cout << nodo << " ";
                }
                std::cout << " | Cutwidth: " << cutwidth << std::endl;

                // Guardar la mejor permutación de la muestra
                if (cutwidth < mejorCutwidthMuestra) {
                    mejorCutwidthMuestra = cutwidth;
                    mejorPermutacionMuestra = nodos;
                }
            }

            // Mostrar la mejor permutación de la muestra
            std::cout << "\nMejor permutacion de la muestra " << muestra << ": ";
            for (int nodo : mejorPermutacionMuestra) {
                std::cout << nodo << " ";
            }
            std::cout << " | Cutwidth de la muestra: " << mejorCutwidthMuestra << "\n";

            // Guardar la mejor permutación de esta muestra para el cálculo final
            mejoresPermutacionesMuestras.push_back(mejorPermutacionMuestra);

            // Separador visual al final de cada muestra
            std::cout << "\n-----------------------------\n";
        }

        // Mostrar todas las mejores permutaciones de cada muestra
        std::cout << "\n===== Mejores Permutaciones de Cada Muestra =====" << std::endl;
        for (int i = 0; i < mejoresPermutacionesMuestras.size(); ++i) {
            std::cout << "Muestra " << (i + 1) << ": ";
            for (int nodo : mejoresPermutacionesMuestras[i]) {
                std::cout << nodo << " ";
            }
            std::cout << std::endl;
        }

        // Calcular el cutwidth final basado en las mejores permutaciones de todas las muestras
        std::vector<int> mejorPermutacionGlobal;
        int mejorCutwidthGlobal = std::numeric_limits<int>::max();

        for (const auto& permutacionMuestra : mejoresPermutacionesMuestras) {
            int cutwidth = calcularCutwidth(permutacionMuestra);
            if (cutwidth < mejorCutwidthGlobal) {
                mejorCutwidthGlobal = cutwidth;
                mejorPermutacionGlobal = permutacionMuestra;
            }
        }

        // Mostrar la mejor permutación global
        std::cout << "\n*****************************" << std::endl;
        std::cout << "Mejor permutacion global (menor cutwidth): ";
        for (int nodo : mejorPermutacionGlobal) {
            std::cout << nodo << " ";
        }
        std::cout << "\nCutwidth global minimo (entre las mejores de cada muestra): " << mejorCutwidthGlobal << std::endl;
        std::cout << "*****************************" << std::endl;
    }

    // Método para generar un archivo DOT para graficar el grafo
    void generarArchivoDOT(const std::string& filename) const {
        std::ofstream dotFile(filename);
        if (!dotFile.is_open()) {
            std::cerr << "Error al abrir el archivo para escritura." << std::endl;
            return;
        }

        dotFile << "graph Grafo {" << std::endl;
        for (int i = 1; i <= numNodos; ++i) {
            for (int vecino : adjList[i]) {
                if (i < vecino) {
                    dotFile << "    " << i << " -- " << vecino << ";" << std::endl;
                }
            }
        }
        dotFile << "}" << std::endl;

        dotFile.close();
        std::cout << "Archivo DOT generado: " << filename << std::endl;
    }
};

// Función para cargar un grafo desde un archivo
Grafo CargarArchivo(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    int numNodos, numAristas;

    if (file.is_open()) {
        std::getline(file, line);
        std::istringstream iss(line);
        iss >> numNodos >> numAristas; // Leer el número de nodos y aristas

        Grafo g(numNodos, numAristas);

        while (std::getline(file, line)) {
            int u, v;
            std::istringstream iss(line);
            iss >> u >> v;
            g.addArista(u, v);
        }

        file.close();
        std::cout << "Grafo cargado correctamente." << std::endl;
        return g;
    } else {
        std::cerr << "Error al abrir el archivo." << std::endl;
        exit(1);
    }
}

int main() {
    // Cargar el grafo desde el archivo
    Grafo g = CargarArchivo("C:/Users/jeanm/Downloads/GRAPH.txt");

    // Imprimir el grafo
    g.imprimirGrafo();

    // Solicitar el número de permutaciones y muestras
    int numPermutaciones, numMuestras;
    std::cout << "Ingrese el numero de permutaciones por muestra: ";
    std::cin >> numPermutaciones;
    std::cout << "Ingrese el numero de muestras: ";
    std::cin >> numMuestras;

    // Evaluar las permutaciones y seleccionar la mejor
    g.evaluarPermutaciones(numPermutaciones, numMuestras);

    return 0;
}

