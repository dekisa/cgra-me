module cgra(Config_Clock, Config_Reset, ConfigIn, ConfigOut, CGRA_Clock, CGRA_Reset, bidir0, bidir1, bidir2, bidir3);
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input CGRA_Clock, CGRA_Reset;
    inout [31:0] bidir0;
    inout [31:0] bidir1;
    inout [31:0] bidir2;
    inout [31:0] bidir3;

    // Wires connecting the main module and submodules
    wire [31:0] block_1_0_out_sig;
    wire [31:0] block_4_0_out0_sig;
    wire [31:0] block_0_0_out0_sig;
    wire [31:0] block_0_0_out1_sig;
    wire [31:0] block_0_0_out2_sig;
    wire [31:0] block_0_0_out3_sig;
    wire [31:0] block_0_0_out4_sig;
    wire [31:0] block_0_0_out5_sig;
    wire [31:0] block_0_0_out6_sig;
    wire [31:0] block_0_0_out7_sig;
    wire [31:0] block_1_1_out_sig;
    wire [31:0] block_1_2_out_sig;
    wire [31:0] block_1_3_out_sig;
    wire [31:0] block_2_0_out_sig;
    wire [31:0] block_5_0_out_sig;
    wire [31:0] block_8_0_out_sig;
    wire [31:0] block_11_0_out_sig;
    wire [31:0] block_4_1_out0_sig;
    wire [31:0] block_4_1_out1_sig;
    wire [31:0] block_4_2_out0_sig;
    wire [31:0] block_4_2_out1_sig;
    wire [31:0] block_4_3_out0_sig;
    wire [31:0] block_4_3_out1_sig;
    wire [31:0] block_4_0_out1_sig;
    wire [31:0] block_3_0_out_sig;
    wire [31:0] block_3_0_rf_out_sig;
    wire [31:0] block_3_1_out_sig;
    wire [31:0] block_3_1_rf_out_sig;
    wire [31:0] block_3_2_out_sig;
    wire [31:0] block_3_2_rf_out_sig;
    wire [31:0] block_3_3_out_sig;
    wire [31:0] block_3_3_rf_out_sig;
    wire [31:0] block_7_0_out0_sig;
    wire [31:0] block_7_0_out1_sig;
    wire [31:0] block_7_1_out0_sig;
    wire [31:0] block_7_1_out1_sig;
    wire [31:0] block_7_2_out0_sig;
    wire [31:0] block_7_2_out1_sig;
    wire [31:0] block_7_3_out0_sig;
    wire [31:0] block_7_3_out1_sig;
    wire [31:0] block_10_0_out0_sig;
    wire [31:0] block_10_0_out1_sig;
    wire [31:0] block_10_1_out0_sig;
    wire [31:0] block_10_1_out1_sig;
    wire [31:0] block_10_2_out0_sig;
    wire [31:0] block_10_2_out1_sig;
    wire [31:0] block_10_3_out0_sig;
    wire [31:0] block_10_3_out1_sig;
    wire [31:0] block_6_0_out_sig;
    wire [31:0] block_6_0_rf_out_sig;
    wire [31:0] block_6_1_out_sig;
    wire [31:0] block_6_1_rf_out_sig;
    wire [31:0] block_6_2_out_sig;
    wire [31:0] block_6_2_rf_out_sig;
    wire [31:0] block_6_3_out_sig;
    wire [31:0] block_6_3_rf_out_sig;
    wire [31:0] block_9_0_out_sig;
    wire [31:0] block_9_0_rf_out_sig;
    wire [31:0] block_9_1_out_sig;
    wire [31:0] block_9_1_rf_out_sig;
    wire [31:0] block_9_2_out_sig;
    wire [31:0] block_9_2_rf_out_sig;
    wire [31:0] block_9_3_out_sig;
    wire [31:0] block_9_3_rf_out_sig;
    wire [31:0] block_12_0_out_sig;
    wire [31:0] block_12_0_rf_out_sig;
    wire [31:0] block_12_1_out_sig;
    wire [31:0] block_12_1_rf_out_sig;
    wire [31:0] block_12_2_out_sig;
    wire [31:0] block_12_2_rf_out_sig;
    wire [31:0] block_12_3_out_sig;
    wire [31:0] block_12_3_rf_out_sig;
    wire block_0_0_config;
    wire block_10_0_config;
    wire block_10_1_config;
    wire block_10_2_config;
    wire block_10_3_config;
    wire block_11_0_config;
    wire block_12_0_config;
    wire block_12_1_config;
    wire block_12_2_config;
    wire block_12_3_config;
    wire block_1_0_config;
    wire block_1_1_config;
    wire block_1_2_config;
    wire block_1_3_config;
    wire block_2_0_config;
    wire block_3_0_config;
    wire block_3_1_config;
    wire block_3_2_config;
    wire block_3_3_config;
    wire block_4_0_config;
    wire block_4_1_config;
    wire block_4_2_config;
    wire block_4_3_config;
    wire block_5_0_config;
    wire block_6_0_config;
    wire block_6_1_config;
    wire block_6_2_config;
    wire block_6_3_config;
    wire block_7_0_config;
    wire block_7_1_config;
    wire block_7_2_config;
    wire block_7_3_config;
    wire block_8_0_config;
    wire block_9_0_config;
    wire block_9_1_config;
    wire block_9_2_config;
    wire block_9_3_config;

    // Declaring the submodules
    globalRF block_0_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(block_0_0_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_3_0_rf_out_sig),
        .in1(block_3_1_rf_out_sig),
        .in2(block_3_2_rf_out_sig),
        .in3(block_3_3_rf_out_sig),
        .out0(block_0_0_out0_sig),
        .out1(block_0_0_out1_sig),
        .out2(block_0_0_out2_sig),
        .out3(block_0_0_out3_sig),
        .out4(block_0_0_out4_sig),
        .out5(block_0_0_out5_sig),
        .out6(block_0_0_out6_sig),
        .out7(block_0_0_out7_sig));
    externRF1_1 block_10_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_0_0_config),
        .ConfigOut(block_10_0_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_12_0_rf_out_sig),
        .out0(block_10_0_out0_sig),
        .out1(block_10_0_out1_sig));
    externRF1_1 block_10_1(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_10_0_config),
        .ConfigOut(block_10_1_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_12_1_rf_out_sig),
        .out0(block_10_1_out0_sig),
        .out1(block_10_1_out1_sig));
    externRF1_1 block_10_2(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_10_1_config),
        .ConfigOut(block_10_2_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_12_2_rf_out_sig),
        .out0(block_10_2_out0_sig),
        .out1(block_10_2_out1_sig));
    externRF1_1 block_10_3(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_10_2_config),
        .ConfigOut(block_10_3_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_12_3_rf_out_sig),
        .out0(block_10_3_out0_sig),
        .out1(block_10_3_out1_sig));
    MemPort block_11_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_10_3_config),
        .ConfigOut(block_11_0_config),
        .in0(block_12_0_out_sig),
        .in1(block_12_1_out_sig),
        .in2(block_12_2_out_sig),
        .in3(block_12_3_out_sig),
        .out(block_11_0_out_sig));
    CGAAdresPE block_12_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_11_0_config),
        .ConfigOut(block_12_0_config),
        .in0(block_9_0_out_sig),
        .in1(block_12_1_out_sig),
        .in2(0) /* A PORT IS UNCONNECTED */,
        .in3(0) /* A PORT IS UNCONNECTED */,
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_11_0_out_sig),
        .out(block_12_0_out_sig),
        .rf_in_muxa(block_10_0_out0_sig),
        .rf_in_muxout(block_10_0_out1_sig),
        .rf_out(block_12_0_rf_out_sig));
    CGAAdresPE block_12_1(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_12_0_config),
        .ConfigOut(block_12_1_config),
        .in0(block_9_1_out_sig),
        .in1(block_12_2_out_sig),
        .in2(0) /* A PORT IS UNCONNECTED */,
        .in3(block_12_0_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_11_0_out_sig),
        .out(block_12_1_out_sig),
        .rf_in_muxa(block_10_1_out0_sig),
        .rf_in_muxout(block_10_1_out1_sig),
        .rf_out(block_12_1_rf_out_sig));
    CGAAdresPE block_12_2(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_12_1_config),
        .ConfigOut(block_12_2_config),
        .in0(block_9_2_out_sig),
        .in1(block_12_3_out_sig),
        .in2(0) /* A PORT IS UNCONNECTED */,
        .in3(block_12_1_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_11_0_out_sig),
        .out(block_12_2_out_sig),
        .rf_in_muxa(block_10_2_out0_sig),
        .rf_in_muxout(block_10_2_out1_sig),
        .rf_out(block_12_2_rf_out_sig));
    CGAAdresPE block_12_3(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_12_2_config),
        .ConfigOut(block_12_3_config),
        .in0(block_9_3_out_sig),
        .in1(0) /* A PORT IS UNCONNECTED */,
        .in2(0) /* A PORT IS UNCONNECTED */,
        .in3(block_12_2_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_11_0_out_sig),
        .out(block_12_3_out_sig),
        .rf_in_muxa(block_10_3_out0_sig),
        .rf_in_muxout(block_10_3_out1_sig),
        .rf_out(block_12_3_rf_out_sig));
    IO block_1_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_12_3_config),
        .ConfigOut(block_1_0_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .bidir(bidir0),
        .in(block_3_0_out_sig),
        .out(block_1_0_out_sig));
    IO block_1_1(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_1_0_config),
        .ConfigOut(block_1_1_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .bidir(bidir1),
        .in(block_3_1_out_sig),
        .out(block_1_1_out_sig));
    IO block_1_2(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_1_1_config),
        .ConfigOut(block_1_2_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .bidir(bidir2),
        .in(block_3_2_out_sig),
        .out(block_1_2_out_sig));
    IO block_1_3(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_1_2_config),
        .ConfigOut(block_1_3_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .bidir(bidir3),
        .in(block_3_3_out_sig),
        .out(block_1_3_out_sig));
    MemPort block_2_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_1_3_config),
        .ConfigOut(block_2_0_config),
        .in0(block_3_0_out_sig),
        .in1(block_3_1_out_sig),
        .in2(block_3_2_out_sig),
        .in3(block_3_3_out_sig),
        .out(block_2_0_out_sig));
    VLIWAdresPE block_3_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_2_0_config),
        .ConfigOut(block_3_0_config),
        .in0(block_1_0_out_sig),
        .in1(block_3_1_out_sig),
        .in2(block_6_0_out_sig),
        .in3(0) /* A PORT IS UNCONNECTED */,
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_2_0_out_sig),
        .out(block_3_0_out_sig),
        .rf_in_muxa(block_0_0_out0_sig),
        .rf_in_muxout(block_0_0_out1_sig),
        .rf_out(block_3_0_rf_out_sig));
    VLIWAdresPE block_3_1(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_3_0_config),
        .ConfigOut(block_3_1_config),
        .in0(block_1_1_out_sig),
        .in1(block_3_2_out_sig),
        .in2(block_6_1_out_sig),
        .in3(block_3_0_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_2_0_out_sig),
        .out(block_3_1_out_sig),
        .rf_in_muxa(block_0_0_out2_sig),
        .rf_in_muxout(block_0_0_out3_sig),
        .rf_out(block_3_1_rf_out_sig));
    VLIWAdresPE block_3_2(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_3_1_config),
        .ConfigOut(block_3_2_config),
        .in0(block_1_2_out_sig),
        .in1(block_3_3_out_sig),
        .in2(block_6_2_out_sig),
        .in3(block_3_1_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_2_0_out_sig),
        .out(block_3_2_out_sig),
        .rf_in_muxa(block_0_0_out4_sig),
        .rf_in_muxout(block_0_0_out5_sig),
        .rf_out(block_3_2_rf_out_sig));
    VLIWAdresPE block_3_3(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_3_2_config),
        .ConfigOut(block_3_3_config),
        .in0(block_1_3_out_sig),
        .in1(0) /* A PORT IS UNCONNECTED */,
        .in2(block_6_3_out_sig),
        .in3(block_3_2_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_2_0_out_sig),
        .out(block_3_3_out_sig),
        .rf_in_muxa(block_0_0_out6_sig),
        .rf_in_muxout(block_0_0_out7_sig),
        .rf_out(block_3_3_rf_out_sig));
    externRF1_1 block_4_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_3_3_config),
        .ConfigOut(block_4_0_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_6_0_rf_out_sig),
        .out0(block_4_0_out0_sig),
        .out1(block_4_0_out1_sig));
    externRF1_1 block_4_1(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_4_0_config),
        .ConfigOut(block_4_1_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_6_1_rf_out_sig),
        .out0(block_4_1_out0_sig),
        .out1(block_4_1_out1_sig));
    externRF1_1 block_4_2(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_4_1_config),
        .ConfigOut(block_4_2_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_6_2_rf_out_sig),
        .out0(block_4_2_out0_sig),
        .out1(block_4_2_out1_sig));
    externRF1_1 block_4_3(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_4_2_config),
        .ConfigOut(block_4_3_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_6_3_rf_out_sig),
        .out0(block_4_3_out0_sig),
        .out1(block_4_3_out1_sig));
    MemPort block_5_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_4_3_config),
        .ConfigOut(block_5_0_config),
        .in0(block_6_0_out_sig),
        .in1(block_6_1_out_sig),
        .in2(block_6_2_out_sig),
        .in3(block_6_3_out_sig),
        .out(block_5_0_out_sig));
    CGAAdresPE block_6_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_5_0_config),
        .ConfigOut(block_6_0_config),
        .in0(block_3_0_out_sig),
        .in1(block_6_1_out_sig),
        .in2(block_9_0_out_sig),
        .in3(0) /* A PORT IS UNCONNECTED */,
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_5_0_out_sig),
        .out(block_6_0_out_sig),
        .rf_in_muxa(block_4_0_out0_sig),
        .rf_in_muxout(block_4_0_out1_sig),
        .rf_out(block_6_0_rf_out_sig));
    CGAAdresPE block_6_1(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_6_0_config),
        .ConfigOut(block_6_1_config),
        .in0(block_3_1_out_sig),
        .in1(block_6_2_out_sig),
        .in2(block_9_1_out_sig),
        .in3(block_6_0_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_5_0_out_sig),
        .out(block_6_1_out_sig),
        .rf_in_muxa(block_4_1_out0_sig),
        .rf_in_muxout(block_4_1_out1_sig),
        .rf_out(block_6_1_rf_out_sig));
    CGAAdresPE block_6_2(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_6_1_config),
        .ConfigOut(block_6_2_config),
        .in0(block_3_2_out_sig),
        .in1(block_6_3_out_sig),
        .in2(block_9_2_out_sig),
        .in3(block_6_1_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_5_0_out_sig),
        .out(block_6_2_out_sig),
        .rf_in_muxa(block_4_2_out0_sig),
        .rf_in_muxout(block_4_2_out1_sig),
        .rf_out(block_6_2_rf_out_sig));
    CGAAdresPE block_6_3(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_6_2_config),
        .ConfigOut(block_6_3_config),
        .in0(block_3_3_out_sig),
        .in1(0) /* A PORT IS UNCONNECTED */,
        .in2(block_9_3_out_sig),
        .in3(block_6_2_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_5_0_out_sig),
        .out(block_6_3_out_sig),
        .rf_in_muxa(block_4_3_out0_sig),
        .rf_in_muxout(block_4_3_out1_sig),
        .rf_out(block_6_3_rf_out_sig));
    externRF1_1 block_7_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_6_3_config),
        .ConfigOut(block_7_0_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_9_0_rf_out_sig),
        .out0(block_7_0_out0_sig),
        .out1(block_7_0_out1_sig));
    externRF1_1 block_7_1(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_7_0_config),
        .ConfigOut(block_7_1_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_9_1_rf_out_sig),
        .out0(block_7_1_out0_sig),
        .out1(block_7_1_out1_sig));
    externRF1_1 block_7_2(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_7_1_config),
        .ConfigOut(block_7_2_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_9_2_rf_out_sig),
        .out0(block_7_2_out0_sig),
        .out1(block_7_2_out1_sig));
    externRF1_1 block_7_3(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_7_2_config),
        .ConfigOut(block_7_3_config),
        .CGRA_Clock(CGRA_Clock),
        .CGRA_Reset(CGRA_Reset),
        .in0(block_9_3_rf_out_sig),
        .out0(block_7_3_out0_sig),
        .out1(block_7_3_out1_sig));
    MemPort block_8_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_7_3_config),
        .ConfigOut(block_8_0_config),
        .in0(block_9_0_out_sig),
        .in1(block_9_1_out_sig),
        .in2(block_9_2_out_sig),
        .in3(block_9_3_out_sig),
        .out(block_8_0_out_sig));
    CGAAdresPE block_9_0(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_8_0_config),
        .ConfigOut(block_9_0_config),
        .in0(block_6_0_out_sig),
        .in1(block_9_1_out_sig),
        .in2(block_12_0_out_sig),
        .in3(0) /* A PORT IS UNCONNECTED */,
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_8_0_out_sig),
        .out(block_9_0_out_sig),
        .rf_in_muxa(block_7_0_out0_sig),
        .rf_in_muxout(block_7_0_out1_sig),
        .rf_out(block_9_0_rf_out_sig));
    CGAAdresPE block_9_1(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_9_0_config),
        .ConfigOut(block_9_1_config),
        .in0(block_6_1_out_sig),
        .in1(block_9_2_out_sig),
        .in2(block_12_1_out_sig),
        .in3(block_9_0_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_8_0_out_sig),
        .out(block_9_1_out_sig),
        .rf_in_muxa(block_7_1_out0_sig),
        .rf_in_muxout(block_7_1_out1_sig),
        .rf_out(block_9_1_rf_out_sig));
    CGAAdresPE block_9_2(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_9_1_config),
        .ConfigOut(block_9_2_config),
        .in0(block_6_2_out_sig),
        .in1(block_9_3_out_sig),
        .in2(block_12_2_out_sig),
        .in3(block_9_1_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_8_0_out_sig),
        .out(block_9_2_out_sig),
        .rf_in_muxa(block_7_2_out0_sig),
        .rf_in_muxout(block_7_2_out1_sig),
        .rf_out(block_9_2_rf_out_sig));
    CGAAdresPE block_9_3(
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(block_9_2_config),
        .ConfigOut(block_9_3_config),
        .in0(block_6_3_out_sig),
        .in1(0) /* A PORT IS UNCONNECTED */,
        .in2(block_12_3_out_sig),
        .in3(block_9_2_out_sig),
        .in4(0) /* A PORT IS UNCONNECTED */,
        .in5(0) /* A PORT IS UNCONNECTED */,
        .in6(0) /* A PORT IS UNCONNECTED */,
        .in7(block_8_0_out_sig),
        .out(block_9_3_out_sig),
        .rf_in_muxa(block_7_3_out0_sig),
        .rf_in_muxout(block_7_3_out1_sig),
        .rf_out(block_9_3_rf_out_sig));
    assign ConfigOut = block_9_3_config;
endmodule

