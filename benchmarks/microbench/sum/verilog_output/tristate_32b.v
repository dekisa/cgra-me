module tristate_32b(enable, in, out);
    parameter size = 32;
    // Specifying the ports
    input enable;
    input [size-1:0] in;
    output [size-1:0] out;
    assign out = enable ? in : {size{1'bZ}};
endmodule

