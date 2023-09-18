use std::env;
use std::process::exit;

fn usage(argv0: &str) {
    eprintln!("usage: {} R1 R2 [Vin]", argv0);
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let argc = args.len();

    if argc < 3 || argc > 4 {
        usage(&args[0]);
        exit(1);
    }

    let r1 = args[1].parse::<f64>().unwrap();
    let r2 = args[2].parse::<f64>().unwrap();
    let mult = r2 / (r1 + r2);

    println!("voltage scale: {}", mult);

    if argc >= 4 {
        let vin = args[3].parse::<f64>().unwrap();
        println!("output voltage (with Vin = {}): {}", vin, mult * vin);
    }
}
