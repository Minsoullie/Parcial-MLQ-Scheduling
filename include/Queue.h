#pragma once
#include <deque>
#include <string>
#include <vector>
#include "Process.h"

// Clase base abstracta: representa UNA cola del esquema MLQ.
// Cada subclase encapsula tanto la estructura de datos como el algoritmo
// de planificación que le corresponde (Strategy pattern aplicado a nivel
// de cola). El MLQScheduler nunca sabe "cómo" decide cada cola: sólo le
// pide "ejecuta un tick" y confía en el polimorfismo.
class Queue {
protected:
    int id;             // Identificador de cola (debe coincidir con el campo Q del archivo)
    std::string label;  // Nombre descriptivo, p.ej. "RR(2)" o "STCF"

public:
    Queue(int id, std::string label) : id(id), label(std::move(label)) {}
    virtual ~Queue() = default;

    int getId() const { return id; }
    const std::string& getLabel() const { return label; }

    // Se invoca cuando un proceso llega (AT <= t) y pertenece a esta cola.
    virtual void addProcess(Process* p) = 0;

    // ¿Tiene esta cola algún proceso pendiente (esperando o en ejecución)?
    virtual bool hasPending() const = 0;

    // Ejecuta EXACTAMENTE una unidad de tiempo [t, t+1) sobre el proceso
    // que le corresponde según su propia política. Devuelve el puntero al
    // proceso ejecutado (o nullptr si no había nada que hacer).
    virtual Process* tick(double currentTime) = 0;
};

// ---------------------------------------------------------------------
// Round Robin con quantum configurable.
// ---------------------------------------------------------------------
class RoundRobinQueue : public Queue {
private:
    double quantum;
    std::deque<Process*> ready;   // cola FIFO de procesos en espera
    Process* current = nullptr;   // proceso que tiene la CPU en este momento
    double usedInSlice = 0.0;     // ticks ya consumidos del quantum actual

public:
    RoundRobinQueue(int id, double quantum);

    void addProcess(Process* p) override;
    bool hasPending() const override;
    Process* tick(double currentTime) override;
};

// ---------------------------------------------------------------------
// STCF (Shortest Time-to-Completion First): expropiativo por diseño.
// En cada tick se re-evalúa cuál proceso tiene el menor tiempo restante,
// lo que produce la expropiación automáticamente sin lógica adicional.
// ---------------------------------------------------------------------
class STCFQueue : public Queue {
private:
    std::vector<Process*> pool;   // todos los procesos que han llegado a esta cola

public:
    explicit STCFQueue(int id);

    void addProcess(Process* p) override;
    bool hasPending() const override;
    Process* tick(double currentTime) override;
};
