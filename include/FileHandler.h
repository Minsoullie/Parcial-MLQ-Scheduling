#pragma once
#include <string>
#include <vector>
#include "Process.h"

namespace FileHandler {

// Lee un archivo de entrada con líneas "etiqueta;BT;AT;Q;Pr".
// Ignora líneas vacías y líneas que comienzan con '#'.
std::vector<Process> readInput(const std::string& path);

// Escribe el archivo de salida con el formato pedido en el enunciado:
//   # archivo: <nombre>
//   # etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT
//   A;6; 0; 1; 5; 3; 9; 0; 9
//   ...
//   # WT=..; CT=..; RT=..; TAT=..;
void writeOutput(const std::string& outPath,
                  const std::string& originalInputName,
                  const std::vector<Process>& procs);

} // namespace FileHandler
