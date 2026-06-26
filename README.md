*This project has been created as part of the 42 curriculum by lscheirm.*

# Codexion

> Master the race for resources before the deadline masters you.

## Description

Codexion is a small simulation written in C with POSIX threads. It is a version
of the classic *dining philosophers* problem. Here, **coders** share **USB
dongles** to compile code.

Each coder is a **thread**. A coder repeats three steps in a loop:
**compiling**, **debugging**, then **refactoring**.

To compile, a coder needs **two dongles** at the same time: the one on its left
and the one on its right. There are as many dongles as coders, placed in a
circle. So two neighbours share the same dongle and must wait for each other.

A coder **burns out** if it does not start a new compile in time. The limit is
`time_to_burnout` ms after the start of its last compile (or after the start of
the simulation).

The simulation stops in two cases:
- one coder **burns out**, or
- every coder has compiled at least `number_of_compiles_required` times.

This project adds a few hard rules on top of the basic problem:
- **Two policies** to choose who gets a dongle: `fifo` (first asked, first
  served) or `edf` (the coder closest to burnout is served first).
- A **binary heap** (a priority queue written by hand) does this choice. No
  priority queue from the standard library is used.
- A **dongle cooldown**: after a dongle is released, it stays unavailable for a
  short time before anyone can take it again.
- A separate **monitor thread** that checks burnout and stops the simulation.

## Instructions

### Build

```bash
make
```

It compiles with `cc -Wall -Wextra -Werror -pthread`.
Rules: `all`, `clean`, `fclean`, `re`.

### Run

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> \
           <time_to_debug> <time_to_refactor> <number_of_compiles_required> \
           <dongle_cooldown> <scheduler>
```

| Argument | Meaning | Unit |
|---|---|---|
| `number_of_coders` | number of coders (and of dongles) | count (> 0) |
| `time_to_burnout` | max time between two compiles | ms |
| `time_to_compile` | time of the compile step | ms |
| `time_to_debug` | time of the debug step | ms |
| `time_to_refactor` | time of the refactor step | ms |
| `number_of_compiles_required` | compiles per coder to win | count |
| `dongle_cooldown` | wait time after a dongle is released | ms |
| `scheduler` | policy: `fifo` or `edf` | text |

All arguments are required. Negative numbers, non-integers, and a scheduler that
is not `fifo` or `edf` are rejected with an error message.

### Examples

A run that ends well (every coder compiles 3 times, nobody burns out):

```bash
./codexion 5 800 200 200 200 3 0 fifo
```

A run made to burn out (the burnout time is too short for a full cycle):

```bash
./codexion 3 250 200 200 200 999 0 edf
```

### Log format

Each state change prints one line: `timestamp coder_number message`.

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
400 1 is refactoring
451 2 burned out
```

The timestamp is in milliseconds, counted from the start of the simulation.

## Project structure

```
src/
├── coder/       what a coder does (run loop, compile, debug + refactor)
├── dongle/      how a dongle is taken and released (choose, take, grant, release, cooldown)
├── heap/        the binary heap (priority queue)
├── scheduler/   the FIFO / EDF compare rule
├── monitor/     the monitor thread (burnout, all-done, stop flag)
├── init_env/    parse arguments, build the environment, create/join threads
├── log/         print the log lines
└── utils/       time and sleep helpers
includes/codexion.h   all types and prototypes
```

## Blocking cases handled

**Deadlock (the main danger).**
To compile, a coder takes one dongle, then the second one. If every coder took
its left dongle first and then waited for its right dongle, they could all wait
for each other forever. This is a **circular wait**.

To stop this, every coder always takes the **smaller-numbered dongle first**,
then the bigger one (function `choose_dongle`). This simple order means a cycle
of waiting can never form, so a deadlock is impossible. This is the classic
**resource ordering** solution, and it removes one of the four **Coffman
conditions** (circular wait) needed for a deadlock.

**Starvation (a coder waits forever).**
Each dongle has its own waiting line, kept as a **binary heap**. When the dongle
is free, it is given to the coder at the **top** of the heap.
- With `fifo`, the top is the coder who asked first.
- With `edf`, the top is the coder whose burnout deadline is the closest.

So no coder is skipped forever, and under `edf` the coder most in danger is
served first.

