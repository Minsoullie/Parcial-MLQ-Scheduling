#pragma once
#include <string>

// Representa un proceso a planificar.
// Guarda tanto los datos de entrada (etiqueta, BT, AT, Q, Pr) como las
// métricas que se calculan durante la simulación (WT, CT, RT, TAT).
class Process {
public:
    // --- Datos de entrada ---
    std::string label;      // Etiqueta (A, B, p1, proc1, ...)
    double burstTime;       // BT: tiempo total de CPU requerido
    double arrivalTime;     // AT: instante de llegada a la cola de listos
    int queueId;            // Q: cola a la que pertenece (1, 2, 3, ...)
    int priority;           // Pr: prioridad dentro de su cola (5 alta, 1 baja)

    // --- Estado durante la simulación ---
    double remainingTime;   // tiempo de CPU que aún le falta
    bool hasStarted;        // ¿ya recibió CPU alguna vez?

    // --- Métricas de salida ---
    double startTime;       // primer instante en que obtuvo CPU
    double completionTime;  // CT: instante en que termina
    double waitingTime;     // WT: TAT - BT
    double responseTime;    // RT: startTime - AT
    double turnaroundTime;  // TAT: CT - AT

    Process(std::string label, double bt, double at, int q, int pr);

    // ¿Ya consumió todo su burst time?
    bool isFinished() const;
};
