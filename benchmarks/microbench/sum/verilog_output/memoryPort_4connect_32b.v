module memoryPort_4connect_32b(Config_Clock, Config_Reset, ConfigIn, ConfigOut, in0, in1, in2, in3, out);
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    input [31:0] in0;
    input [31:0] in1;
    input [31:0] in2;
    input [31:0] in3;
    output [31:0] out;

    // Wires for the the config cells
    wire [1:0] MuxAddr_sig;
    wire MuxAddr_config;
    wire [1:0] MuxData_sig;
    wire MuxData_config;
    wire WriteRq_sig;
    wire WriteRq_config;

    // Wires connecting the main module and submodules
    wire [31:0] mux_addr_out_sig;
    wire [31:0] mux_data_out_sig;

    // Declaring the config cells
    ConfigCell #(2) MuxAddr (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(MuxAddr_config),
        .select(MuxAddr_sig));
    ConfigCell #(2) MuxData (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(MuxAddr_config),
        .ConfigOut(MuxData_config),
        .select(MuxData_sig));
    ConfigCell #(1) WriteRq (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(MuxData_config),
        .ConfigOut(WriteRq_config),
        .select(WriteRq_sig));

    // Declaring the submodules
    memUnit_32b #(32) mem_unit(
        .addr(mux_addr_out_sig),
        .data_in(mux_data_out_sig),
        .data_out(out),
        .w_rq(WriteRq_sig));
    mux_4to1_32b #(32) mux_addr(
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .out(mux_addr_out_sig),
        .select(MuxAddr_sig));
    mux_4to1_32b #(32) mux_data(
        .in0(in0),
        .in1(in1),
        .in2(in2),
        .in3(in3),
        .out(mux_data_out_sig),
        .select(MuxData_sig));
    assign ConfigOut = WriteRq_config;
endmodule

