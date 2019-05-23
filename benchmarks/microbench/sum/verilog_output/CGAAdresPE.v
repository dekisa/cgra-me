module CGAAdresPE(Config_Clock, Config_Reset, ConfigIn, ConfigOut, in0, in1, in2, in3, in4, in5, in6, in7, out, rf_in_muxa, rf_in_muxout, rf_out);
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input [31:0] in0;
    input [31:0] in1;
    input [31:0] in2;
    input [31:0] in3;
    input [31:0] in4;
    input [31:0] in5;
    input [31:0] in6;
    input [31:0] in7;
    output [31:0] out;
    input [31:0] rf_in_muxa;
    input [31:0] rf_in_muxout;
    output [31:0] rf_out;

    // Wires for the the config cells
    wire [3:0] CGAAdresPE_mux0_Config_sig;
    wire CGAAdresPE_mux0_Config_config;
    wire [3:0] CGAAdresPE_mux1_Config_sig;
    wire CGAAdresPE_mux1_Config_config;
    wire [2:0] CGAAdresPE_mux2_Config_sig;
    wire CGAAdresPE_mux2_Config_config;
    wire func_Config_sig;
    wire func_Config_config;
    wire mux_out_Config_sig;
    wire mux_out_Config_config;

    // Wires connecting the main module and submodules
    wire [31:0] const_out_sig;
    wire [31:0] CGAAdresPE_mux0_out_sig;
    wire [31:0] CGAAdresPE_mux1_out_sig;
    wire [31:0] CGAAdresPE_mux2_out_sig;
    wire const_config;

    // Declaring the config cells
    ConfigCell #(4) CGAAdresPE_mux0_Config (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(CGAAdresPE_mux0_Config_config),
        .select(CGAAdresPE_mux0_Config_sig));
    ConfigCell #(4) CGAAdresPE_mux1_Config (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(CGAAdresPE_mux0_Config_config),
        .ConfigOut(CGAAdresPE_mux1_Config_config),
        .select(CGAAdresPE_mux1_Config_sig));
    ConfigCell #(3) CGAAdresPE_mux2_Config (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(CGAAdresPE_mux1_Config_config),
        .ConfigOut(CGAAdresPE_mux2_Config_config),
        .select(CGAAdresPE_mux2_Config_sig));
    ConfigCell #(1) func_Config (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(CGAAdresPE_mux2_Config_config),
        .ConfigOut(func_Config_config),
        .select(func_Config_sig));
    ConfigCell #(1) mux_out_Config (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(func_Config_config),
        .ConfigOut(mux_out_Config_config),
        .select(mux_out_Config_sig));

    // Declaring the submodules
    mux_10to1_32b #(32) CGAAdresPE_mux0(
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .in4(in4),
        .in5(in5),
        .in6(in6),
        .in7(in7),
        .in8(const_out_sig),
        .in9(rf_in_muxa),
        .out(CGAAdresPE_mux0_out_sig),
        .select(CGAAdresPE_mux0_Config_sig));
    mux_9to1_32b #(32) CGAAdresPE_mux1(
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .in4(in4),
        .in5(in5),
        .in6(in6),
        .in7(in7),
        .in8(const_out_sig),
        .out(CGAAdresPE_mux1_out_sig),
        .select(CGAAdresPE_mux1_Config_sig));
    mux_8to1_32b #(32) CGAAdresPE_mux2(
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .in4(in4),
        .in5(in5),
        .in6(in6),
        .in7(in7),
        .out(CGAAdresPE_mux2_out_sig),
        .select(CGAAdresPE_mux2_Config_sig));
    const_32b #(32) const(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(mux_out_Config_config),
        .ConfigOut(const_config),
        .out(const_out_sig));
    func_32b_add_sub #(32) func(
        .in_a(CGAAdresPE_mux0_out_sig),
        .in_b(CGAAdresPE_mux1_out_sig),
        .out(rf_out),
        .select(func_Config_sig));
    mux_2to1_32b #(32) mux_out(
        .in0(CGAAdresPE_mux2_out_sig),
        .in1(rf_in_muxout),
        .out(out),
        .select(mux_out_Config_sig));
    assign ConfigOut = const_config;
endmodule

