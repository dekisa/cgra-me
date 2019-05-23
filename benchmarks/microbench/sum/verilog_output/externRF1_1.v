module externRF1_1(Config_Clock, Config_Reset, ConfigIn, ConfigOut, CGRA_Clock, CGRA_Reset, in0, out0, out1);
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input CGRA_Clock, CGRA_Reset;
    input [31:0] in0;
    output [31:0] out0;
    output [31:0] out1;

    // Wires for the the config cells
    wire rf_WE0_sig;
    wire rf_WE0_config;
    wire rf_addr_i0_sig;
    wire rf_addr_i0_config;
    wire rf_addr_o0_sig;
    wire rf_addr_o0_config;
    wire rf_addr_o1_sig;
    wire rf_addr_o1_config;

    // Wires connecting the main module and submodules

    // Declaring the config cells
    ConfigCell #(1) rf_WE0 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(rf_WE0_config),
        .select(rf_WE0_sig));
    ConfigCell #(1) rf_addr_i0 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_WE0_config),
        .ConfigOut(rf_addr_i0_config),
        .select(rf_addr_i0_sig));
    ConfigCell #(1) rf_addr_o0 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_i0_config),
        .ConfigOut(rf_addr_o0_config),
        .select(rf_addr_o0_sig));
    ConfigCell #(1) rf_addr_o1 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_o0_config),
        .ConfigOut(rf_addr_o1_config),
        .select(rf_addr_o1_sig));

    // Declaring the submodules
    registerFile_1in_2out_32b #(1, 32) rf(
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .WE0(rf_WE0_sig),
        .address_in0(rf_addr_i0_sig),
        .address_out0(rf_addr_o0_sig),
        .address_out1(rf_addr_o1_sig),
        .in0(in0),
        .out0(out0),
        .out1(out1));
    assign ConfigOut = rf_addr_o1_config;
endmodule

