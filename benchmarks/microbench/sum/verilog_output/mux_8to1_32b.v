module mux_8to1_32b(in0, in1, in2, in3, in4, in5, in6, in7, out, select);
    parameter size = 32;
    // Specifying the ports
    input [size-1:0] in0;
    input [size-1:0] in1;
    input [size-1:0] in2;
    input [size-1:0] in3;
    input [size-1:0] in4;
    input [size-1:0] in5;
    input [size-1:0] in6;
    input [size-1:0] in7;
    output reg [size-1:0] out;
    input [2:0] select;
    always @(*)
        case (select)
            0: out = in0;
            1: out = in1;
            2: out = in2;
            3: out = in3;
            4: out = in4;
            5: out = in5;
            6: out = in6;
            7: out = in7;
            default: out = {size{1'bx}};
        endcase
endmodule

