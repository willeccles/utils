use std::env;
use std::process::exit;

fn usage(argv0: &str) {
    eprintln!(r"usage: {} [%][0x]HEX
  If the hex value begins with a % character, it will be interpreted as
  little-endian and its byte order will be swapped before converting.", argv0);
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let argc = args.len();

    if argc != 2 {
        usage(&args[0]);
        exit(1);
    }

    let arg_lower = args[1].to_lowercase();
    let mut arg: &str = arg_lower.trim();

    if arg.is_empty() {
        eprintln!("invalid argument");
        exit(1);
    }

    let le = if arg.len() >= 1 && arg.starts_with("%") {
        arg = arg.strip_prefix("%").unwrap();
        true
    } else {
        false
    };

    if arg.is_empty() {
        eprintln!("invalid argument");
        exit(1);
    }

    if arg.len() >= 2 && arg.starts_with("0x") {
        arg = arg.strip_prefix("0x").unwrap();
    }

    let mut v = match u32::from_str_radix(arg, 16) {
        Ok(val) => val,
        Err(e) => {
            eprintln!("failed to parse argument: {e}");
            exit(1);
        },
    };

    if le {
        v = v.swap_bytes();
    }

    let float = f32::from_bits(v);

    println!("{}", float);
}
