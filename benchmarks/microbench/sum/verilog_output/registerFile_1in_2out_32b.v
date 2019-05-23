module registerFile_1in_2out_32b(CGRA_Clock, CGRA_Reset, WE0, address_in0, address_out0, address_out1, in0, out0, out1);
    parameter log2regs = 1;
    parameter size = 32;
    // Specifying the ports
    input CGRA_Clock, CGRA_Reset;
    input WE0;
    input [log2regs-1:0] address_in0;
    input [log2regs-1:0] address_out0;
    input [log2regs-1:0] address_out1;
    input [size-1:0] in0;
    output reg [size-1:0] out0;
    output reg [size-1:0] out1;

    // Setting the always blocks and inside registers
    reg [size-1:0] register[2**log2regs-1:0];
    always@(posedge CGRA_Clock, posedge CGRA_Reset)
        if(CGRA_Reset)
            begin : RESET
                integer i;
                for (i = 0; i < 2**log2regs; i = i+1)
                    register[i] = 0;
            end
        else
            begin
                out0 = register[address_out0];
                out1 = register[address_out1];
                if(WE0)
                    register[address_in0] = in0;
            end
endmodule

