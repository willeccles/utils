const std = @import("std");
const exit = std.process.exit;

fn usage(argv0: [:0]const u8) !void {
    const stderr = std.io.getStdOut().writer();
    const usage_str = "usage: {s} R1 R2 [Vin]\n";

    try stderr.print(usage_str, .{ std.fs.path.basename(argv0) });
}

fn dividerScale(r1: f64, r2: f64) f64 {
    return r2 / (r1 + r2);
}

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    const stdout = std.io.getStdOut().writer();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    if (args.len < 3 or args.len > 4) {
        try usage(args[0]);
        exit(1);
    }

    const r1 = try std.fmt.parseFloat(f64, args[1]);
    const r2 = try std.fmt.parseFloat(f64, args[2]);
    const scale = dividerScale(r1, r2);

    try stdout.print("voltage scale: {d:.6}\n", .{ scale });

    if (args.len >= 4) {
        const vin = try std.fmt.parseFloat(f64, args[3]);
        try stdout.print("output voltage (with Vin = {d}): {d:.6}\n", .{ vin, vin * scale });
    }
}
