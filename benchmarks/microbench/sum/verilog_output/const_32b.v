module const_32b(Config_Clock, Config_Reset, ConfigIn, ConfigOut, out);
    parameter size = 32;
    // Specifying the ports
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    output [size-1:0] out;

    // Wires for the the config cells
    wire [31:0] ConstVal_sig;
    wire ConstVal_config;

    // Declaring the config cells
    ConfigCell #(32) ConstVal (
        .Config_Clock(Config_Clock),
        .Config_Reset(Config_Reset),
        .ConfigIn(ConfigIn),
        .ConfigOut(ConstVal_config),
        .select(ConstVal_sig));
    assign ConfigOut = ConstVal_config;

    assign out = ConstVal_sig;
endmodule

