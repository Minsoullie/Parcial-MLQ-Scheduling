# Guion para el video explicativo (máx. 5:00)

> Esto es una **guía**, no un libreto para leer palabra por palabra. Grábate
> explicándolo con tus propias palabras — el profesor evalúa que se note
> comprensión real, no una lectura. Usa `--trace` al ejecutar el programa
> para mostrar la traza en pantalla mientras hablas.

---

## 0:00 – 0:30 | Introducción (30 s)

- Tu nombre, código y el tema: "Simulador de planificación MLQ".
- Una frase sobre qué es MLQ: varias colas con distinta prioridad, cada
  una con su propia política de planificación.
- Menciona el esquema elegido: **Cola 1 = RR(2)**, **Cola 2 = RR(3)**,
  **Cola 3 = STCF**, con prioridad estricta 1 > 2 > 3.

## 0:30 – 2:00 | Recorrido por el código (90 s)

Muestra el editor/IDE con el proyecto abierto y explica, en este orden:

1. **`Process`** — qué datos trae de entrada (`label, BT, AT, Q, Pr`) y qué
   métricas calcula (`WT, CT, RT, TAT`).
2. **`Queue` (clase abstracta)** — explica que aquí está el patrón
   *Strategy*: cada cola concreta encapsula su propia estructura de datos
   y su propio algoritmo.
   - `RoundRobinQueue`: usa un `deque` (FIFO) + un contador de quantum
     consumido.
   - `STCFQueue`: usa un `vector` y en cada tick recorre la lista buscando
     el proceso con menor tiempo restante — así el algoritmo se vuelve
     expropiativo "gratis", sin lógica extra.
3. **`MLQScheduler`** — el corazón del simulador. Explica el bucle
   principal: en cada unidad de tiempo (tick):
   1. Inserta los procesos que ya llegaron en su cola correspondiente.
   2. Busca la primera cola (en orden de prioridad) que tenga trabajo.
   3. Le pide *solo a esa cola* que ejecute un tick.
   - Aquí explica **por qué esto produce expropiación entre colas
     automáticamente**: si una cola no es la activa, su `tick()` no se
     llama, así que su proceso en curso queda "congelado" y retoma
     exactamente donde iba cuando vuelva a ser la de mayor prioridad.
4. **`FileHandler`** — cómo se parsean las líneas `etiqueta;BT;AT;Q;Pr` y
   cómo se escribe el archivo de salida con el formato pedido.

*(Personalización sugerida: cuenta qué parte te costó más entender o
depurar, y cómo lo resolviste — eso demuestra comprensión propia.)*

## 2:00 – 3:30 | Un archivo de entrada propio y su ejecución (90 s)

- Muestra tu archivo de entrada (créalo tú, distinto a los tres de ejemplo
  del repositorio) y explica por qué lo diseñaste así (ej. "quiero forzar
  una expropiación entre la cola 1 y la cola 3").
- Ejecuta: `./mlq_scheduler data/tu_archivo.txt --trace`
- Señala en la traza al menos un momento de expropiación y explica qué
  pasó: qué proceso estaba corriendo, cuál lo interrumpió y por qué.

## 3:30 – 4:30 | Análisis de resultados (60 s)

- Abre el archivo `_out.txt` generado.
- Explica las métricas de 1 o 2 procesos concretos (por qué su WT/RT/TAT
  da ese valor, relacionándolo con la traza que acabas de mostrar).
- Comenta los promedios finales: ¿tiene sentido que sean altos o bajos
  dado el orden de llegada y el esquema de colas usado?

## 4:30 – 5:00 | Cierre (30 s)

- Una conclusión breve: qué aprendiste sobre planificación expropiativa
  vs. no expropiativa con este ejercicio.
- (Opcional, conecta con los objetivos del enunciado) una frase sobre cómo
  esta misma lógica de "colas con prioridad + política interna" aparece en
  otros dominios: por ejemplo, calidad de servicio (QoS) en redes, o
  priorización de tareas en gestión de proyectos.

---

### Checklist antes de grabar

- [ ] Repositorio ya subido (público) y accesible.
- [ ] Al menos 3 archivos de entrada probados (los de `data/` + el tuyo).
- [ ] Terminal grande / letra legible en pantalla.
- [ ] Duración total ≤ 5:00.
