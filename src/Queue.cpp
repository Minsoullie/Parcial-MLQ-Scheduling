#include "Queue.h"

// ============================== RoundRobinQueue ==============================

RoundRobinQueue::RoundRobinQueue(int id, double quantum)
    : Queue(id, "RR(" + std::to_string((int)quantum) + ")"), quantum(quantum) {}

void RoundRobinQueue::addProcess(Process* p) {
    ready.push_back(p);
}

bool RoundRobinQueue::hasPending() const {
    return current != nullptr || !ready.empty();
}

Process* RoundRobinQueue::tick(double currentTime) {
    // Si no hay proceso "cargado" en la CPU, tomar el siguiente de la fila.
    if (current == nullptr) {
        if (ready.empty()) return nullptr;
        current = ready.front();
        ready.pop_front();
        usedInSlice = 0.0;
    }

    // Registrar tiempo de respuesta la primera vez que corre.
    if (!current->hasStarted) {
        current->hasStarted = true;
        current->startTime = currentTime;
        current->responseTime = currentTime - current->arrivalTime;
    }

    // Ejecutar 1 unidad de tiempo.
    current->remainingTime -= 1.0;
    usedInSlice += 1.0;
    double now = currentTime + 1.0;
    Process* executed = current;

    if (current->isFinished()) {
        current->completionTime = now;
        current->turnaroundTime = current->completionTime - current->arrivalTime;
        current->waitingTime = current->turnaroundTime - current->burstTime;
        current = nullptr;
        usedInSlice = 0.0;
    } else if (usedInSlice >= quantum) {
        // Se agotó el quantum sin terminar: vuelve al final de la fila.
        ready.push_back(current);
        current = nullptr;
        usedInSlice = 0.0;
    }
    // Si no terminó su quantum ni el proceso (fue expropiado por una cola de
    // mayor prioridad, es decir, tick() de esta cola simplemente no fue
    // llamado en esos instantes), "current" y "usedInSlice" quedan intactos
    // y el proceso retomará su MISMO quantum en el siguiente turno.

    return executed;
}

// ================================= STCFQueue =================================

STCFQueue::STCFQueue(int id) : Queue(id, "STCF") {}

void STCFQueue::addProcess(Process* p) {
    pool.push_back(p);
}

bool STCFQueue::hasPending() const {
    for (Process* p : pool) {
        if (!p->isFinished()) return true;
    }
    return false;
}

Process* STCFQueue::tick(double currentTime) {
    // Buscar, entre los procesos ya llegados y no terminados, el de menor
    // tiempo restante. Empates: el que llegó primero; si persiste el
    // empate, el orden original del archivo de entrada (estable por índice).
    Process* best = nullptr;
    for (Process* p : pool) {
        if (p->isFinished()) continue;
        if (best == nullptr ||
            p->remainingTime < best->remainingTime ||
            (p->remainingTime == best->remainingTime && p->arrivalTime < best->arrivalTime)) {
            best = p;
        }
    }
    if (best == nullptr) return nullptr;

    if (!best->hasStarted) {
        best->hasStarted = true;
        best->startTime = currentTime;
        best->responseTime = currentTime - best->arrivalTime;
    }

    best->remainingTime -= 1.0;
    double now = currentTime + 1.0;

    if (best->isFinished()) {
        best->completionTime = now;
        best->turnaroundTime = best->completionTime - best->arrivalTime;
        best->waitingTime = best->turnaroundTime - best->burstTime;
    }

    return best;
}
