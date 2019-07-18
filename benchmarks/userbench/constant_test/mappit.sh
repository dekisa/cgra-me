#env for CGRA mapping
export CGRA_MAPPER_ARGS="-v -a -o" #--gen-verilog $PWD/verilog_output/ --gen-testbench"
export CGRA_ARCH_ARGS="-c 3 --arch-opts \"cols=3 rows=3\""
make run_mapper
