module io_32b(Config_Clock, Config_Reset, ConfigIn, ConfigOut, CGRA_Clock, CGRA_Reset, bidir, in, out);
    parameter size = 32;
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input CGRA_Clock, CGRA_Reset;
    inout [size-1:0] bidir;
    input [size-1:0] in;
    output [size-1:0] out;

    // Wires for the the config cells
    wire OEConfig_sig;
    wire OEConfig_config;

    // Wires connecting the main module and submodules
    wire [31:0] reg_in_out_sig;

    // Declaring the config cells
    ConfigCell #(1) OEConfig (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(OEConfig_config),
        .select(OEConfig_sig));

    // Declaring the submodules
    tristate_32b #(size) OE(
        .enable(OEConfig_sig),
        .in(reg_in_out_sig),
        .out(bidir));
    register_32b #(size) reg_in(
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in(in),
        .out(reg_in_out_sig));
    register_32b #(size) reg_out(
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in(bidir),
        .out(out));
    assign ConfigOut = OEConfig_config;
endmodule

