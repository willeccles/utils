use std::env;
use std::process::exit;
use std::io::prelude::*;
use std::fs::File;
use std::path::Path;
use std::io::BufReader;

fn usage(argv0: &str) {
    eprintln!("usage: {} PATH", argv0);
}

// TODO figure out error handling, which appears to be a massive PITA in rust...
fn calc_crc<R>(reader: &mut BufReader<R>) -> u16 {
    0
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let argc = args.len();

    if argc != 2 {
        usage(&argv[0]);
        exit(1);
    }

    let path = Path::new(args[1]);
    let mut file = match File::open(&path) {
        Ok(file) => file,
        Err(e) => {
            eprintln!("error opening file: {e}");
            exit(1);
        },
    };

    let mut buf_reader = BufReader::new(file);
}
