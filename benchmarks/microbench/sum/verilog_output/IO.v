module IO(Config_Clock, Config_Reset, ConfigIn, ConfigOut, CGRA_Clock, CGRA_Reset, bidir, in, out);
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input CGRA_Clock, CGRA_Reset;
    inout [31:0] bidir;
    input [31:0] in;
    output [31:0] out;

    // Wires connecting the main module and submodules
    wire io_config;

    // Declaring the submodules
    io_32b #(32) io(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(io_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .bidir(bidir),
        .in(in),
        .out(out));
    assign ConfigOut = io_config;
endmodule

