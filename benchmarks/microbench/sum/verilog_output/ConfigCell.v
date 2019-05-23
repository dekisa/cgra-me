module ConfigCell(ConfigIn, ConfigOut, Config_Clock, Config_Reset, select);
    parameter size = 1;
    input Config_Clock, Config_Reset, ConfigIn;
    output ConfigOut;
    output [size-1:0] select;
    reg [size-1:0] temp;

    always @(posedge Config_Clock, posedge Config_Reset)
        if (Config_Reset)
            temp = 0;
        else
            begin
                temp = temp >> 1;
                temp[size-1] = ConfigIn;
            end
    assign select = temp;
    assign ConfigOut = temp[0];
endmodule
