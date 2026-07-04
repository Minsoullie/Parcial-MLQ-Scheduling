#include "FileHandler.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, delim)) tokens.push_back(item);
    return tokens;
}

// Imprime enteros sin decimales y valores fraccionarios con hasta 2 decimales.
std::string fmt(double value) {
    std::ostringstream out;
    if (std::abs(value - std::round(value)) < 1e-9) {
        out << static_cast<long long>(std::round(value));
    } else {
        out << std::fixed << std::setprecision(2) << value;
    }
    return out.str();
}

std::string baseName(const std::string& path) {
    size_t slash = path.find_last_of("/\\");
    return (slash == std::string::npos) ? path : path.substr(slash + 1);
}

} // namespace

namespace FileHandler {

std::vector<Process> readInput(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo de entrada: " + path);
    }

    std::vector<Process> procs;
    std::string line;
    int lineNo = 0;
    while (std::getline(file, line)) {
        lineNo++;
        std::string t = trim(line);
        if (t.empty() || t[0] == '#') continue;

        std::vector<std::string> tok = split(t, ';');
        if (tok.size() < 5) {
            throw std::runtime_error("Línea " + std::to_string(lineNo) +
                                      " inválida (se esperaban 5 campos): " + line);
        }
        std::string label = trim(tok[0]);
        double bt = std::stod(trim(tok[1]));
        double at = std::stod(trim(tok[2]));
        int q = std::stoi(trim(tok[3]));
        int pr = std::stoi(trim(tok[4]));

        procs.emplace_back(label, bt, at, q, pr);
    }
    return procs;
}

void writeOutput(const std::string& outPath,
                  const std::string& originalInputName,
                  const std::vector<Process>& procs) {
    std::ofstream out(outPath);
    if (!out.is_open()) {
        throw std::runtime_error("No se pudo crear el archivo de salida: " + outPath);
    }

    out << "# archivo: " << baseName(originalInputName) << "\n";
    out << "# etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT\n";

    double sumWT = 0, sumCT = 0, sumRT = 0, sumTAT = 0;
    for (const auto& p : procs) {
        out << p.label << ";" << fmt(p.burstTime) << "; " << fmt(p.arrivalTime) << "; "
            << p.queueId << "; " << p.priority << "; " << fmt(p.waitingTime) << "; "
            << fmt(p.completionTime) << "; " << fmt(p.responseTime) << "; "
            << fmt(p.turnaroundTime) << "\n";
        sumWT += p.waitingTime;
        sumCT += p.completionTime;
        sumRT += p.responseTime;
        sumTAT += p.turnaroundTime;
    }

    size_t n = procs.size();
    if (n > 0) {
        out << "# WT=" << fmt(sumWT / n) << "; CT=" << fmt(sumCT / n) << "; RT="
            << fmt(sumRT / n) << "; TAT=" << fmt(sumTAT / n) << ";\n";
    }
}

} // namespace FileHandler
