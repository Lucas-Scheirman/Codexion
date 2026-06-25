*This project has been created as part of the 42 curriculum by lscheirm.*

# Codexion

> Master the race for resources before the deadline masters you.

## Description

Codexion is a concurrency simulation written in C using POSIX threads. It is a
variation of the classic *dining philosophers* problem: instead of philosophers
sharing forks, **coders** share **USB dongles** to compile quantum code.

Each coder is a thread that cycles endlessly through three states — **compiling**,
**debugging**, and **refactoring**. To compile, a coder needs the two dongles
adjacent to its seat (one on the left, one on the right), held simultaneously.
There are exactly as many dongles as coders, arranged in a circle, so neighbours
compete for the same shared dongle.

The twist that makes this project harder than a plain philosophers exercise:

- **Configurable scheduling** — when several coders want the same dongles, access
  is arbitrated by a policy chosen at launch: `fifo` (first request served first)
  or `edf` (earliest burnout deadline served first).
- **A hand-written priority queue (binary heap)** drives that arbitration. No
  standard-library priority queue is used.
- **A dongle cooldown** — once released, a dongle stays unavailable for a
  configurable delay before anyone may take it again.
- **A dedicated monitor thread** watches every coder and detects burnout within
  10 ms of it happening.

A coder **burns out** if it fails to start a new compile within `time_to_burnout`
milliseconds of its previous compile (or of the simulation start). The simulation
ends either when one coder burns out, or when every coder has compiled the required
number of times.

## Instructions

### Compilation

```bash
make
```

The project builds with `cc -Wall -Wextra -Werror -pthread`. Available rules:
`all`, `clean`, `fclean`, `re`.

### Execution

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> \
           <time_to_debug> <time_to_refactor> <number_of_compiles_required> \
           <dongle_cooldown> <scheduler>
