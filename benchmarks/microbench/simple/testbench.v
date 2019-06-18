module CGRA_configurator(
    input      clock,
    input      enable,
    input      sync_reset,

    output reg bitstream,
    output reg done
);

    localparam TOTAL_NUM_BITS = 848;
	reg [0:TOTAL_NUM_BITS-1] storage = {
		1'bx, // mem_3::WriteRq
		1'bx,1'bx, // mem_3::MuxData
		1'bx,1'bx, // mem_3::MuxAddr
		1'b0, // mem_2::WriteRq
		1'bx,1'bx, // mem_2::MuxData
		1'b0,1'b0, // mem_2::MuxAddr
		1'b0, // mem_1::WriteRq
		1'bx,1'bx, // mem_1::MuxData
		1'b0,1'b1, // mem_1::MuxAddr
		1'b1, // mem_0::WriteRq
		1'b0,1'b0, // mem_0::MuxData
		1'b1,1'b0, // mem_0::MuxAddr
		1'bx, // io_unit::OEConfig
		1'bx, // io_top_3::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx,1'bx, // io_top_2::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx,1'bx, // io_top_1::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_top_0::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_right_3::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx,1'bx, // io_right_2::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx,1'bx, // io_right_1::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_right_0::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_left_3::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx,1'bx, // io_left_2::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx,1'bx, // io_left_1::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_left_0::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_bottom_3::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx,1'bx, // io_bottom_2::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx,1'bx, // io_bottom_1::MuxConfig
		1'bx, // io_unit::OEConfig
		1'bx, // io_bottom_0::MuxConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c3_r3::ConstVal
		1'b0, // b_c3_r3::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r3::MuxBypassConfig
		1'b1,1'b0,1'b0,1'b1, // b_c3_r3::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r3::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r3::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c3_r2::ConstVal
		1'b1, // b_c3_r2::MuxOutConfig
		1'b0,1'b0,1'b0,1'b1, // b_c3_r2::MuxBypassConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r2::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r2::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r2::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c3_r1::ConstVal
		1'b1, // b_c3_r1::MuxOutConfig
		1'b0,1'b1,1'b1,1'b0, // b_c3_r1::MuxBypassConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r1::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r1::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r1::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c3_r0::ConstVal
		1'bx, // b_c3_r0::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r0::MuxBypassConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r0::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r0::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c3_r0::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c2_r3::ConstVal
		1'b0, // b_c2_r3::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r3::MuxBypassConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r3::MuxBConfig
		1'b1,1'b0,1'b0,1'b1, // b_c2_r3::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r3::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c2_r2::ConstVal
		1'b0, // b_c2_r2::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r2::MuxBypassConfig
		1'b1,1'b0,1'b0,1'b1, // b_c2_r2::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r2::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r2::FuncConfig
		1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1, // b_c2_r1::ConstVal
		1'b0, // b_c2_r1::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r1::MuxBypassConfig
		1'b1,1'b1,1'b1,1'b0, // b_c2_r1::MuxBConfig
		1'b0,1'b1,1'b0,1'b1, // b_c2_r1::MuxAConfig
		1'b1,1'b0,1'b0,1'b0, // b_c2_r1::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c2_r0::ConstVal
		1'b1, // b_c2_r0::MuxOutConfig
		1'b1,1'b0,1'b1,1'b0, // b_c2_r0::MuxBypassConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r0::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r0::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c2_r0::FuncConfig
		1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1, // b_c1_r3::ConstVal
		1'b0, // b_c1_r3::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r3::MuxBypassConfig
		1'b0,1'b1,1'b0,1'b1, // b_c1_r3::MuxBConfig
		1'b0,1'b1,1'b0,1'b0, // b_c1_r3::MuxAConfig
		1'b0,1'b0,1'b0,1'b0, // b_c1_r3::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c1_r2::ConstVal
		1'b0, // b_c1_r2::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r2::MuxBypassConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r2::MuxBConfig
		1'b0,1'b1,1'b1,1'b0, // b_c1_r2::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r2::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c1_r1::ConstVal
		1'b1, // b_c1_r1::MuxOutConfig
		1'b0,1'b0,1'b0,1'b1, // b_c1_r1::MuxBypassConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r1::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r1::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r1::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c1_r0::ConstVal
		1'b0, // b_c1_r0::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c1_r0::MuxBypassConfig
		1'b0,1'b0,1'b1,1'b0, // b_c1_r0::MuxBConfig
		1'b0,1'b1,1'b1,1'b0, // b_c1_r0::MuxAConfig
		1'b0,1'b0,1'b0,1'b0, // b_c1_r0::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c0_r3::ConstVal
		1'b0, // b_c0_r3::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r3::MuxBypassConfig
		1'b1,1'b0,1'b0,1'b1, // b_c0_r3::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r3::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r3::FuncConfig
		1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1, // b_c0_r2::ConstVal
		1'b0, // b_c0_r2::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r2::MuxBypassConfig
		1'b0,1'b0,1'b1,1'b0, // b_c0_r2::MuxBConfig
		1'b0,1'b1,1'b0,1'b1, // b_c0_r2::MuxAConfig
		1'b1,1'b0,1'b0,1'b0, // b_c0_r2::FuncConfig
		1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx,1'bx, // b_c0_r1::ConstVal
		1'b0, // b_c0_r1::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r1::MuxBypassConfig
		1'b1,1'b0,1'b1,1'b0, // b_c0_r1::MuxBConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r1::MuxAConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r1::FuncConfig
		1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1, // b_c0_r0::ConstVal
		1'b0, // b_c0_r0::MuxOutConfig
		1'bx,1'bx,1'bx,1'bx, // b_c0_r0::MuxBypassConfig
		1'b0,1'b1,1'b1,1'b0, // b_c0_r0::MuxBConfig
		1'b0,1'b1,1'b0,1'b1, // b_c0_r0::MuxAConfig
		1'b1,1'b0,1'b0,1'b0 // b_c0_r0::FuncConfig
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
