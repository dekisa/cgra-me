module func_32b_add_sub(in_a, in_b, out, select);
    parameter size = 32;
    // Specifying the ports
    input [size-1:0] in_a;
    input [size-1:0] in_b;
    output reg [size-1:0] out;
    input select;

    // Declaring wires to direct module output into multiplexer
    wire [size-1:0] add_sel;
    wire [size-1:0] sub_sel;

    // Declaring the submodules
    op_add #(size) op_add(.a(in_a), .b(in_b), .c(add_sel));
    op_sub #(size) op_sub(.a(in_a), .b(in_b), .c(sub_sel));

    always @*
        case (select)
            0: out = add_sel;
            1: out = sub_sel;
            default: out = {size{1'bx}};
        endcase
endmodule

