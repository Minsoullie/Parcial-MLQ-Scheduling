#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include "FileHandler.h"
#include "MLQScheduler.h"
#include "Queue.h"

// Deriva un nombre de salida "mlqXXX.txt" -> "mlqXXX_out.txt"
static std::string defaultOutputName(const std::string& inputPath) {
    std::string path = inputPath;
    size_t dot = path.find_last_of('.');
    std::string base = (dot == std::string::npos) ? path : path.substr(0, dot);
    std::string ext = (dot == std::string::npos) ? "" : path.substr(dot);
    return base + "_out" + (ext.empty() ? ".txt" : ext);
}

static void printTrace(const MLQScheduler& sched) {
    std::cout << "\nTraza de ejecucion (Gantt):\n";
    for (const auto& seg : sched.getTrace()) {
        std::cout << "  [" << std::setw(4) << std::get<0>(seg) << " - "
                  << std::setw(4) << std::get<1>(seg) << "]  "
                  << std::get<2>(seg) << "  (" << std::get<3>(seg) << ")\n";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo_entrada> [archivo_salida] [--trace]\n";
        std::cerr << "Esquema fijo: Cola1=RR(2), Cola2=RR(3), Cola3=STCF\n";
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath;
    bool showTrace = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--trace") {
            showTrace = true;
        } else if (outputPath.empty()) {
            outputPath = arg;
        }
    }
    if (outputPath.empty()) outputPath = defaultOutputName(inputPath);

    try {
        std::vector<Process> processes = FileHandler::readInput(inputPath);
        if (processes.empty()) {
            std::cerr << "Advertencia: el archivo de entrada no contiene procesos validos.\n";
        }

        MLQScheduler scheduler;
        // Esquema elegido para este parcial: RR(2), RR(3), STCF
        scheduler.addQueue(std::make_unique<RoundRobinQueue>(1, 2.0)); // Cola 1 (mayor prioridad)
        scheduler.addQueue(std::make_unique<RoundRobinQueue>(2, 3.0)); // Cola 2
        scheduler.addQueue(std::make_unique<STCFQueue>(3));            // Cola 3 (menor prioridad)

        scheduler.loadProcesses(processes);
        scheduler.run();

        FileHandler::writeOutput(outputPath, inputPath, processes);

        std::cout << "Simulacion completa (" << processes.size() << " procesos).\n";
        std::cout << "Resultados escritos en: " << outputPath << "\n";

        if (showTrace) printTrace(scheduler);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
