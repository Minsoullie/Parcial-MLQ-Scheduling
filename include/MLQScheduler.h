#pragma once
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "Process.h"
#include "Queue.h"

// Orquesta la simulación completa: recibe las colas ya configuradas
// (orden = prioridad, la primera es la de mayor prioridad), reparte los
// procesos según su campo Q a medida que "llegan", y avanza el reloj
// tick a tick preguntando siempre a la cola de mayor prioridad con
// trabajo pendiente qué proceso debe ejecutarse.
class MLQScheduler {
private:
    std::vector<std::unique_ptr<Queue>> queues;
    std::vector<Process*> processes;   // no-owning: los dueños son quienes llaman a loadProcesses
    // Traza de ejecución (para Gantt / depuración): [inicio, fin, etiqueta, cola]
    std::vector<std::tuple<double, double, std::string, std::string>> trace;

    int queueIndexForId(int qid) const;

public:
    void addQueue(std::unique_ptr<Queue> q);
    void loadProcesses(std::vector<Process>& procs);
    void run();

    const std::vector<std::tuple<double, double, std::string, std::string>>& getTrace() const {
        return trace;
    }
};
