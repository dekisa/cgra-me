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

#include <CGRA/CGRA.h>

#include <numeric>
#include <ostream>
#include <CGRA/user-inc/NESWbitstream.h>

void BitStream::append(const ConfigCell* ccell, const std::vector<BitSetting>& bits) {
    if (not setting_storage.emplace(ccell, bits).second) {
        throw cgrame_error("adding duplicate config setting!");
    }
    ccell_order.push_back(ccell);
}

// Regular bitstream printing
void BitStream::print()
{
    for (const auto& ccell : ccell_order) {
        std::cout << setting_storage.at(ccell);
    }
};

// Bitstream printing in debug mode, adds spaces between bits in different indexes
void BitStream::print_debug()
{
    for (const auto& ccell : ccell_order) {
        std::cout << ccell->getName() << ": " << setting_storage.at(ccell) << '\n';
    }
};

void bitstream_word::fill(unsigned int val, int size){
    if(val == INVALID_VAL)
        val = 0;
    for(int i = 0; i < size; i++){
        word = (word >> 1) | (val << 31);
        current_position-=1;
        val = val >> 1;
    }
};

std::ostream& operator<<(std::ostream& os, const bitstream_word& bw){
    unsigned int local_copy_word = bw.word;

    //make sure to fill with zeros if the word is not completed    
    if(bw.current_position > 0){
        for (int i = 0; i < bw.current_position; i++){
            local_copy_word = (local_copy_word >> 1);
        }
    }

    return os.write((char*)&local_copy_word, 4);
};


