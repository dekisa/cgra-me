module func_32b_add_sub_multiply_divide_and_or_xor_shl_ashr_lshr(in_a, in_b, out, select);
    parameter size = 32;
    // Specifying the ports
    input [size-1:0] in_a;
    input [size-1:0] in_b;
    output reg [size-1:0] out;
    input [3:0] select;

    // Declaring wires to direct module output into multiplexer
    wire [size-1:0] add_sel;
    wire [size-1:0] sub_sel;
    wire [size-1:0] mul_sel;
    wire [size-1:0] div_sel;
    wire [size-1:0] and_sel;
    wire [size-1:0] or_sel;
    wire [size-1:0] xor_sel;
    wire [size-1:0] shl_sel;
    wire [size-1:0] ashr_sel;
    wire [size-1:0] lshr_sel;

    // Declaring the submodules
    op_add #(size) op_add(.a(in_a), .b(in_b), .c(add_sel));
    op_sub #(size) op_sub(.a(in_a), .b(in_b), .c(sub_sel));
    op_multiply #(size) op_multiply(.a(in_a), .b(in_b), .c(mul_sel));
    op_divide #(size) op_divide(.a(in_a), .b(in_b), .c(div_sel));
    op_and #(size) op_and(.a(in_a), .b(in_b), .c(and_sel));
    op_or #(size) op_or(.a(in_a), .b(in_b), .c(or_sel));
    op_xor #(size) op_xor(.a(in_a), .b(in_b), .c(xor_sel));
    op_shl #(size) op_shl(.a(in_a), .b(in_b), .c(shl_sel));
    op_ashr #(size) op_ashr(.a(in_a), .b(in_b), .c(ashr_sel));
    op_lshr #(size) op_lshr(.a(in_a), .b(in_b), .c(lshr_sel));

    always @*
        case (select)
            0: out = add_sel;
            1: out = sub_sel;
            2: out = mul_sel;
            3: out = div_sel;
            4: out = and_sel;
            5: out = or_sel;
            6: out = xor_sel;
            7: out = shl_sel;
            8: out = ashr_sel;
            9: out = lshr_sel;
            default: out = {size{1'bx}};
        endcase
endmodule

