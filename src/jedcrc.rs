use std::env;
use std::process::exit;
use std::io::prelude::*;
use std::fs::File;
use std::path::Path;
use std::io::BufReader;

#[derive(Debug)]
enum JedCrcError {
    StartNotFound,
    EndNotFound,
    FileIOError,
}

fn usage(argv0: &str) {
    eprintln!(r"usage: {} [FILE]
  If a FILE is missing or '-', reads from stdin.", argv0);
}

// TODO figure out error handling, which appears to be a massive PITA in rust...
fn calc_crc<R>(reader: &mut BufReader<R>) -> Result<u16, JedCrcError> where R: std::io::Read {
    let mut start = false;
    let mut end = false;
    let mut eof = false;
    let mut sum: u16 = 0;

    let mut bytes = reader.bytes();

    while !end && !eof {
        let res = bytes.next();
        if res.is_some() {
            match res.unwrap() {
                Ok(byte) => {
                    // STX found
                    if !start && byte == 0x02 {
                        start = true;
                    }

                    if start && !end {
                        sum = sum.wrapping_add(byte as u16);
                    }

                    // ETX found
                    if !end && byte == 0x03 {
                        end = true;
                    }
                },
                Err(_) => return Err(JedCrcError::FileIOError),
            }
        } else {
            eof = true;
        }
    }

    if !start {
        return Err(JedCrcError::StartNotFound);
    }

    if !end {
        return Err(JedCrcError::EndNotFound);
    }

    return Ok(sum);
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let argc = args.len();

    if argc > 2 {
        usage(&args[0]);
        exit(1);
    }

    let res = if argc == 2 && args[1] != "-" {
        let path = Path::new(&args[1]);
        let file = match File::open(&path) {
            Ok(file) => file,
            Err(e) => {
                eprintln!("error opening file: {e}");
                exit(1);
            },
        };

        let mut buf_reader = BufReader::new(file);
        calc_crc(&mut buf_reader)
    } else {
        let mut buf_reader = BufReader::new(std::io::stdin());

        calc_crc(&mut buf_reader)
    };

    match res {
        Ok(sum) => println!("Checksum: {:0>4X}", sum),
        Err(e) => {
            println!("Error: {:?}", e);
            exit(1);
        },
    }
}
