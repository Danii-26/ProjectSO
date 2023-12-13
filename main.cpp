/*
  Autor1: Daniel Arias Castrillón 2222205-3743
  <daniel.arias.castrillon@correounivalle.edu.co>     
  Autor2: Venus Juliana Paipilla 2177134-3744
  <venus.paipilla@correounivalle.edu.co>
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>

class Proceso {
private:
    int IDProceso;
    int tiempoLlegada;
    int tiempoRafaga;

public:
    Proceso(int pid, int llegada, int rafaga) : IDProceso(pid), tiempoLlegada(llegada), tiempoRafaga(rafaga) {}

    int obtenerIDProceso() const { return IDProceso; }
    int obtenerTiempoLlegada() const { return tiempoLlegada; }
    int obtenerTiempoRafaga() const { return tiempoRafaga; }
    void establecerTiempoLlegada(int llegada) { tiempoLlegada = llegada; }
    void establecerTiempoRafaga(int rafaga) { tiempoRafaga = rafaga; }
};

class Planificador {
public:
    virtual void planificar(std::vector<Proceso>& procesos) = 0;
};

class FCFS : public Planificador {
public:
    void planificar(std::vector<Proceso>& procesos) override {
        std::sort(procesos.begin(), procesos.end(), [](const Proceso& a, const Proceso& b) {
            return a.obtenerTiempoLlegada() < b.obtenerTiempoLlegada();
        });

        double tiempoRetornoTotal = 0;
        double tiempoEsperaTotal = 0;
        double tiempoActual = 0;

        for (const auto& proceso : procesos) {
            tiempoRetornoTotal += (tiempoActual - proceso.obtenerTiempoLlegada() + proceso.obtenerTiempoRafaga());
            tiempoEsperaTotal += (tiempoActual - proceso.obtenerTiempoLlegada());
            tiempoActual += proceso.obtenerTiempoRafaga();
        }

        double tiempoRetornoPromedio = tiempoRetornoTotal / procesos.size();
        double tiempoEsperaPromedio = tiempoEsperaTotal / procesos.size();

        std::cout << "Métricas de la cola FCFS:" << std::endl;
        std::cout << "Tiempo de Retorno Promedio: " << std::fixed << std::setprecision(2) << tiempoRetornoPromedio << std::endl;
        std::cout << "Tiempo de Espera Promedio: " << std::fixed << std::setprecision(2) << tiempoEsperaPromedio << std::endl;
    }
};

class SJN : public Planificador {
public:
    void planificar(std::vector<Proceso>& procesos) override {
        std::sort(procesos.begin(), procesos.end(), [](const Proceso& a, const Proceso& b) {
            return a.obtenerTiempoRafaga() < b.obtenerTiempoRafaga();
        });

        double tiempoRetornoTotal = 0;
        double tiempoEsperaTotal = 0;
        double tiempoActual = 0;

        for (const auto& proceso : procesos) {
            tiempoRetornoTotal += (tiempoActual - proceso.obtenerTiempoLlegada() + proceso.obtenerTiempoRafaga());
            tiempoEsperaTotal += (tiempoActual - proceso.obtenerTiempoLlegada());
            tiempoActual += proceso.obtenerTiempoRafaga();
        }

        double tiempoRetornoPromedio = tiempoRetornoTotal / procesos.size();
        double tiempoEsperaPromedio = tiempoEsperaTotal / procesos.size();

        std::cout << "Métricas de la cola SJN:" << std::endl;
        std::cout << "Tiempo de Retorno Promedio: " << std::fixed << std::setprecision(2) << tiempoRetornoPromedio << std::endl;
        std::cout << "Tiempo de Espera Promedio: " << std::fixed << std::setprecision(2) << tiempoEsperaPromedio << std::endl;
    }
};

class RR : public Planificador {
private:
    int quantum;

public:
    RR(int quantum) : quantum(quantum) {}

    void planificar(std::vector<Proceso>& procesos) override {
        int tiempoActual = 0;
        int tiempoRafagaTotal = 0;

        for (const auto& proceso : procesos) {
            tiempoRafagaTotal += proceso.obtenerTiempoRafaga();
        }

        std::queue<Proceso> colaListos;

        int indice = 0;
        while (indice < procesos.size() || !colaListos.empty()) {
            while (indice < procesos.size() && procesos[indice].obtenerTiempoLlegada() <= tiempoActual) {
                colaListos.push(procesos[indice]);
                ++indice;
            }

            if (colaListos.empty()) {
                tiempoActual = procesos[indice].obtenerTiempoLlegada();
                continue;
            }

            Proceso procesoActual = colaListos.front();
            colaListos.pop();

            int tiempoEjecucion = std::min(quantum, procesoActual.obtenerTiempoRafaga());
            tiempoRafagaTotal -= tiempoEjecucion;
            tiempoActual += tiempoEjecucion;

            procesoActual.establecerTiempoRafaga(procesoActual.obtenerTiempoRafaga() - tiempoEjecucion);

            if (procesoActual.obtenerTiempoRafaga() > 0) {
                colaListos.push(procesoActual);
            }
        }

        double tiempoRetornoPromedio = (tiempoRafagaTotal + procesos.size() * quantum) / procesos.size();
        double tiempoEsperaPromedio = tiempoRetornoPromedio - tiempoRafagaTotal / procesos.size();

        std::cout << "Métricas de la cola RR:" << std::endl;
        std::cout << "Tiempo de Retorno Promedio: " << tiempoRetornoPromedio << std::endl;
        std::cout << "Tiempo de Espera Promedio: " << tiempoEsperaPromedio << std::endl;
    }
};

class MLQ : public Planificador {
public:
    void planificar(std::vector<Proceso>& procesos) override {
        std::vector<Proceso> cola1;
        std::vector<Proceso> cola2;

        for (const auto& proceso : procesos) {
            if (proceso.obtenerTiempoRafaga() <= 10) {
                cola1.push_back(proceso);
            } else {
                cola2.push_back(proceso);
            }
        }

        RR rr1(10); // Round Robin con quantum 10 para cola 1
        FCFS fcfs;  // FCFS para cola 2

        std::cout << "Métricas de la cola MLQ:" << std::endl;
        std::cout << "Cola 1 (Quantum 10):" << std::endl;
        rr1.planificar(cola1); // Llamar a la función planificar de RR directamente con el vector de procesos
        std::cout << "Cola 2 (FCFS):" << std::endl;
        fcfs.planificar(cola2); // Llamar a la función planificar de FCFS directamente con el vector de procesos
    }
};

class MLFQ : public Planificador {
public:
    void planificar(std::vector<Proceso>& procesos) override {
        std::vector<Proceso> cola;

        for (const auto& proceso : procesos) {
            cola.push_back(proceso);
        }

        RR rr1(8); // Round Robin con quantum 8 para cola 1
        RR rr2(16); // Round Robin con quantum 16 para cola 2
        FCFS fcfs;  // FCFS para cola 3

        std::cout << "Métricas de la cola MLFQ:" << std::endl;
        std::cout << "Cola 1 (Quantum 8):" << std::endl;
        rr1.planificar(cola); // Llamar a la función planificar de RR directamente con el vector de procesos
        std::cout << "Cola 2 (Quantum 16):" << std::endl;
        rr2.planificar(cola); // Llamar a la función planificar de RR directamente con el vector de procesos
        std::cout << "Cola 3 (FCFS):" << std::endl;
        fcfs.planificar(cola); // Llamar a la función planificar de FCFS directamente con el vector de procesos
    }
};

void crearProceso(std::vector<Proceso>& procesos) {
    int pid, llegada, rafaga;
    std::cout << "Ingrese ID de Proceso, Tiempo de Llegada, Tiempo de Ráfaga para el Proceso " << procesos.size() + 1 << " (separado por espacios): ";
    std::cin >> pid >> llegada >> rafaga;
    procesos.emplace_back(pid, llegada, rafaga);
    std::cout << "El proceso con ID " << pid << " ha sido creado con éxito." << std::endl;
}

void editarProceso(std::vector<Proceso>& procesos) {
    int pid, llegada, rafaga;
    std::cout << "Ingrese ID de Proceso a editar: ";
    std::cin >> pid;

    auto it = std::find_if(procesos.begin(), procesos.end(), [pid](const Proceso& p) {
        return p.obtenerIDProceso() == pid;
    });

    if (it != procesos.end()) {
        std::cout << "Ingrese nuevo Tiempo de Llegada, Tiempo de Ráfaga (separado por espacios): ";
        std::cin >> llegada >> rafaga;
        it->establecerTiempoLlegada(llegada);
        it->establecerTiempoRafaga(rafaga);
        std::cout << "El proceso con ID " << pid << " ha sido editado con éxito." << std::endl;
    } else {
        std::cout << "ID de Proceso no encontrado." << std::endl;
    }
}

void eliminarProceso(std::vector<Proceso>& procesos) {
    int pid;
    std::cout << "Ingrese ID de Proceso a eliminar: ";
    std::cin >> pid;

    auto tamanoInicial = procesos.size();

    procesos.erase(std::remove_if(procesos.begin(), procesos.end(), [pid](const Proceso& p) {
        return p.obtenerIDProceso() == pid;
    }), procesos.end());

    if (procesos.size() < tamanoInicial) {
        std::cout << "El proceso con ID " << pid << " ha sido eliminado con éxito." << std::endl;
    } else {
        std::cout << "ID de Proceso no encontrado." << std::endl;
    }
}

void calcularMetricas(const std::vector<Proceso>& procesos) {
    double tiempoRetornoTotal = 0;
    double tiempoEsperaTotal = 0;

    for (const auto& proceso : procesos) {
        tiempoRetornoTotal += (proceso.obtenerTiempoRafaga() + proceso.obtenerTiempoLlegada());
        tiempoEsperaTotal += (tiempoRetornoTotal - proceso.obtenerTiempoLlegada() - proceso.obtenerTiempoRafaga());
    }

    double tiempoRetornoPromedio = tiempoRetornoTotal / procesos.size();
    double tiempoEsperaPromedio = tiempoEsperaTotal / procesos.size();

    std::cout << "Métricas Generales:" << std::endl;
    std::cout << "Tiempo Promedio de Retorno: " << tiempoRetornoPromedio << std::endl;
    std::cout << "Tiempo Promedio de Espera: " << tiempoEsperaPromedio << std::endl;
}

void mostrarMenu() {
    std::cout << "\nMenú de Simulación:" << std::endl;
    std::cout << "1. Crear Proceso" << std::endl;
    std::cout << "2. Editar Proceso" << std::endl;
    std::cout << "3. Eliminar Proceso" << std::endl;
    std::cout << "4. Ejecutar FCFS" << std::endl;
    std::cout << "5. Ejecutar SJN" << std::endl;
    std::cout << "6. Ejecutar RR" << std::endl;
    std::cout << "7. Ejecutar MLQ" << std::endl;
    std::cout << "8. Ejecutar MLFQ" << std::endl;
    std::cout << "9. Ver Métricas" << std::endl;
    std::cout << "0. Salir" << std::endl;
    std::cout << "Elige una opción: ";
}

int main() {
    std::vector<Proceso> procesos;
    int opcion;

    do {
        mostrarMenu();
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                crearProceso(procesos);
                break;
            case 2:
                editarProceso(procesos);
                break;
            case 3:
                eliminarProceso(procesos);
                break;
            case 4: {
                if (procesos.empty()) {
                    std::cout << "No existe ningún proceso." << std::endl;
                } else {
                    FCFS fcfs;
                    fcfs.planificar(procesos);
                }
                break;
            }
            case 5: {
                if (procesos.empty()) {
                    std::cout << "No existe ningún proceso." << std::endl;
                } else {
                    SJN sjn;
                    sjn.planificar(procesos);
                }
                break;
            }
            case 6: {
                if (procesos.empty()) {
                    std::cout << "No existe ningún proceso." << std::endl;
                } else {
                    int quantum;
                    std::cout << "Ingresa quantum para RR: ";
                    std::cin >> quantum;
                    RR rr(quantum);
                    rr.planificar(procesos);
                }
                break;
            }
            case 7: {
                if (procesos.empty()) {
                    std::cout << "No existe ningún proceso." << std::endl;
                } else {
                    MLQ mlq;
                    mlq.planificar(procesos);
                }
                break;
            }
            case 8: {
                if (procesos.empty()) {
                    std::cout << "No existe ningún proceso." << std::endl;
                } else {
                    MLFQ mlfq;
                    mlfq.planificar(procesos);
                }
                break;
            }
            case 9:
                if (procesos.empty()) {
                    std::cout << "No existe ningún proceso." << std::endl;
                } else {
                    calcularMetricas(procesos);
                }
                break;
            case 0:
                std::cout << "Saliendo..." << std::endl;
                break;
            default:
                std::cout << "Opción no válida. Por favor intente nuevamente." << std::endl;
        }
    } while (opcion != 0);

    return 0;
}
