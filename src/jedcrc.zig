const std = @import("std");
const exit = std.process.exit;

const STX = 0x02;
const ETX = 0x03;

const JedCrcError = error {
    StartNotFound,
    EndNotFound,
    FileIOError,
};

fn usage(argv0: [:0]const u8) !void {
    const stderr = std.io.getStdOut().writer();
    const usage_str =
        \\usage: {s} [FILE]
        \\  If FILE is missing or "-", read from stdin.
        \\
        ;

    try stderr.print(usage_str, .{ std.fs.path.basename(argv0) });
}

fn computeJedecCrc(reader: anytype) JedCrcError!u16 {
    var start = false;
    var end = false;
    var eof = false;
    var sum: u16 = 0;

    while (!end and !eof) {
        if (reader.readByte()) |v| {
            if (!start and v == STX) {
                start = true;
            }

            if (start and !end) {
                _ = @addWithOverflow(u16, sum, v, &sum);
                //sum += v;
            }

            if (!end and v == ETX) {
                end = true;
            }
        } else |err| {
            switch (err) {
                error.EndOfStream => eof = true,
                else => return JedCrcError.FileIOError,
            }
        }
    }

    if (!start) {
        return JedCrcError.StartNotFound;
    } else if (!end) {
        return JedCrcError.EndNotFound;
    } else {
        return sum;
    }
}

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    const stdout = std.io.getStdOut().writer();
    const stderr = std.io.getStdOut().writer();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len > 2) {
        try usage(args[0]);
        exit(1);
    }

    var file = blk: {
        if (args.len == 1 or std.mem.eql(u8, args[1], "-")) {
            break :blk std.io.getStdIn();
        } else {
            break :blk try std.fs.cwd().openFile(args[1], .{ .mode = .read_only });
        }
    };

    defer file.close();

    var buf = std.io.bufferedReader(file.reader());
    const reader = &buf.reader();

    if (computeJedecCrc(reader)) |sum| {
        try stdout.print("Checksum: {X:.4}\n", .{ sum });
    } else |err| {
        switch (err) {
            JedCrcError.StartNotFound => try stderr.print("No STX byte found\n", .{}),
            JedCrcError.EndNotFound => try stderr.print("No ETX byte found\n", .{}),
            JedCrcError.FileIOError => try stderr.print("File IO error\n", .{}),
        }
        exit(1);
    }
}
