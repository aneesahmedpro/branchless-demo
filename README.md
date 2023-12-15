# Demonstration of Branchless Programming

Branchless solutions run much faster than the solutions that contain branches.

## Benchmark Problem

Given a random alphanumeric string, convert it to an uppercase string.

The random string contains lowercase letters, uppercase letters and numeric digits. Ignore the digits for case-conversion.

## How to run the C Demo

Use `build.sh` to build the executable:
```sh
./build.sh
```

Run the executable with desired benchmarking parameters:
```sh
./demo 500 1000000  # 500 rounds, 1000000 chars
```

> **Tip:** Use `clean.sh` to delete all build artefacts.

> **Tip:** Run the executable without any command-line arguments to see help/usage.

## How to run the Rust Demo

Use `cargo` to build the executable (use **release** build):
```sh
cargo build --release
```

Run the executable with desired benchmarking parameters:
```sh
./target/release/demo 500 1000000  # 500 rounds, 1000000 chars
```

> **Tip:** Use `cargo run --release` to both build and run the demo. E.g.: `cargo run --release -- 500 1000000`

> **Tip:** Run the executable without any command-line arguments to see help/usage.

## What is Branchless Programming?

Write code in such a way that does not have any conditional jumps.

Whenever an `if` construct is used, the flow of program execution is split into two potential paths ahead; this is referred to as branching in the code. Which of these two paths will be taken is decided by whether the `if` condition is true or false. Thus, the execution flow depends on user input and other runtime parameters.

If all `if` constructs are eliminated, then there are no branches in the code at all, and the flow of program execution is linear, straightforward and predictable.

## Why is a Branchless Program faster?

Modern CPUs are sophisticated pieces of machinery, and use complicated tricks to boost performance. The relevant trick here is a combination of Speculative Execution and Branch Prediction.

### Speculative Execution

The CPU is faster than the speed of data flow. Thus, for every program instruction, CPU wastes a lot of time, idling away, waiting for

+ the read/write operations to finish accessing the RAM.
+ the next instruction to be fetched into CPU for execution.

Thus, the CPU pre-fetches the next few instructions in advance, so that it can start decoding them while it is waiting for the current instruction to finish its operations.

### Branch Prediction

As explained earlier, every conditional jump in a program creates branching. The CPU wants to speculatively execute the next instructions, but it cannot foresee which of the two paths will be taken. By the time CPU would get enough information to decide whether or not the conditional jump will happen, it would have wasted a lot of time waiting because it has nothing else to do in the mean time.

Thus, it predicts which of the two paths will be taken based on statistical analysis, and starts speculatively executing the instructions in the path it think is more likely. This statistical analysis is well-designed, and hence the branch predictor ends up being right about the prediction most of the time. This gives a significant boost to the overall program execution speed.

However, every time the branch predictor is wrong about its prediction, it has to discard all the *work* it did speculatively, as the *work* is no longer relevant. It then has to wait for the instructions to be fetched that actually are from the code path that it originally considered the less likely of the two paths. Thus, the CPU has wasted a lot time in such a case. This leads to worse program execution speed.

### Random Input confusing the Branch Predictor

If the input data is nearly random, that is, the odds of each of the two code paths in a branching point are nearly 50/50, then no amount good statistical analysis is going to be of any help. The branch predictor cannot discover a pattern where there is absolutely none. In such a case, the odds of the branch predictor being wrong is also 50/50. This leads to stalling in CPU, and causes a significant drop in the program execution speed.

Thus, nearly random input data confuses the branch predictor.

### Skipping the Branch Predictor to boost Speed

By writing code in such a way that does not have any conditional jumps, any dependence on branch prediction is avoided. This enables the CPU to continuously speculatively execute the next instructions without any problems, boosting the program execution speed. Thus, such a program that is free of branches ends up running much faster.
