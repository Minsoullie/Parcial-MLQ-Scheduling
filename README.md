# Simulador MLQ (Multilevel Queue Scheduling)

Sistemas Operativos — Universidad del Valle — Parcial No. 1

Simulador orientado a objetos en **C++17** del algoritmo de planificación
**MLQ** con el esquema de colas:

| Cola | Prioridad | Algoritmo |
|------|-----------|-----------|
| 1    | Más alta  | Round Robin, quantum = 2 |
| 2    | Media     | Round Robin, quantum = 3 |
| 3    | Más baja  | STCF (Shortest Time-to-Completion First, expropiativo) |

La cola 1 siempre tiene prioridad absoluta sobre la 2, y la 2 sobre la 3: si
llega un proceso a una cola de mayor prioridad mientras se ejecuta uno de
menor prioridad, este último es expropiado de inmediato (ver `DISEÑO.md`
más abajo / sección de este README).

## Estructura del proyecto

```
mlq-scheduler/
├── include/            # headers (.h)
│   ├── Process.h
│   ├── Queue.h          # Queue (abstracta), RoundRobinQueue, STCFQueue
│   ├── MLQScheduler.h
│   └── FileHandler.h
├── src/                # implementación (.cpp)
│   ├── Process.cpp
│   ├── Queue.cpp
│   ├── MLQScheduler.cpp
│   ├── FileHandler.cpp
│   └── main.cpp
├── data/               # archivos de entrada de ejemplo
│   ├── mlq001.txt
│   ├── mlq002.txt
│   └── mlq003.txt
├── output/             # aquí se escriben los archivos de salida
├── Makefile
└── README.md
```

## Compilar

Requiere `g++` con soporte C++17 (GCC ≥ 7) y `make`.

```bash
make
```

Esto genera el ejecutable `mlq_scheduler` en la raíz del proyecto.

## Ejecutar

```bash
./mlq_scheduler <archivo_entrada> [archivo_salida] [--trace]
```

- `archivo_entrada`: ruta al `.txt` con los procesos (formato del enunciado).
- `archivo_salida` (opcional): si no se indica, se genera automáticamente
  como `<entrada>_out.txt`.
- `--trace` (opcional): imprime en consola la traza de ejecución (Gantt en
  texto), útil para explicar el funcionamiento en el video.

Ejemplos:

```bash
./mlq_scheduler data/mlq001.txt output/mlq001_out.txt --trace
./mlq_scheduler data/mlq002.txt output/mlq002_out.txt
./mlq_scheduler data/mlq003.txt output/mlq003_out.txt --trace
```

También se puede usar el Makefile:

```bash
make run FILE=data/mlq001.txt
```

## Formato de entrada

```
# etiqueta; burst time (BT); arrival time (AT); Queue (Q); Priority(5>1)
A;6;0;1;5
```

Las líneas vacías o que comienzan con `#` se ignoran.

## Formato de salida

```
# archivo: mlq001.txt
# etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT
A;6; 0; 1; 5; 4; 10; 0; 10
...
# WT=19.80; CT=29.40; RT=15; TAT=29.40;
```

Donde `WT` = waiting time, `CT` = completion time, `RT` = response time,
`TAT` = turnaround time; la última línea son los promedios.

## Diseño / decisiones técnicas

1. **Motor de simulación por "ticks".** El reloj avanza en incrementos de
   1 unidad de tiempo. En cada tick el `MLQScheduler`:
   1. Inserta en su cola correspondiente los procesos cuyo `AT` ya se cumplió.
   2. Busca, en orden de prioridad (cola 1 → 2 → 3), la primera cola con
      trabajo pendiente.
   3. Le pide a **esa** cola (y solo a esa) que ejecute un tick.

   Esto simplifica enormemente la implementación de STCF (en cada tick se
   reevalúa cuál es el proceso de menor tiempo restante, lo que produce la
   expropiación automáticamente) y hace que la expropiación **entre colas**
   sea un efecto natural del diseño: si una cola no es la activa, su método
   `tick()` simplemente no se invoca, así que su estado (quantum consumido,
   proceso en curso) queda "congelado" hasta que vuelva a ser la de mayor
   prioridad con trabajo pendiente.

   *Nota:* esto asume tiempos (`BT`/`AT`) enteros o múltiplos del tick. Los
   archivos de prueba de este curso cumplen esa condición.

2. **Expropiación entre colas.** Cuando un proceso de una cola de mayor
   prioridad llega mientras se ejecuta uno de menor prioridad, este último
   **conserva su lugar y el resto de su quantum** (no se reinicia ni se
   manda al final de la fila): al recuperar la CPU continúa exactamente
   donde se quedó. Es una decisión de diseño razonable y se explica más a
   fondo en el informe.

3. **Round Robin.** Cada cola RR mantiene su propia `deque<Process*>` FIFO.
   Un proceso se reencola al final solo si agota su quantum sin terminar.

4. **STCF.** No usa cola FIFO: mantiene un `vector<Process*>` con todos los
   procesos que han llegado a esa cola, y en cada tick escoge el de menor
   `remainingTime` (empatan por `arrivalTime`, y si persiste, por el orden
   original del archivo).

5. **Campo `Priority`.** Se lee, se conserva y se imprime en la salida,
   pero **no se usa** para decidir el orden de ejecución, porque ninguna de
   las tres colas de este esquema usa disciplina por prioridad (RR y STCF
   no la necesitan).

## Casos de prueba incluidos

- `mlq001.txt`: el ejemplo del enunciado (todos llegan en t=0). Sirve para
  validar formato y ver el comportamiento "puro" de cada disciplina.
- `mlq002.txt`: llegadas escalonadas (t=0, 5, 10) que fuerzan expropiación
  entre las tres colas simultáneamente.
- `mlq003.txt`: varios procesos llegando a la cola STCF en distintos
  instantes, para evidenciar la expropiación *dentro* de la misma cola.

Los tres se verificaron a mano (traza tick a tick) contra la salida del
programa antes de la entrega.
