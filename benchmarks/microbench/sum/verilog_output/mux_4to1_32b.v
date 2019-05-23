module mux_4to1_32b(in0, in1, in2, in3, out, select);
    parameter size = 32;
    // Specifying the ports
    input [size-1:0] in0;
    input [size-1:0] in1;
    input [size-1:0] in2;
    input [size-1:0] in3;
    output reg [size-1:0] out;
    input [1:0] select;
    always @(*)
        case (select)
            0: out = in0;
            1: out = in1;
            2: out = in2;
            3: out = in3;
            default: out = {size{1'bx}};
        endcase
endmodule

