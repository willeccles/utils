const std = @import("std");
const exit = std.process.exit;

extern "c" fn dlerror() ?[*:0]const u8;

const RTLD_NOW: c_int = 0x00002;

const SymbolType = enum {
    int8,
    int16,
    int32,
    int64,
    uint8,
    uint16,
    uint32,
    uint64,
    xint8,
    xint16,
    xint32,
    xint64,
    float,
    double,
    string,
};

fn usage(argv0: [:0]const u8) !void {
    const stderr = std.io.getStdOut().writer();
    const usage_str =
        \\usage: {s} FILE SYMBOL TYPE
        \\  FILE    an object file (.o, .so)
        \\  SYMBOL  a symbol name
        \\  TYPE    the type of the symbol:
        \\            - i8, i16, i32, i64
        \\            - u8, u16, u32, u64
        \\            - x8, x16, x32, x64
        \\            - [f]loat, [d]ouble
        \\            - [s]tring (null-terminated)
        ;

    try stderr.print(usage_str, .{ std.fs.path.basename(argv0) });
}

fn parseType(str: []const u8) ?SymbolType {
    const type_names = .{
        .{ "i8",     SymbolType.int8 },
        .{ "i16",    SymbolType.int16 },
        .{ "i32",    SymbolType.int32 },
        .{ "i64",    SymbolType.int64 },
        .{ "u8",     SymbolType.uint8 },
        .{ "u16",    SymbolType.uint16 },
        .{ "u32",    SymbolType.uint32 },
        .{ "u64",    SymbolType.uint64 },
        .{ "x8",     SymbolType.xint8 },
        .{ "x16",    SymbolType.xint16 },
        .{ "x32",    SymbolType.xint32 },
        .{ "x64",    SymbolType.xint64 },
        .{ "float",  SymbolType.float },
        .{ "f",      SymbolType.float },
        .{ "double", SymbolType.double },
        .{ "d",      SymbolType.double },
        .{ "string", SymbolType.string },
        .{ "s",      SymbolType.string },
    };

    inline for (type_names) |mapping| {
        if (std.mem.eql(u8, str, mapping[0])) {
            return mapping[1];
        }
    }

    return null;
}

fn printSymbolValue(sym: *anyopaque, sym_type: SymbolType) !void {
    const stdout = std.io.getStdOut().writer();

    const value_types = .{
        .{ SymbolType.int8,   i8,            "{d}\n" },
        .{ SymbolType.int16,  i16,           "{d}\n" },
        .{ SymbolType.int32,  i32,           "{d}\n" },
        .{ SymbolType.int64,  i64,           "{d}\n" },
        .{ SymbolType.uint8,  u8,            "{d}\n" },
        .{ SymbolType.uint16, u16,           "{d}\n" },
        .{ SymbolType.uint32, u32,           "{d}\n" },
        .{ SymbolType.uint64, u64,           "{d}\n" },
        .{ SymbolType.xint8,  u8,            "0x{x}\n" },
        .{ SymbolType.xint16, u16,           "0x{x}\n" },
        .{ SymbolType.xint32, u32,           "0x{x}\n" },
        .{ SymbolType.xint64, u64,           "0x{x}\n" },
        .{ SymbolType.float,  f32,           "{d}\n" },
        .{ SymbolType.double, f64,           "{d}\n" },
        .{ SymbolType.string, [*:0]const u8, "{s}\n" },
    };

    inline for (value_types) |vt| {
        if (sym_type == vt[0]) {
            const T: type = *align(@alignOf(anyopaque)) vt[1];
            try stdout.print(vt[2], .{ @ptrCast(T, sym).* });
        }
    }
}

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    const stderr = std.io.getStdErr().writer();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len != 4) {
        try usage(args[0]);
        exit(1);
    }

    const path = args[1];
    const symbol = args[2];
    const sym_type = args[3];

    const t = parseType(sym_type) orelse {
        try stderr.print("invalid type: {s}\n", .{ sym_type });
        try usage(args[0]);
        exit(1);
    };

    const handle = std.c.dlopen(path, RTLD_NOW) orelse {
        try stderr.print("{?s}\n", .{ dlerror() });
        exit(1);
    };
    defer _ = std.c.dlclose(handle);

    // clear errors, if any
    _ = dlerror();

    const sym_ptr = std.c.dlsym(handle, symbol) orelse {
        try stderr.print("{?s}\n", .{ dlerror() });
        exit(1);
    };

    try printSymbolValue(sym_ptr, t);
}
