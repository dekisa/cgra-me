module registerFile_4in_8out_32b(CGRA_Clock, CGRA_Reset, WE0, WE1, WE2, WE3, address_in0, address_in1, address_in2, address_in3, address_out0, address_out1, address_out2, address_out3, address_out4, address_out5, address_out6, address_out7, in0, in1, in2, in3, out0, out1, out2, out3, out4, out5, out6, out7);
    parameter log2regs = 3;
    parameter size = 32;
    // Specifying the ports
    input CGRA_Clock, CGRA_Reset;
    input WE0;
    input WE1;
    input WE2;
    input WE3;
    input [log2regs-1:0] address_in0;
    input [log2regs-1:0] address_in1;
    input [log2regs-1:0] address_in2;
    input [log2regs-1:0] address_in3;
    input [log2regs-1:0] address_out0;
    input [log2regs-1:0] address_out1;
    input [log2regs-1:0] address_out2;
    input [log2regs-1:0] address_out3;
    input [log2regs-1:0] address_out4;
    input [log2regs-1:0] address_out5;
    input [log2regs-1:0] address_out6;
    input [log2regs-1:0] address_out7;
    input [size-1:0] in0;
    input [size-1:0] in1;
    input [size-1:0] in2;
    input [size-1:0] in3;
    output reg [size-1:0] out0;
    output reg [size-1:0] out1;
    output reg [size-1:0] out2;
    output reg [size-1:0] out3;
    output reg [size-1:0] out4;
    output reg [size-1:0] out5;
    output reg [size-1:0] out6;
    output reg [size-1:0] out7;

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
                out2 = register[address_out2];
                out3 = register[address_out3];
                out4 = register[address_out4];
                out5 = register[address_out5];
                out6 = register[address_out6];
                out7 = register[address_out7];
                if(WE0)
                    register[address_in0] = in0;
                if(WE1)
                    register[address_in1] = in1;
                if(WE2)
                    register[address_in2] = in2;
                if(WE3)
                    register[address_in3] = in3;
            end
endmodule

