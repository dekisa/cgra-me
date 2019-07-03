/*******************************************************************************
 * CGRA-ME Software End-User License Agreement
 *
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors S. Chin, K. Niu, N. Sakamoto, J. Zhao,
 * A. Rui, S. Yin, A. Mertens, J. Anderson, and the University of Toronto. Users
 * agree to not redistribute the software, in source or binary form, to other
 * persons or other institutions. Users may modify or use the source code for
 * other non-commercial, not-for-profit research endeavours, provided that all
 * copyright attribution on the source code is retained, and the original or
 * modified source code is not redistributed, in whole or in part, or included
 * in or with any commercial product, except by written agreement with the
 * authors, and full and complete attribution for use of the code is given in
 * any resulting publications.
 *
 * Only non-commercial, not-for-profit use of this software is permitted. No
 * part of this software may be incorporated into a commercial product without
 * the written consent of the authors. The software may not be used for the
 * design of a commercial electronic product without the written consent of the
 * authors. The use of this software to assist in the development of new
 * commercial CGRA architectures or commercial soft processor architectures is
 * also prohibited without the written consent of the authors.
 *
 * This software is provided "as is" with no warranties or guarantees of
 * support.
 *
 * This Agreement shall be governed by the laws of Province of Ontario, Canada.
 *
 * Please contact Prof. Anderson if you are interested in commercial use of the
 * CGRA-ME framework.
 ******************************************************************************/

#include <CGRA/user-inc/UserModules.h>

NESWArchFU::NESWArchFU(std::string name, bool full_fu)
    : Module(name)
{
    mt = MOD_COMPOSITE;

    // add input muxs
    addSubModule(new Multiplexer("mux_a", 5)); // plus 2 for feedback and constant
    addSubModule(new Multiplexer("mux_b", 5));
    addSubModule(new Multiplexer("mux_n", 4));
    addSubModule(new Multiplexer("mux_e", 4));
    addSubModule(new Multiplexer("mux_s", 4));
    addSubModule(new Multiplexer("mux_w", 4));
    // for passthrough
    //addSubModule(new Multiplexer("mux_bypass", num_inputs)); // for all eight inputs

    // add const unit
    addSubModule(new ConstUnit("const"));

    // add FU
    if(full_fu)
    {
        addSubModule(new FuncUnit(
              "func",
              {
              OPGRAPH_OP_ADD,
              OPGRAPH_OP_MUL,
              OPGRAPH_OP_SUB,
              OPGRAPH_OP_DIV,
              OPGRAPH_OP_AND,
              OPGRAPH_OP_OR,
              OPGRAPH_OP_XOR,
              OPGRAPH_OP_SHL,
              OPGRAPH_OP_SHRA,
              OPGRAPH_OP_SHRL}));
    }
    else
    {
        addSubModule(new FuncUnit(
              "func",
              {
              OPGRAPH_OP_ADD,
              OPGRAPH_OP_SUB,
              OPGRAPH_OP_AND,
              OPGRAPH_OP_OR,
              OPGRAPH_OP_XOR,
              OPGRAPH_OP_SHL,
              OPGRAPH_OP_SHRA,
              OPGRAPH_OP_SHRL}));
    }

    // add reg
    //addSubModule(new Register("register"));

    // add fu output mux
    //addSubModule(new Multiplexer("mux_out", 2)); // 2, 1 for bypass and one from the register output
    // config cells
    addConfig(new ConfigCell("MuxAConfig"), {"mux_a.select"});
    addConfig(new ConfigCell("MuxBConfig"), {"mux_b.select"});
    addConfig(new ConfigCell("MuxNConfig"), {"mux_n.select"});
    addConfig(new ConfigCell("MuxEConfig"), {"mux_e.select"});
    addConfig(new ConfigCell("MuxSConfig"), {"mux_s.select"});
    addConfig(new ConfigCell("MuxWConfig"), {"mux_w.select"});
    addConfig(new ConfigCell("FuncConfig"), {"func.select"});

    // input ports
    addPort("in_n", PORT_INPUT);
    addPort("in_e", PORT_INPUT);
    addPort("in_s", PORT_INPUT);
    addPort("in_w", PORT_INPUT);
    // output ports
    addPort("out_n", PORT_OUTPUT);
    addPort("out_e", PORT_OUTPUT);
    addPort("out_s", PORT_OUTPUT);
    addPort("out_w", PORT_OUTPUT);

    // to mux_a
    addConnection("this.in_n", "mux_a.in0");
    addConnection("this.in_e", "mux_a.in1");
    addConnection("this.in_s", "mux_a.in2");
    addConnection("this.in_w", "mux_a.in3");
    //addConnection("mux_out.out", "mux_a.in" + std::to_string(num_inputs));
    addConnection("const.out", "mux_a.in4");

    // to mux_b
    addConnection("this.in_n", "mux_b.in0");
    addConnection("this.in_e", "mux_b.in1");
    addConnection("this.in_s", "mux_b.in2");
    addConnection("this.in_w", "mux_b.in3");
    //addConnection("mux_out.out", "mux_a.in" + std::to_string(num_inputs));
    addConnection("const.out", "mux_b.in4");
    // to out mux n
    addConnection("this.in_e", "mux_n.in0");
    addConnection("this.in_s", "mux_n.in1");
    addConnection("this.in_w", "mux_n.in2");
    addConnection("func.out", "mux_n.in3");
    // to out mux e
    addConnection("this.in_n", "mux_e.in0");
    addConnection("this.in_s", "mux_e.in1");
    addConnection("this.in_w", "mux_e.in2");
    addConnection("func.out", "mux_e.in3");
    // to out mux s
    addConnection("this.in_n", "mux_s.in0");
    addConnection("this.in_e", "mux_s.in1");
    addConnection("this.in_w", "mux_s.in2");
    addConnection("func.out", "mux_s.in3");
    // to out mux w
    addConnection("this.in_n", "mux_w.in0");
    addConnection("this.in_e", "mux_w.in1");
    addConnection("this.in_s", "mux_w.in2");
    addConnection("func.out", "mux_w.in3");
    // to funcunit
    addConnection("mux_a.out", "func.in_a");
    addConnection("mux_b.out", "func.in_b");
    addConnection("mux_n.out", "this.out_n");
    addConnection("mux_e.out", "this.out_e");
    addConnection("mux_s.out", "this.out_s");
    addConnection("mux_w.out", "this.out_w");
    // to reg
    ///addConnection("func.out", "register.in");

    // to output mux
    //addConnection("register.out", "mux_out.in0");
    //addConnection("mux_bypass.out", "mux_out.in1");

    // to output
    //addConnection("mux_out.out", "this.out");

}

