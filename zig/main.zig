const std = @import("std");

const Token = struct {
    type: TokenType,
    literal: []const u8,

    pub fn format(
        self: Token,
        comptime options: []const u8,
        fmt: std.fmt.FormatOptions,
        writer: anytype,
    ) !void {
        // Unused here.
        _ = options;
        _ = fmt;

        try writer.print("TYPE: {s} LITERAL: {s}", .{
            @tagName(self.type),
            self.literal,
        });
    }
};
const TokenType = enum {
    ILLEGAL,
    EOF_TOKEN,
    // Identifiers + literals
    IDENT, // add, foobar, x, y, ...
    INT, // 1343456
    // Operators
    ASSIGN,
    PLUS,
    // Delimiters
    COMMA,
    SEMICOLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    // Keywords
    FUNCTION,
    LET,
};

pub fn main() !void {
    const token = Token{
        .type = .IDENT,
        .literal = "x",
    };
    std.debug.print("Token: {}\n", .{token});
}
