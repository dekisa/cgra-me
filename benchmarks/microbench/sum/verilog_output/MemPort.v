module MemPort(Config_Clock, Config_Reset, ConfigIn, ConfigOut, in0, in1, in2, in3, out);
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input [31:0] in0;
    input [31:0] in1;
    input [31:0] in2;
    input [31:0] in3;
    output [31:0] out;

    // Wires connecting the main module and submodules
    wire mem_port_config;

    // Declaring the submodules
    memoryPort_4connect_32b mem_port(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(mem_port_config),
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .out(out));
    assign ConfigOut = mem_port_config;
endmodule