// Prints out bits in reverse order for a ModelSim testbench
void BitStream::print_testbench(std::ostream& os) const
{
    static const char* const total_num_bits_localparam =   "TOTAL_NUM_BITS";
    static const char* const clock_sig =                   "clock";
    static const char* const enable_sig =                  "enable";
    static const char* const sync_reset_sig =              "sync_reset";
    static const char* const bitstream_sig =               "bitstream";
    static const char* const done_sig =                    "done";
    static const char* const storage =                     "storage";
    static const char* const storage_pos =                 "next_pos";

    const int total_num_bits = std::accumulate(begin(setting_storage), end(setting_storage), 0, [&](auto&& accum, auto&& ccell_and_setting) {
        return accum + ccell_and_setting.first->getStorageSize();
    });

    os <<
        "module CGRA_configurator(\n"
        "    input      "<<clock_sig<<",\n"
        "    input      "<<enable_sig<<",\n"
        "    input      "<<sync_reset_sig<<",\n"
        "\n"
        "    output reg "<<bitstream_sig<<",\n"
        "    output reg "<<done_sig<<"\n"
        ");\n"
        "\n"
        "    localparam "<<total_num_bits_localparam<<" = " << total_num_bits << ";\n"
    ;

    os << "\treg [0:"<<total_num_bits_localparam<<"-1] "<<storage<<" = {\n";
    for (auto ccell_it = rbegin(ccell_order); ccell_it != rend(ccell_order); ++ccell_it) {
        const auto& ccell = *ccell_it;
        const auto& setting = setting_storage.at(ccell);
        os << "\t\t";
        for (auto bitsetting_it = rbegin(setting); bitsetting_it != rend(setting); ++bitsetting_it) {
            const auto& bitsetting = *bitsetting_it;
            os << for_verilog(bitsetting);
            if (std::next(ccell_it) != rend(ccell_order) || std::next(bitsetting_it) != rend(setting)) {
                os << ',';
            }
        }
        os << " // " << ccell->getAllConnectedPorts().at(0)->getModule().parent->getName() << "::" << ccell->getName() << '\n';
    }

    os <<
        "	};\n"
        "\n"
        "	reg [31:0] "<<storage_pos<<";\n"
        "	always @(posedge "<<clock_sig<<") begin\n"
        "		if (sync_reset) begin\n"
        "			"<<storage_pos<<" <= 0;\n"
        "			"<<bitstream_sig<<" <= 1'bx;\n"
        "			"<<done_sig<<" <= 0;\n"
        "		end else if ("<<storage_pos<<" >= "<<total_num_bits_localparam<<") begin\n"
        "			"<<done_sig<<" <= 1;\n"
        "			"<<bitstream_sig<<" <= 1'bx;\n"
        "		end else if ("<<enable_sig<<") begin\n"
        "			"<<bitstream_sig<<" <= "<<storage<<"["<<storage_pos<<"];\n"
        "			"<<storage_pos<<" <= "<<storage_pos<<" + 1;\n"
        "		end\n"
        "	end\n"
        "endmodule\n"
    ;
}
// bitstream printing to binary file, format defined in NEWSbitstream.h, 
// for achitecture NESWArchFU.cpp, NESWArch.cpp
void BitStream::print_bitstream(std::ostream& os) const{
    //write the start value
    bitstream_word word_start;
    word_start.fill(0xa3c5, 16);
    os << word_start;

    //calculate and write the size
    int rows = ((CGRA*)ccell_order.at(0)->getAllConnectedPorts().at(0)->getModule().parent->parent)->ROWS;
    int cols = ((CGRA*)ccell_order.at(0)->getAllConnectedPorts().at(0)->getModule().parent->parent)->COLS;
    int size = rows*cols;

    //info to write to binary file later
    unsigned int block_count = 0;    

    //iterate over blocks 
    auto ccell_it = ccell_order.begin();
    for (int i = 0; i < size; i++) {

        //check if block settings are valid
        bool block_valid = false;
        for (int i = 0; i < CELLS_PER_BLOCK; i++)
            if(is_valid(setting_storage.at(*(ccell_it + i))))
                block_valid = true;
        if(!block_valid){
            //move to next block
            ccell_it += CELLS_PER_BLOCK;
            continue;        
        } else
        block_count++;

        //get configuration cells
        const auto& fu_cell         = *(ccell_it + FU_CONFIG_IT);
        const auto& constant_cell   = *(ccell_it + CONST_IT);
        const auto& mux_fu1_cell    = *(ccell_it + MUX_FU1_IT);
        const auto& mux_fu2_cell    = *(ccell_it + MUX_FU2_IT);
        const auto& mux_n_cell      = *(ccell_it + MUX_N_IT);
        const auto& mux_e_cell      = *(ccell_it + MUX_E_IT);
        const auto& mux_s_cell      = *(ccell_it + MUX_S_IT);
        const auto& mux_w_cell      = *(ccell_it + MUX_W_IT);

        //emprty words
        bitstream_word word1;
        bitstream_word word2;
        bitstream_word word3;
        bitstream_word word4;

        //extract position info
        std::string block_name = fu_cell->getAllConnectedPorts().at(0)->getModule().parent->getName();
        std::string delimiter = "_";
        size_t delim_pos_1 = block_name.find(delimiter);
        size_t delim_pos_2 = block_name.find(delimiter, delim_pos_1+1);
        unsigned int xpos = std::stoi(block_name.substr(delim_pos_1+2, delim_pos_2 - delim_pos_1 - 2));
        unsigned int ypos = std::stoi(block_name.substr(delim_pos_2+2));

        // FU configuration
        unsigned int fu_config  = to_binary(setting_storage.at(fu_cell));
        // Constant value
        unsigned int constant   = to_binary(setting_storage.at(constant_cell));
        // mux info
        unsigned int mux_fu1    = to_binary(setting_storage.at(mux_fu1_cell));
        unsigned int mux_fu2    = to_binary(setting_storage.at(mux_fu2_cell));
        unsigned int mux_n      = to_binary(setting_storage.at(mux_n_cell));
        unsigned int mux_e      = to_binary(setting_storage.at(mux_e_cell));
        unsigned int mux_s      = to_binary(setting_storage.at(mux_s_cell));
        unsigned int mux_w      = to_binary(setting_storage.at(mux_w_cell));

        //calculate fr and fs
        GET_FR_FU(fu1, n, e, s, w);
        GET_FR_FU(fu2, n, e, s, w);
        GET_FS_FU(fu, n, e, s, w);
        GET_FS_NESW(n, fu1, fu2, e, s, w);
        GET_FS_NESW(e, fu1, fu2, n, s, w);
        GET_FS_NESW(s, fu1, fu2, n, e, w);
        GET_FS_NESW(w, fu1, fu2, n, e, s);
        GET_FR_NESW(n, e, s, w);
        GET_FR_NESW(e, n, s, w);
        GET_FR_NESW(s, n, e, w);
        GET_FR_NESW(w, n, e, s);

        //fill words        
        word1.fill(xpos, 6);
        word1.fill(ypos, 6);
        word1.fill(mux_fu1,3);
        word1.fill(mux_fu2,2);
        word1.fill(mux_n,2);
        word1.fill(mux_e,2);
        word1.fill(mux_s,2);
        word1.fill(mux_w,2);
        word1.fill(mux_fu1,3);
        word1.fill(fr_fu1,4);
        
        word2.fill(mux_fu2,2);
        word2.fill(fr_fu2, 4);
        word2.fill(fs_n, 5);
        word2.fill(fs_e, 5);
        word2.fill(fs_s, 5);
        word2.fill(fs_w, 5);
        word2.fill(fu_config, 5);
        word2.fill(0, 1);

        word3.fill(fs_fu, 4);
        word1.fill(mux_n,2);
        word3.fill(fr_n, 5);
        word1.fill(mux_e,2);
        word3.fill(fr_e, 5);
        word1.fill(mux_s,2);
        word3.fill(fr_s, 5);
        word1.fill(mux_w,2);
        word3.fill(fr_w, 5);

        word4.fill(constant,32);

        //write out
        os << word1 << word2 << word3 << word4;

        //move to next block
        ccell_it += CELLS_PER_BLOCK;
    }
    word_start.fill(block_count, 16);
    os.seekp(0);
    os << word_start;

}

