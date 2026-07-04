#include "Process.h"

Process::Process(std::string label, double bt, double at, int q, int pr)
    : label(std::move(label)),
      burstTime(bt),
      arrivalTime(at),
      queueId(q),
      priority(pr),
      remainingTime(bt),
      hasStarted(false),
      startTime(-1.0),
      completionTime(-1.0),
      waitingTime(0.0),
      responseTime(0.0),
      turnaroundTime(0.0) {}

bool Process::isFinished() const {
    return remainingTime <= 0.0;
}