**Cooldown.**
When a dongle is released, it cannot be taken again for `dongle_cooldown` ms.
The coder that wins the dongle waits for the rest of this delay (function
`cooldown_sleep`) before it really holds it. So a dongle is never taken too early.

**Precise burnout detection.**
A separate monitor thread checks every coder about every 0.5 ms. If a coder did
not start a compile in time, the monitor prints `burned out` and stops the
simulation. 0.5 ms is well under the 10 ms the subject asks.

**Log serialization.**
All prints go through one function with one mutex (`log_lock`). So two lines can
never mix on the same line.

**Clean stop.**
When the simulation must stop, a stop flag is set, and every coder that sleeps on
a dongle is woken with `pthread_cond_broadcast`. So no thread stays blocked, and
every `pthread_join` returns. After the stop, no more state lines are printed
(only the `burned out` line).

## Thread synchronization mechanisms

The project uses three POSIX tools: `pthread_mutex_t`, `pthread_cond_t`, and
`pthread_t`. There are **no global variables**: everything is kept in one shared
`t_env` structure passed by pointer.

**One mutex and one condition variable per dongle (`dongle->lock`, `dongle->cond`).**
The mutex protects the state of one dongle: `is_taken`, `released_at`, and its
heap. A coder that wants the dongle locks it, pushes its request into the heap,
then calls `pthread_cond_wait`. This call **frees the mutex and sleeps** at the
same time. When a coder releases the dongle, it calls `pthread_cond_broadcast` to
wake the waiters. Each waiter checks again: "is the dongle free, and am I at the
top of the heap?" Only then it takes it. This is how coders pass dongles to each
other without ever talking directly.

**One data mutex per coder (`coder->data_lock`).**
It protects `last_compile_start` and `compiles_done`. The coder writes these
values; the monitor reads them. The mutex stops a data race between them.

**One stop mutex (`stop_lock`).**
It protects the `stop` flag, shared between the monitor (which sets it) and the
coders (which read it every loop). Reading goes through `should_stop` and writing
through `set_stop`. Both always take the mutex, so the flag is never read while it
is written.

**One log mutex (`log_lock`).**
It wraps the single `printf`, so two messages never interleave.

**Race condition example.**
Two neighbours share one dongle. Without a lock, both could read `is_taken == 0`
at the same moment and both take it (the dongle would be used twice). In Codexion
this cannot happen: the check and then `is_taken = 1` are both done while holding
`dongle->lock`, and only the single coder at the top of the heap is allowed to do
them. The other coder cannot even read the dongle's state until the first one
unlocks, and by then `is_taken` is already `1`, so it waits.

**Coders and monitor communication.**
They never call each other. They only share state. Coders publish their progress
by updating `last_compile_start` and `compiles_done`. The monitor reads these to
decide if a coder burned out or if everyone is done. The stop flag is the only
signal the monitor sends back, and the broadcast makes sure even a sleeping coder
receives it.

## Resources

References used to study the problem:
- *The Linux Programming Interface*, Michael Kerrisk — chapters on POSIX threads,
  mutexes, and condition variables.
- POSIX `pthread` manual pages (`man pthread_create`, `man pthread_cond_wait`,
  `man pthread_mutex_lock`, ...).
- The 42 *Philosophers* project, the base version of the dining philosophers
  problem this exercise extends.
- Edsger W. Dijkstra's original work on the dining philosophers problem and
  deadlock.
- Standard material on binary heaps and priority queues.

### Use of AI

AI was used as a study and review helper, not to write the core logic for me.
It helped with:
- **Argument parsing:** finding edge cases (the value `0`, a leading `+`, numbers
  that are too long, scheduler text matching) and splitting functions to respect
  the Norm.
- **Concurrency design:** talking through deadlock prevention by ordering, the
  per-dongle mutex/condition/heap design, and why the holder leaves the heap and
  uses `is_taken`.
- **Concept clarification:** the difference between `last_compile_start` and
  `released_at`, the burnout and EDF deadline formulas, and why shared data needs
  a mutex.
- **Project organization:** grouping files by feature (`coder/`, `dongle/`, ...)
  and making file and function names consistent.

Every AI suggestion was read, tested, and rewritten when needed, so the author
can explain each choice during the defence.
