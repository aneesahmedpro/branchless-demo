use std::process::ExitCode;

fn to_upper_naive(string: &mut Vec<u8>) {
    const DIFF: u8 = b'a' - b'A';

    for byte in string {
        if *byte >= b'a' && *byte <= b'z' {
            *byte -= DIFF;
        }
    }
}

fn to_upper_branchless(string: &mut Vec<u8>) {
    const DIFF: u8 = b'a' - b'A';

    for byte in string {
        *byte -= DIFF * ((*byte >= b'a' && *byte <= b'z') as u8);
    }
}

fn to_upper_rust_std(string: &mut Vec<u8>) {
    string.make_ascii_uppercase();
}

fn generate_random_string(string_len: usize) -> Vec<u8> {
    use rand::Rng;

    const SET: &[u8] = b"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    let mut rng = rand::thread_rng();

    let string: Vec<u8> = (0..string_len)
        .map(|_| {
            let idx = rng.gen_range(0..SET.len());
            SET[idx]
        })
        .collect();

    string
}

fn benchmark(
    func: fn(&mut Vec<u8>),
    func_label: &str,
    rounds: u32,
    string_len: usize
) {
    use std::time;
    use std::io::{self, Write};

    print!("function: {}", func_label);
    io::stdout().flush().expect("stdout should flush");

    let mut total_time = time::Duration::ZERO;
    for _ in 0..rounds {
        let string = generate_random_string(string_len);

        let mut output = string.clone();
        let t = time::Instant::now();
        func(&mut output);
        total_time += t.elapsed();

        let correct_output = string.to_ascii_uppercase();
        if output != correct_output {
            println!(" | wrong output");
            return;
        }
    }
    let time_per_call = total_time / rounds;

    println!(" | time: {} microseconds", time_per_call.as_micros());
}

fn print_usage() {
    let argv0 = std::env::args().nth(0).expect("OS should provide argv[0]");
    print!("\nUsage: {} N L\n", argv0);
    print!(concat!(
        "    N = number of test rounds\n",
        "    L = length of test string\n",
    ));
    print!("\nExample: {} 500 1000000\n", argv0);
}

fn main() -> ExitCode {
    let argv: Vec<String> = std::env::args().collect();
    if argv.len() != 3 {
        eprintln!("Invalid invocation.");
        print_usage();
        return ExitCode::FAILURE;
    }
    let test_rounds: u32 = argv[1].parse().unwrap_or(0);
    if test_rounds == 0 {
        eprintln!("Error: N must be a non-zero positive integer.");
        print_usage();
        return ExitCode::FAILURE;
    }
    let test_string_len: usize = argv[2].parse().unwrap_or(0);
    if test_string_len == 0 {
        eprintln!("Error: L must be a non-zero positive integer.");
        print_usage();
        return ExitCode::FAILURE;
    }

    benchmark(to_upper_naive, "naive", test_rounds, test_string_len);
    benchmark(to_upper_branchless, "branchless", test_rounds, test_string_len);
    benchmark(to_upper_rust_std, "rust std", test_rounds, test_string_len);

    ExitCode::SUCCESS
}
