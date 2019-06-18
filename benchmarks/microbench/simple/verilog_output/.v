module (Config_Clock, Config_Reset, ConfigIn, ConfigOut, CGRA_Clock, CGRA_Reset, in0, in1, in2, in3, in4, out);
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input CGRA_Clock, CGRA_Reset;
    input [31:0] in0;
    input [31:0] in1;
    input [31:0] in2;
    input [31:0] in3;
    input [31:0] in4;
    output [31:0] out;

    // Wires for the the config cells
    wire [3:0] FuncConfig_sig;
    wire FuncConfig_config;
    wire [2:0] MuxAConfig_sig;
    wire MuxAConfig_config;
    wire [2:0] MuxBConfig_sig;
    wire MuxBConfig_config;
    wire [2:0] MuxBypassConfig_sig;
    wire MuxBypassConfig_config;
    wire MuxOutConfig_sig;
    wire MuxOutConfig_config;

    // Wires connecting the main module and submodules
    wire [31:0] mux_a_out_sig;
    wire [31:0] mux_b_out_sig;
    wire [31:0] mux_bypass_out_sig;
    wire [31:0] const_out_sig;
    wire [31:0] func_out_sig;
    wire [31:0] register_out_sig;
    wire [31:0] mux_out_out_sig;
    wire const_config;

    // Declaring the config cells
    ConfigCell #(4) FuncConfig (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(FuncConfig_config),
        .select(FuncConfig_sig));
    ConfigCell #(3) MuxAConfig (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(FuncConfig_config),
        .ConfigOut(MuxAConfig_config),
        .select(MuxAConfig_sig));
    ConfigCell #(3) MuxBConfig (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(MuxAConfig_config),
        .ConfigOut(MuxBConfig_config),
        .select(MuxBConfig_sig));
    ConfigCell #(3) MuxBypassConfig (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(MuxBConfig_config),
        .ConfigOut(MuxBypassConfig_config),
        .select(MuxBypassConfig_sig));
    ConfigCell #(1) MuxOutConfig (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(MuxBypassConfig_config),
        .ConfigOut(MuxOutConfig_config),
        .select(MuxOutConfig_sig));

    // Declaring the submodules
    const_32b #(32) const(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(MuxOutConfig_config),
        .ConfigOut(const_config),
        .out(const_out_sig));
    func_32b_add_multiply_sub_divide_and_or_xor_shl_ashr_lshr #(32) func(
        .in_a(mux_a_out_sig),
        .in_b(mux_b_out_sig),
        .out(func_out_sig),
        .select(FuncConfig_sig));
    mux_7to1_32b #(32) mux_a(
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .in4(in4),
        .in5(mux_out_out_sig),
        .in6(const_out_sig),
        .out(mux_a_out_sig),
        .select(MuxAConfig_sig));
    mux_7to1_32b #(32) mux_b(
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .in4(in4),
        .in5(mux_out_out_sig),
        .in6(const_out_sig),
        .out(mux_b_out_sig),
        .select(MuxBConfig_sig));
    mux_5to1_32b #(32) mux_bypass(
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .in4(in4),
        .out(mux_bypass_out_sig),
        .select(MuxBypassConfig_sig));
    mux_2to1_32b #(32) mux_out(
        .in0(register_out_sig),
        .in1(mux_bypass_out_sig),
        .out(mux_out_out_sig),
        .select(MuxOutConfig_sig));
    register_32b #(32) register(
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in(func_out_sig),
        .out(register_out_sig));
    assign out = mux_out_out_sig;
    assign ConfigOut = const_config;
endmodule

