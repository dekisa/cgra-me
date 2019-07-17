module CGRA_configurator(
    input      clock,
    input      enable,
    input      sync_reset,

    output reg bitstream,
    output reg done
);

    localparam TOTAL_NUM_BITS = 465;
	reg [0:TOTAL_NUM_BITS-1] storage = {
		1'bx, // io_top_2::OEConfig
		1'bx, // io_top_2::IEConfig
		1'bx, // io_top_1::OEConfig
		1'bx, // io_top_1::IEConfig
		1'bx, // io_top_0::OEConfig
		1'bx, // io_top_0::IEConfig
		1'bx, // io_right_2::OEConfig
		1'b0, // io_right_2::IEConfig
		1'b1, // io_right_1::OEConfig
		1'b0, // io_right_1::IEConfig
		1'bx, // io_right_0::OEConfig
		1'bx, // io_right_0::IEConfig
		1'bx, // io_left_2::OEConfig
		1'bx, // io_left_2::IEConfig
		1'bx, // io_left_1::OEConfig
		1'bx, // io_left_1::IEConfig
		1'bx, // io_left_0::OEConfig
		1'bx, // io_left_0::IEConfig
		1'bx, // io_bottom_2::OEConfig
		1'b0, // io_bottom_2::IEConfig
		1'b1, // io_bottom_1::OEConfig
		1'bx, // io_bottom_1::IEConfig
		1'bx, // io_bottom_0::OEConfig
		1'bx, // io_bottom_0::IEConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c2_r2::ConstVal
		1'b1,1'b1, // b_c2_r2::MuxWConfig
		1'bx,1'bx, // b_c2_r2::MuxSConfig
		1'b1,1'b1, // b_c2_r2::MuxNConfig
		1'bx,1'bx, // b_c2_r2::MuxEConfig
		1'b1,1'b0, // b_c2_r2::MuxBConfig
		1'b0,1'b1,1'b0, // b_c2_r2::MuxAConfig
		1'b0,1'b0,1'b0,1'b0, // b_c2_r2::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c2_r1::ConstVal
		1'bx,1'bx, // b_c2_r1::MuxWConfig
		1'bx,1'bx, // b_c2_r1::MuxSConfig
		1'bx,1'bx, // b_c2_r1::MuxNConfig
		1'b1,1'b1, // b_c2_r1::MuxEConfig
		1'b0,1'b1, // b_c2_r1::MuxBConfig
		1'b1,1'b0,1'b0, // b_c2_r1::MuxAConfig
		1'b0,1'b0,1'b0,1'b0, // b_c2_r1::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c2_r0::ConstVal
		1'bx,1'bx, // b_c2_r0::MuxWConfig
		1'bx,1'bx, // b_c2_r0::MuxSConfig
		1'bx,1'bx, // b_c2_r0::MuxNConfig
		1'bx,1'bx, // b_c2_r0::MuxEConfig
		1'bx,1'bx, // b_c2_r0::MuxBConfig
		1'bx,1'bx,1'bx, // b_c2_r0::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r0::FuncConfig
		1'b1,1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0, // b_c1_r2::ConstVal
		1'bx,1'bx, // b_c1_r2::MuxWConfig
		1'b1,1'b1, // b_c1_r2::MuxSConfig
		1'bx,1'bx, // b_c1_r2::MuxNConfig
		1'bx,1'bx, // b_c1_r2::MuxEConfig
		1'b1,1'b0, // b_c1_r2::MuxBConfig
		1'b0,1'b0,1'b1, // b_c1_r2::MuxAConfig
		1'b0,1'b1,1'b0,1'b0, // b_c1_r2::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c1_r1::ConstVal
		1'bx,1'bx, // b_c1_r1::MuxWConfig
		1'bx,1'bx, // b_c1_r1::MuxSConfig
		1'bx,1'bx, // b_c1_r1::MuxNConfig
		1'bx,1'bx, // b_c1_r1::MuxEConfig
		1'bx,1'bx, // b_c1_r1::MuxBConfig
		1'bx,1'bx,1'bx, // b_c1_r1::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r1::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c1_r0::ConstVal
		1'bx,1'bx, // b_c1_r0::MuxWConfig
		1'bx,1'bx, // b_c1_r0::MuxSConfig
		1'bx,1'bx, // b_c1_r0::MuxNConfig
		1'bx,1'bx, // b_c1_r0::MuxEConfig
		1'bx,1'bx, // b_c1_r0::MuxBConfig
		1'bx,1'bx,1'bx, // b_c1_r0::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r0::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c0_r2::ConstVal
		1'bx,1'bx, // b_c0_r2::MuxWConfig
		1'bx,1'bx, // b_c0_r2::MuxSConfig
		1'bx,1'bx, // b_c0_r2::MuxNConfig
		1'bx,1'bx, // b_c0_r2::MuxEConfig
		1'bx,1'bx, // b_c0_r2::MuxBConfig
		1'bx,1'bx,1'bx, // b_c0_r2::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r2::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c0_r1::ConstVal
		1'bx,1'bx, // b_c0_r1::MuxWConfig
		1'bx,1'bx, // b_c0_r1::MuxSConfig
		1'bx,1'bx, // b_c0_r1::MuxNConfig
		1'bx,1'bx, // b_c0_r1::MuxEConfig
		1'bx,1'bx, // b_c0_r1::MuxBConfig
		1'bx,1'bx,1'bx, // b_c0_r1::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r1::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c0_r0::ConstVal
		1'bx,1'bx, // b_c0_r0::MuxWConfig
		1'bx,1'bx, // b_c0_r0::MuxSConfig
		1'bx,1'bx, // b_c0_r0::MuxNConfig
		1'bx,1'bx, // b_c0_r0::MuxEConfig
		1'bx,1'bx, // b_c0_r0::MuxBConfig
		1'bx,1'bx,1'bx, // b_c0_r0::MuxAConfig
		1'bx,1'bx,1'bx,1'bx // b_c0_r0::FuncConfig
	};

	reg [31:0] next_pos;
	always @(posedge clock) begin
		if (sync_reset) begin
			next_pos <= 0;
			bitstream <= 1'bx;
			done <= 0;
		end else if (next_pos >= TOTAL_NUM_BITS) begin
			done <= 1;
			bitstream <= 1'bx;
		end else if (enable) begin
			bitstream <= storage[next_pos];
			next_pos <= next_pos + 1;
		end
	end
endmodule
