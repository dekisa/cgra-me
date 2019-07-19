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

/**

  This is a simple example archs for mapping.
  All of the archs wont produce accurate RTL.
  Currently there are no configuration cells in this designs.

 **/

#include <CGRA/Exception.h>
#include <CGRA/user-inc/UserArchs.h>
#include <CGRA/user-inc/UserModules.h>

std::unique_ptr<CGRA> UserArchs::createNESWArch(const std::map<std::string, std::string> & args)
{
    int cols;
    int rows;
    int homo;
    try
    {
        cols = std::stoi(args.at("cols"));
        rows = std::stoi(args.at("rows"));
        homo = std::stoi(args.at("homogeneous_fu"));
    }
    catch(const std::out_of_range & e)
    {
        throw cgrame_error("C++ Architecture Argument Error");
    }

    std::unique_ptr<CGRA> result(new CGRA());

    result->ROWS = rows;
    result->COLS = cols;

    // create top IOs
    for (int i = 0; i < cols; i++)
    {
        result->addPort("ext_io_top_" + std::to_string(i), PORT_BIDIR);
        result->addSubModule(new IO("io_top_" + std::to_string(i), 32));
        result->addConnection(
            "io_top_" + std::to_string(i) + ".bidir",
            "this.ext_io_top_" + std::to_string(i)
            ); // to top-level external pin
    }

    // create right IOs
    for (int i = 0; i < rows; i++)
    {
        result->addPort("ext_io_right_" + std::to_string(i), PORT_BIDIR);
        result->addSubModule(new IO("io_right_" + std::to_string(i), 32));
        result->addConnection(
            "io_right_" + std::to_string(i) + ".bidir",
            "this.ext_io_right_" + std::to_string(i)
            ); // to top-level external pin
    }

    // create bottom IOs
    for (int i = 0; i < cols; i++)
    {
        result->addPort("ext_io_bottom_" + std::to_string(i), PORT_BIDIR);
        result->addSubModule(new IO("io_bottom_" + std::to_string(i), 32));
        result->addConnection(
            "io_bottom_" + std::to_string(i) + ".bidir",
            "this.ext_io_bottom_" + std::to_string(i)
            ); // to top-level external pin
    }

    // create left IOs
    for (int i = 0; i < rows; i++)
    {
        result->addPort("ext_io_left_" + std::to_string(i), PORT_BIDIR);
        result->addSubModule(new IO("io_left_" + std::to_string(i), 32));
        result->addConnection(
            "io_left_" + std::to_string(i) + ".bidir",
            "this.ext_io_left_" + std::to_string(i)
            ); // to top-level external pin
    }

    // create cols and rows of FUs
    if(homo)
    {
        for (int c = 0; c < cols; c++)
        {
            for (int r = 0; r < rows; r++)
            {
                result->addSubModule(new NESWArchFU("b_c" + std::to_string(c) + "_r" + std::to_string(r)));
            }
        }
    }
    else // hetero checker pattern
    {
        for (int c = 0; c < cols; c++)
        {
            for (int r = 0; r < rows; r++)
            {
                if((c + r) % 2) // create
                {
                    result->addSubModule(new NESWArchFU("b_c" + std::to_string(c) + "_r" + std::to_string(r), false));
                }
                else
                {
                    result->addSubModule(new NESWArchFU("b_c" + std::to_string(c) + "_r" + std::to_string(r), true));
                }
            }
        }
    }

    // create all connections

    // top row I/O connections
    for(int c = 0; c < cols; c++)
    {
        std::string io_name = "io_top_" + std::to_string(c);
        std::string block_name = "b_c" + std::to_string(c) + "_r" + std::to_string(rows-1);
        result->addConnection(io_name + ".out", block_name + ".in_n"); // to block
        result->addConnection(block_name + ".out_n", io_name + ".in"); // to io
    }

    // right col I/O connections
    for(int r = 0; r < rows; r++)
    {
        std::string io_name = "io_right_" + std::to_string(r);
        std::string block_name = "b_c" + std::to_string(cols-1) + "_r" + std::to_string(r);
        result->addConnection(io_name + ".out", block_name + ".in_e"); // to block
        result->addConnection(block_name + ".out_e", io_name + ".in"); // to io
    }

    // bottom row I/O connections
    for(int c = 0; c < cols; c++)
    {
        std::string io_name = "io_bottom_" + std::to_string(c);
        std::string block_name = "b_c" + std::to_string(c) + "_r" + std::to_string(0);
        result->addConnection(io_name + ".out", block_name + ".in_s"); // to block
        result->addConnection(block_name + ".out_s", io_name + ".in"); // to io
    }

    // left col I/O connections
    for(int r = 0; r < rows; r++)
    {
        std::string io_name = "io_left_" + std::to_string(r);
        std::string block_name = "b_c" + std::to_string(0) + "_r" + std::to_string(r);
        result->addConnection(io_name + ".out", block_name + ".in_w"); // to block
        result->addConnection(block_name + ".out_w", io_name + ".in"); // to io
    }

    // internal north/south connections
    for (int c = 0; c < cols; c++)
    {
        for (int r = 0; r < rows - 1; r++)
        {
            std::string block_name_c_r  = "b_c" + std::to_string(c) + "_r" + std::to_string(r);
            std::string block_name_c_r1 = "b_c" + std::to_string(c) + "_r" + std::to_string(r + 1);

            // north / south connections
            result->addConnection(block_name_c_r + ".out_n", block_name_c_r1 + ".in_s");
            result->addConnection(block_name_c_r1 + ".out_s", block_name_c_r + ".in_n");
        }
    }

    // internal west/east connections
    for (int c = 0; c < cols - 1; c++)
    {
        for (int r = 0; r < rows; r++)
        {
            std::string block_name_c_r  = "b_c" + std::to_string(c) + "_r" + std::to_string(r);
            std::string block_name_c1_r = "b_c" + std::to_string(c + 1) + "_r" + std::to_string(r);

            // east / west connections
            result->addConnection(block_name_c_r + ".out_e", block_name_c1_r + ".in_w");
            result->addConnection(block_name_c1_r + ".out_w", block_name_c_r + ".in_e");
        }
    }

    return result;
}

