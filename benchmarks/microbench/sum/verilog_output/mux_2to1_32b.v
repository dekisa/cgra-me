module mux_2to1_32b(in0, in1, out, select);
    parameter size = 32;
    // Specifying the ports
    input [size-1:0] in0;
    input [size-1:0] in1;
    output reg [size-1:0] out;
    input select;
    always @(*)
        case (select)
            0: out = in0;
            1: out = in1;
            default: out = {size{1'bx}};
        endcase
endmodule

