module register_32b(CGRA_Clock, CGRA_Reset, in, out);
    parameter size = 32;
    // Specifying the ports
    input CGRA_Clock, CGRA_Reset;
    input [size-1:0] in;
    output reg [size-1:0] out;
    always @(posedge CGRA_Clock, posedge CGRA_Reset)
        if (CGRA_Reset == 1)
            out <= 0;
        else
            out <= in;
endmodule

