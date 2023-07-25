const std = @import("std");
const exit = std.process.exit;

const ConversionError = error {
    EmptyInput,
    InvalidInput,
};

fn usage(argv0: [:0]const u8) !void {
    const stderr = std.io.getStdOut().writer();
    const usage_str =
        \\usage: {s} [%][0x]HEX
        \\  If the hex value begins with a % character, it will be interpreted
        \\  as little-endian and its byte order will be swapped before
        \\  converting.
        \\
        ;

    try stderr.print(usage_str, .{ std.fs.path.basename(argv0) });
}

fn convert(arg: []const u8) ConversionError!f32 {
    var val = std.mem.trim(u8, arg, " \t\n");

    if (val.len == 0) {
        return ConversionError.EmptyInput;
    }

    const le = val[0] == '%';
    if (le) {
        val = val[1..];
    }

    if (std.mem.startsWith(u8, val, "0x") or std.mem.startsWith(u8, val, "0X")) {
        val = val[2..];
    }

    if (val.len == 0) {
        return ConversionError.EmptyInput;
    }

    var uval = std.fmt.parseUnsigned(u32, val, 16) catch {
        return ConversionError.InvalidInput;
    };

    if (le) {
        uval = @byteSwap(uval);
    }

    return @bitCast(f32, uval);
}

fn printFloat(w: anytype, f: f32) !void {
    // roughly mimic the default behavior of %g in printf(3)
    if (f < 1e6 and f >= 1e-4) {
        try w.print("{d:.6}\n", .{ f });
    } else {
        try w.print("{e}\n", .{ f });
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

    if (args.len != 2) {
        try usage(args[0]);
        exit(1);
    }

    if (convert(args[1])) |val| {
        try printFloat(stdout, val);
    } else |err| {
        switch (err) {
            ConversionError.EmptyInput => {
                try stderr.print("error: empty input\n", .{});
            },
            ConversionError.InvalidInput => {
                try stderr.print("error: invalid input\n", .{});
            },
        }
        exit(1);
    }
}

test "convert valid" {
    const tests = .{
        .{"42fb8997",              125.76873016357422, 0.00000000001},
        .{"  42fb8997  ",          125.76873016357422, 0.00000000001},
        .{"\t \n42fb8997 \n \t",   125.76873016357422, 0.00000000001},
        .{"0x42fb8997",            125.76873016357422, 0.00000000001},
        .{"  0x42fb8997  ",        125.76873016357422, 0.00000000001},
        .{"\t \n0x42fb8997 \n \t", 125.76873016357422, 0.00000000001},
        .{"0X42fb8997",            125.76873016357422, 0.00000000001},
        .{"  0X42fb8997  ",        125.76873016357422, 0.00000000001},
        .{"\t \n0X42fb8997 \n \t", 125.76873016357422, 0.00000000001},
    };

    inline for (tests) |t| {
        try std.testing.expectApproxEqAbs(try convert(t[0]), t[1], t[2]);
    }
}

test "convert little endian" {
    const tests = .{
        .{"%009A893C",              0.01679706573486328, 0.00000000001},
        .{"  %009A893C  ",          0.01679706573486328, 0.00000000001},
        .{"\t \n%009A893C \n \t",   0.01679706573486328, 0.00000000001},
        .{"%0x009A893C",            0.01679706573486328, 0.00000000001},
        .{"  %0x009A893C  ",        0.01679706573486328, 0.00000000001},
        .{"\t \n%0x009A893C \n \t", 0.01679706573486328, 0.00000000001},
        .{"%0X009A893C",            0.01679706573486328, 0.00000000001},
        .{"  %0X009A893C  ",        0.01679706573486328, 0.00000000001},
        .{"\t \n%0X009A893C \n \t", 0.01679706573486328, 0.00000000001},
    };

    inline for (tests) |t| {
        try std.testing.expectApproxEqAbs(try convert(t[0]), t[1], t[2]);
    }
}

test "convert invalid input" {
    // empty input
    const empty_inputs = .{
        "",
        "   ",
        " \n \t ",
        "%",
        " %  ",
        " \n %\t ",
    };

    // invalid input
    const invalid_inputs = .{
        "fffffffff",
        "%fffffffff",
        "-1",
        "%-1",
        "hello",
        "%hello",
    };

    inline for (empty_inputs) |i| {
        try std.testing.expectError(ConversionError.EmptyInput, convert(i));
    }

    inline for (invalid_inputs) |i| {
        try std.testing.expectError(ConversionError.InvalidInput, convert(i));
    }
}
