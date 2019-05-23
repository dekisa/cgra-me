#env for CGRA mapping
#export CGRA_ME_ROOTDIR=../../../
#export CGRA_ME_
export CGRA_MAPPER_ARGS="-v -a -o" #--gen-testbench" #--gen-verilog $PWD/verilog_output/ --gen-testbench"
#export CGRA_ARCH_ARGS="--xml $CGRA_ME_ROOTDIR/arch/simple/arch-homo-orth.xml"
#export CGRA_ARCH_ARGS="--xml $CGRA_ME_ROOTDIR/arch/simple/archfiles/adres-no-torroid.xml"
export CGRA_ARCH_ARGS="-c 2"
make run_mapper
