#include "MLQScheduler.h"
#include <algorithm>
#include <stdexcept>

void MLQScheduler::addQueue(std::unique_ptr<Queue> q) {
    queues.push_back(std::move(q));
}

void MLQScheduler::loadProcesses(std::vector<Process>& procs) {
    processes.clear();
    for (auto& p : procs) processes.push_back(&p);
}

int MLQScheduler::queueIndexForId(int qid) const {
    for (size_t i = 0; i < queues.size(); ++i) {
        if (queues[i]->getId() == qid) return static_cast<int>(i);
    }
    throw std::runtime_error("No hay una cola configurada para Q=" + std::to_string(qid));
}

void MLQScheduler::run() {
    // Procesos ordenados por tiempo de llegada, para saber cuándo insertarlos.
    std::vector<Process*> byArrival = processes;
    std::stable_sort(byArrival.begin(), byArrival.end(),
                      [](Process* a, Process* b) { return a->arrivalTime < b->arrivalTime; });

    size_t nextArrival = 0;
    size_t total = byArrival.size();
    size_t completed = 0;
    double t = total > 0 ? byArrival.front()->arrivalTime : 0.0;

    while (completed < total) {
        // 1) Insertar en su cola todos los procesos que ya llegaron.
        while (nextArrival < byArrival.size() && byArrival[nextArrival]->arrivalTime <= t) {
            Process* p = byArrival[nextArrival];
            queues[queueIndexForId(p->queueId)]->addProcess(p);
            nextArrival++;
        }

        // 2) Elegir la cola de mayor prioridad (primera en el vector) con trabajo pendiente.
        Queue* active = nullptr;
        for (auto& q : queues) {
            if (q->hasPending()) { active = q.get(); break; }
        }

        // 3) Si ninguna cola tiene trabajo, la CPU está inactiva: saltar al próximo arribo.
        if (active == nullptr) {
            if (nextArrival < byArrival.size()) {
                t = byArrival[nextArrival]->arrivalTime;
                continue;
            }
            break; // no debería ocurrir si completed < total
        }

        // 4) Ejecutar un tick en la cola activa.
        Process* executed = active->tick(t);

        // 5) Registrar traza (para Gantt) fusionando con el intervalo anterior si es el mismo proceso.
        if (executed != nullptr) {
            if (!trace.empty() && std::get<2>(trace.back()) == executed->label &&
                std::get<1>(trace.back()) == t) {
                std::get<1>(trace.back()) = t + 1.0;
            } else {
                trace.emplace_back(t, t + 1.0, executed->label, active->getLabel());
            }
            if (executed->isFinished()) completed++;
        }

        t += 1.0;
    }
}