```

| Argument | Meaning | Unit |
|---|---|---|
| `number_of_coders` | Number of coders (and dongles) | count (> 0) |
| `time_to_burnout` | Max delay between two compiles before burnout | ms |
| `time_to_compile` | Duration of the compile phase | ms |
| `time_to_debug` | Duration of the debug phase | ms |
| `time_to_refactor` | Duration of the refactor phase | ms |
| `number_of_compiles_required` | Compiles per coder before success | count |
| `dongle_cooldown` | Unavailability delay after release | ms |
| `scheduler` | Arbitration policy: `fifo` or `edf` | string |

All arguments are mandatory. Negative numbers, non-integers, and any scheduler
value other than `fifo` or `edf` are rejected with an error message.

### Usage examples

A run that ends in **success** (every coder compiles 3 times, no burnout):

```bash
./codexion 5 800 200 200 200 3 0 fifo
```

A run engineered to **burn out** (burnout window shorter than a full cycle):

```bash
./codexion 3 450 200 200 200 50 0 fifo
```

The same scenario under the deadline-aware policy:

```bash
./codexion 3 450 200 200 200 50 0 edf
```

### Log format

Every state change prints one line: `timestamp_in_ms coder_number message`.

```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 1 is debugging
400 1 is refactoring
451 1 burned out
```

Timestamps are relative to the start of the simulation.

## Feature list

- One thread per coder, plus one separate monitor thread.
- `fifo` and `edf` scheduling, selectable at runtime.
- Custom binary min-heap priority queue (no standard library structure).
- Mandatory dongle cooldown after every release.
- Burnout detection accurate to within 10 ms.
- Serialized logging — output lines never interleave.
- Clean shutdown: every thread joins, every mutex and condition variable is
  destroyed, all heap memory is freed.

## Technical choices

The simulation is built around a single shared `t_env` structure passed by pointer
to every thread, so **no global variables** are used (forbidden by the subject).

Rather than locking the two dongles directly — the textbook source of deadlock in
the dining philosophers problem — Codexion routes **every** acquisition request
through a single central arbiter. A coder that wants to compile pushes a request
into a shared priority queue and waits on a condition variable until the scheduler
decides it is its turn. Because only one coder is ever cleared to take resources at
a time, two coders never contend for the same dongles simultaneously, and circular
waiting cannot form.

The priority queue is a hand-written binary min-heap stored on a flat array. The
heap is policy-agnostic: it always pops the request with the smallest `priority`
value. Only the *meaning* of that value changes with the scheduler — under `fifo`
it is the request's arrival timestamp, under `edf` it is the coder's burnout
deadline (`last_compile_start + time_to_burnout`). The smaller the value, the more
urgent the request.

## Blocking cases handled

Concurrency correctness rests on neutralising the four **Coffman conditions**
required for a deadlock to occur:

- **Mutual exclusion** is unavoidable here (a dongle is held by at most one coder),
  so the other three conditions are the ones actively prevented.
- **Hold and wait** is eliminated: a coder never grabs one dongle and then blocks
  waiting for the second. It is only granted access once the scheduler has verified
  that **both** of its dongles are free and past cooldown — the two dongles are
  always acquired together, atomically, or not at all.
- **No preemption** is sidestepped because resources are granted by a central
  authority instead of being seized: the scheduler decides, the coder never forces.
- **Circular wait** cannot form because there is a single decision point. With one
  global ordering of who-takes-what, there is no cycle of "A waits for B waits for
  A".

Other blocking concerns addressed:

- **Starvation prevention.** Under `edf`, the coder closest to burning out is always
  served first, which keeps the most-at-risk coder alive. Under `fifo`, requests are
  served strictly in arrival order, so no coder can be indefinitely overtaken.
- **Cooldown handling.** Before a coder is cleared to compile, the scheduler checks
  that `now - released_at >= dongle_cooldown` for **both** of its dongles. A coder
  whose turn has come but whose dongles are still cooling down simply stays queued
  and is re-evaluated, rather than taking a dongle too early.
- **Precise burnout detection.** A dedicated monitor thread polls every coder every
  millisecond, comparing elapsed time since `last_compile_start` against
  `time_to_burnout`. The 1 ms poll interval keeps detection well within the 10 ms
  precision the subject requires.
- **Log serialization.** All output goes through a single logging function guarded
  by a dedicated mutex, so two state messages can never interleave on one line.
- **Clean termination.** When the simulation must stop, the stop flag is raised and
  every coder waiting on the scheduler's condition variable is woken, so no thread
  is left blocked forever and every `pthread_join` returns.

## Thread synchronization mechanisms

The implementation relies on three POSIX primitives — `pthread_mutex_t`,
`pthread_cond_t`, and `pthread_t` — coordinated around the shared environment.

**Scheduling mutex + condition variable (`sched_lock` / `sched_cond`).**
This pair is the heart of the arbitration. The mutex protects *both* the priority
queue and the dongles' state (`is_taken`, `released_at`), because the decision to
grant resources depends on all of them at once; protecting them with separate locks
would require a multi-lock protocol and reintroduce the very deadlock risk we are
avoiding. A coder pushes its request, then calls `pthread_cond_wait`, which
atomically releases the mutex and sleeps. When a coder releases its dongles, or when
the simulation is told to stop, `pthread_cond_broadcast` wakes the waiters so they
can re-check whether it is now their turn. This is the thread-safe channel through
which coders hand resources off to one another without ever talking directly.

**Stop mutex (`stop_lock`).**
The boolean stop flag is shared between the monitor (which sets it) and every coder
(which reads it each cycle). Both access paths go through small helper functions
(`is_stopped` / `set_stop`) that always take the mutex, so the flag can never be
read while it is being written. Centralising access in two functions makes it
impossible to forget the lock at a call site.

**Logging mutex (`log_lock`).**
Wraps the single `printf` in the logging routine so concurrent state changes from
different threads are written one at a time, never mixed.

**Preventing race conditions — a concrete example.**
Two neighbours share a dongle. Without synchronization, both could read `is_taken == 0`
at the same instant and both conclude the dongle is free, duplicating it. In
Codexion this is impossible: the check and the subsequent `is_taken = 1` both happen
while holding `sched_lock`, and only the single coder at the head of the heap is ever
allowed to perform them. The second coder cannot even evaluate the dongle's state
until the first has released the mutex, by which point `is_taken` already reflects
the grant.

**Communication between coders and the monitor.**
The two never call each other. They communicate only through shared state: coders
publish their progress by updating `last_compile_start` and `compiles_done`; the
monitor reads those fields to decide whether a burnout occurred or whether everyone
has finished. The stop flag is the monitor's one outbound signal, and the condition
variable broadcast guarantees that signal reaches even coders currently asleep in
the queue.

## Resources

Classic references used while studying the problem:

- *The Linux Programming Interface*, Michael Kerrisk — chapters on POSIX threads,
  mutexes, and condition variables.
- POSIX `pthread` manual pages (`man pthread_create`, `man pthread_cond_wait`,
  `man pthread_mutex_lock`, …).
- The 42 *Philosophers* project, the canonical introduction to the dining
  philosophers problem this exercise builds on.
- Edsger W. Dijkstra's original writings on the dining philosophers problem and
  deadlock avoidance.
- Standard material on binary heaps and priority-queue implementation.

### Use of AI

AI assistance was used as a study and review aid, not as a code generator for the
core logic. Specifically:

- **Argument parsing:** debugging validation edge cases (handling `0`, the `+`
  sign, over-long numeric strings, scheduler string matching) and splitting
  functions to respect the Norm's line limits.
- **Architecture discussion:** clarifying the separation of responsibilities
  between the coder threads and the monitor thread, and why two separate `create`
  then `join` loops are required for genuine parallelism.
- **Concept clarification:** the role of `last_compile_start` versus `released_at`,
  the meaning of the burnout and EDF-deadline formulas, and why shared state needs
  mutex protection.
- **Scheduler and heap design:** discussing the central-arbiter approach to
  deadlock avoidance and the binary-heap structure backing the priority queue.

Every AI suggestion was reviewed, tested, and rewritten where needed so the author
can explain each design decision during the defence.
