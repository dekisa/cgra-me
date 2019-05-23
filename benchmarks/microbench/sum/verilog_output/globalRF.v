module globalRF(Config_Clock, Config_Reset, ConfigIn, ConfigOut, CGRA_Clock, CGRA_Reset, in0, in1, in2, in3, out0, out1, out2, out3, out4, out5, out6, out7);
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input CGRA_Clock, CGRA_Reset;
    input [31:0] in0;
    input [31:0] in1;
    input [31:0] in2;
    input [31:0] in3;
    output [31:0] out0;
    output [31:0] out1;
    output [31:0] out2;
    output [31:0] out3;
    output [31:0] out4;
    output [31:0] out5;
    output [31:0] out6;
    output [31:0] out7;

    // Wires for the the config cells
    wire rf_WE0_sig;
    wire rf_WE0_config;
    wire rf_WE1_sig;
    wire rf_WE1_config;
    wire rf_WE2_sig;
    wire rf_WE2_config;
    wire rf_WE3_sig;
    wire rf_WE3_config;
    wire [2:0] rf_addr_i0_sig;
    wire rf_addr_i0_config;
    wire [2:0] rf_addr_i1_sig;
    wire rf_addr_i1_config;
    wire [2:0] rf_addr_i2_sig;
    wire rf_addr_i2_config;
    wire [2:0] rf_addr_i3_sig;
    wire rf_addr_i3_config;
    wire [2:0] rf_addr_o0_sig;
    wire rf_addr_o0_config;
    wire [2:0] rf_addr_o1_sig;
    wire rf_addr_o1_config;
    wire [2:0] rf_addr_o2_sig;
    wire rf_addr_o2_config;
    wire [2:0] rf_addr_o3_sig;
    wire rf_addr_o3_config;
    wire [2:0] rf_addr_o4_sig;
    wire rf_addr_o4_config;
    wire [2:0] rf_addr_o5_sig;
    wire rf_addr_o5_config;
    wire [2:0] rf_addr_o6_sig;
    wire rf_addr_o6_config;
    wire [2:0] rf_addr_o7_sig;
    wire rf_addr_o7_config;

    // Wires connecting the main module and submodules

    // Declaring the config cells
    ConfigCell #(1) rf_WE0 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(rf_WE0_config),
        .select(rf_WE0_sig));
    ConfigCell #(1) rf_WE1 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_WE0_config),
        .ConfigOut(rf_WE1_config),
        .select(rf_WE1_sig));
    ConfigCell #(1) rf_WE2 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_WE1_config),
        .ConfigOut(rf_WE2_config),
        .select(rf_WE2_sig));
    ConfigCell #(1) rf_WE3 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_WE2_config),
        .ConfigOut(rf_WE3_config),
        .select(rf_WE3_sig));
    ConfigCell #(3) rf_addr_i0 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_WE3_config),
        .ConfigOut(rf_addr_i0_config),
        .select(rf_addr_i0_sig));
    ConfigCell #(3) rf_addr_i1 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_i0_config),
        .ConfigOut(rf_addr_i1_config),
        .select(rf_addr_i1_sig));
    ConfigCell #(3) rf_addr_i2 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_i1_config),
        .ConfigOut(rf_addr_i2_config),
        .select(rf_addr_i2_sig));
    ConfigCell #(3) rf_addr_i3 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_i2_config),
        .ConfigOut(rf_addr_i3_config),
        .select(rf_addr_i3_sig));
    ConfigCell #(3) rf_addr_o0 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_i3_config),
        .ConfigOut(rf_addr_o0_config),
        .select(rf_addr_o0_sig));
    ConfigCell #(3) rf_addr_o1 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_o0_config),
        .ConfigOut(rf_addr_o1_config),
        .select(rf_addr_o1_sig));
    ConfigCell #(3) rf_addr_o2 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_o1_config),
        .ConfigOut(rf_addr_o2_config),
        .select(rf_addr_o2_sig));
    ConfigCell #(3) rf_addr_o3 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_o2_config),
        .ConfigOut(rf_addr_o3_config),
        .select(rf_addr_o3_sig));
    ConfigCell #(3) rf_addr_o4 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_o3_config),
        .ConfigOut(rf_addr_o4_config),
        .select(rf_addr_o4_sig));
    ConfigCell #(3) rf_addr_o5 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_o4_config),
        .ConfigOut(rf_addr_o5_config),
        .select(rf_addr_o5_sig));
    ConfigCell #(3) rf_addr_o6 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_o5_config),
        .ConfigOut(rf_addr_o6_config),
        .select(rf_addr_o6_sig));
    ConfigCell #(3) rf_addr_o7 (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(rf_addr_o6_config),
        .ConfigOut(rf_addr_o7_config),
        .select(rf_addr_o7_sig));

    // Declaring the submodules
    registerFile_4in_8out_32b #(3, 32) rf(
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .WE0(rf_WE0_sig),
        .WE1(rf_WE1_sig),
        .WE2(rf_WE2_sig),
        .WE3(rf_WE3_sig),
        .address_in0(rf_addr_i0_sig),
        .address_in1(rf_addr_i1_sig),
        .address_in2(rf_addr_i2_sig),
        .address_in3(rf_addr_i3_sig),
        .address_out0(rf_addr_o0_sig),
        .address_out1(rf_addr_o1_sig),
        .address_out2(rf_addr_o2_sig),
        .address_out3(rf_addr_o3_sig),
        .address_out4(rf_addr_o4_sig),
        .address_out5(rf_addr_o5_sig),
        .address_out6(rf_addr_o6_sig),
        .address_out7(rf_addr_o7_sig),
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .out0(out0),
        .out1(out1),
        .out2(out2),
        .out3(out3),
        .out4(out4),
        .out5(out5),
        .out6(out6),
        .out7(out7));
    assign ConfigOut = rf_addr_o7_config;
endmodule

