# TCL File Generated by Component Editor 16.0
# Wed Sep 13 13:28:37 BST 2017
# DO NOT MODIFY


# 
# DE5Top "DE5Top" v1.0
#  2017.09.13.13:28:37
# 
# 

# 
# request TCL package from ACDS 16.0
# 
package require -exact qsys 16.0


# 
# module DE5Top
# 
set_module_property DESCRIPTION ""
set_module_property NAME DE5Top
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME DE5Top
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL de5Top
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file de5Top.v VERILOG PATH ../rtl/de5Top.v TOP_LEVEL_FILE


# 
# parameters
# 


# 
# display items
# 


# 
# connection point clock
# 
add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true
set_interface_property clock EXPORT_OF ""
set_interface_property clock PORT_NAME_MAP ""
set_interface_property clock CMSIS_SVD_VARIABLES ""
set_interface_property clock SVD_ADDRESS_GROUP ""

add_interface_port clock CLK clk Input 1


# 
# connection point reset_sink
# 
add_interface reset_sink reset end
set_interface_property reset_sink associatedClock clock
set_interface_property reset_sink synchronousEdges DEASSERT
set_interface_property reset_sink ENABLED true
set_interface_property reset_sink EXPORT_OF ""
set_interface_property reset_sink PORT_NAME_MAP ""
set_interface_property reset_sink CMSIS_SVD_VARIABLES ""
set_interface_property reset_sink SVD_ADDRESS_GROUP ""

add_interface_port reset_sink RST_N reset_n Input 1


# 
# connection point jtag_master
# 
add_interface jtag_master avalon start
set_interface_property jtag_master addressUnits SYMBOLS
set_interface_property jtag_master associatedClock clock
set_interface_property jtag_master associatedReset reset_sink
set_interface_property jtag_master bitsPerSymbol 8
set_interface_property jtag_master burstOnBurstBoundariesOnly false
set_interface_property jtag_master burstcountUnits WORDS
set_interface_property jtag_master doStreamReads false
set_interface_property jtag_master doStreamWrites false
set_interface_property jtag_master holdTime 0
set_interface_property jtag_master linewrapBursts false
set_interface_property jtag_master maximumPendingReadTransactions 0
set_interface_property jtag_master maximumPendingWriteTransactions 0
set_interface_property jtag_master readLatency 0
set_interface_property jtag_master readWaitTime 1
set_interface_property jtag_master setupTime 0
set_interface_property jtag_master timingUnits Cycles
set_interface_property jtag_master writeWaitTime 0
set_interface_property jtag_master ENABLED true
set_interface_property jtag_master EXPORT_OF ""
set_interface_property jtag_master PORT_NAME_MAP ""
set_interface_property jtag_master CMSIS_SVD_VARIABLES ""
set_interface_property jtag_master SVD_ADDRESS_GROUP ""

add_interface_port jtag_master jtagIfc_uart_address address Output 3
add_interface_port jtag_master jtagIfc_uart_writedata writedata Output 32
add_interface_port jtag_master jtagIfc_uart_write write Output 1
add_interface_port jtag_master jtagIfc_uart_read read Output 1
add_interface_port jtag_master jtagIfc_uart_uart_waitrequest waitrequest Input 1
add_interface_port jtag_master jtagIfc_uart_uart_readdata readdata Input 32


# 
# connection point dram_1
# 
add_interface dram_1 avalon start
set_interface_property dram_1 addressUnits SYMBOLS
set_interface_property dram_1 associatedClock clock
set_interface_property dram_1 associatedReset reset_sink
set_interface_property dram_1 bitsPerSymbol 8
set_interface_property dram_1 burstOnBurstBoundariesOnly false
set_interface_property dram_1 burstcountUnits WORDS
set_interface_property dram_1 doStreamReads false
set_interface_property dram_1 doStreamWrites false
set_interface_property dram_1 holdTime 0
set_interface_property dram_1 linewrapBursts false
set_interface_property dram_1 maximumPendingReadTransactions 0
set_interface_property dram_1 maximumPendingWriteTransactions 0
set_interface_property dram_1 readLatency 0
set_interface_property dram_1 readWaitTime 1
set_interface_property dram_1 setupTime 0
set_interface_property dram_1 timingUnits Cycles
set_interface_property dram_1 writeWaitTime 0
set_interface_property dram_1 ENABLED true
set_interface_property dram_1 EXPORT_OF ""
set_interface_property dram_1 PORT_NAME_MAP ""
set_interface_property dram_1 CMSIS_SVD_VARIABLES ""
set_interface_property dram_1 SVD_ADDRESS_GROUP ""

add_interface_port dram_1 dramIfcs_0_m_address address Output 31
add_interface_port dram_1 dramIfcs_0_m_burstcount burstcount Output 1
add_interface_port dram_1 dramIfcs_0_m_read read Output 1
add_interface_port dram_1 dramIfcs_0_m_readdata readdata Input 256
add_interface_port dram_1 dramIfcs_0_m_readdatavalid readdatavalid Input 1
add_interface_port dram_1 dramIfcs_0_m_waitrequest waitrequest Input 1
add_interface_port dram_1 dramIfcs_0_m_write write Output 1
add_interface_port dram_1 dramIfcs_0_m_writedata writedata Output 256


# 
# connection point dram_2
# 
add_interface dram_2 avalon start
set_interface_property dram_2 addressUnits SYMBOLS
set_interface_property dram_2 associatedClock clock
set_interface_property dram_2 associatedReset reset_sink
set_interface_property dram_2 bitsPerSymbol 8
set_interface_property dram_2 burstOnBurstBoundariesOnly false
set_interface_property dram_2 burstcountUnits WORDS
set_interface_property dram_2 doStreamReads false
set_interface_property dram_2 doStreamWrites false
set_interface_property dram_2 holdTime 0
set_interface_property dram_2 linewrapBursts false
set_interface_property dram_2 maximumPendingReadTransactions 0
set_interface_property dram_2 maximumPendingWriteTransactions 0
set_interface_property dram_2 readLatency 0
set_interface_property dram_2 readWaitTime 1
set_interface_property dram_2 setupTime 0
set_interface_property dram_2 timingUnits Cycles
set_interface_property dram_2 writeWaitTime 0
set_interface_property dram_2 ENABLED true
set_interface_property dram_2 EXPORT_OF ""
set_interface_property dram_2 PORT_NAME_MAP ""
set_interface_property dram_2 CMSIS_SVD_VARIABLES ""
set_interface_property dram_2 SVD_ADDRESS_GROUP ""

add_interface_port dram_2 dramIfcs_1_m_readdata readdata Input 256
add_interface_port dram_2 dramIfcs_1_m_readdatavalid readdatavalid Input 1
add_interface_port dram_2 dramIfcs_1_m_waitrequest waitrequest Input 1
add_interface_port dram_2 dramIfcs_1_m_writedata writedata Output 256
add_interface_port dram_2 dramIfcs_1_m_address address Output 31
add_interface_port dram_2 dramIfcs_1_m_read read Output 1
add_interface_port dram_2 dramIfcs_1_m_write write Output 1
add_interface_port dram_2 dramIfcs_1_m_burstcount burstcount Output 1


# 
# connection point northmac_source
# 
add_interface northmac_source avalon_streaming start
set_interface_property northmac_source associatedClock clock
set_interface_property northmac_source associatedReset reset_sink
set_interface_property northmac_source dataBitsPerSymbol 8
set_interface_property northmac_source errorDescriptor ""
set_interface_property northmac_source firstSymbolInHighOrderBits true
set_interface_property northmac_source maxChannel 0
set_interface_property northmac_source readyLatency 0
set_interface_property northmac_source ENABLED true
set_interface_property northmac_source EXPORT_OF ""
set_interface_property northmac_source PORT_NAME_MAP ""
set_interface_property northmac_source CMSIS_SVD_VARIABLES ""
set_interface_property northmac_source SVD_ADDRESS_GROUP ""

add_interface_port northmac_source northMac_source_endofpacket endofpacket Output 1
add_interface_port northmac_source northMac_source_data data Output 64
add_interface_port northmac_source northMac_source_source_ready ready Input 1
add_interface_port northmac_source northMac_source_startofpacket startofpacket Output 1
add_interface_port northmac_source northMac_source_valid valid Output 1
add_interface_port northmac_source northMac_source_empty empty Output 3
add_interface_port northmac_source northMac_source_error error Output 1


# 
# connection point southmac_source
# 
add_interface southmac_source avalon_streaming start
set_interface_property southmac_source associatedClock clock
set_interface_property southmac_source associatedReset reset_sink
set_interface_property southmac_source dataBitsPerSymbol 8
set_interface_property southmac_source errorDescriptor ""
set_interface_property southmac_source firstSymbolInHighOrderBits true
set_interface_property southmac_source maxChannel 0
set_interface_property southmac_source readyLatency 0
set_interface_property southmac_source ENABLED true
set_interface_property southmac_source EXPORT_OF ""
set_interface_property southmac_source PORT_NAME_MAP ""
set_interface_property southmac_source CMSIS_SVD_VARIABLES ""
set_interface_property southmac_source SVD_ADDRESS_GROUP ""

add_interface_port southmac_source southMac_source_endofpacket endofpacket Output 1
add_interface_port southmac_source southMac_source_data data Output 64
add_interface_port southmac_source southMac_source_source_ready ready Input 1
add_interface_port southmac_source southMac_source_startofpacket startofpacket Output 1
add_interface_port southmac_source southMac_source_valid valid Output 1
add_interface_port southmac_source southMac_source_empty empty Output 3
add_interface_port southmac_source southMac_source_error error Output 1


# 
# connection point eastmac_source
# 
add_interface eastmac_source avalon_streaming start
set_interface_property eastmac_source associatedClock clock
set_interface_property eastmac_source associatedReset reset_sink
set_interface_property eastmac_source dataBitsPerSymbol 8
set_interface_property eastmac_source errorDescriptor ""
set_interface_property eastmac_source firstSymbolInHighOrderBits true
set_interface_property eastmac_source maxChannel 0
set_interface_property eastmac_source readyLatency 0
set_interface_property eastmac_source ENABLED true
set_interface_property eastmac_source EXPORT_OF ""
set_interface_property eastmac_source PORT_NAME_MAP ""
set_interface_property eastmac_source CMSIS_SVD_VARIABLES ""
set_interface_property eastmac_source SVD_ADDRESS_GROUP ""

add_interface_port eastmac_source eastMac_source_endofpacket endofpacket Output 1
add_interface_port eastmac_source eastMac_source_data data Output 64
add_interface_port eastmac_source eastMac_source_source_ready ready Input 1
add_interface_port eastmac_source eastMac_source_startofpacket startofpacket Output 1
add_interface_port eastmac_source eastMac_source_valid valid Output 1
add_interface_port eastmac_source eastMac_source_empty empty Output 3
add_interface_port eastmac_source eastMac_source_error error Output 1


# 
# connection point westmac_source
# 
add_interface westmac_source avalon_streaming start
set_interface_property westmac_source associatedClock clock
set_interface_property westmac_source associatedReset reset_sink
set_interface_property westmac_source dataBitsPerSymbol 8
set_interface_property westmac_source errorDescriptor ""
set_interface_property westmac_source firstSymbolInHighOrderBits true
set_interface_property westmac_source maxChannel 0
set_interface_property westmac_source readyLatency 0
set_interface_property westmac_source ENABLED true
set_interface_property westmac_source EXPORT_OF ""
set_interface_property westmac_source PORT_NAME_MAP ""
set_interface_property westmac_source CMSIS_SVD_VARIABLES ""
set_interface_property westmac_source SVD_ADDRESS_GROUP ""

add_interface_port westmac_source westMac_source_endofpacket endofpacket Output 1
add_interface_port westmac_source westMac_source_data data Output 64
add_interface_port westmac_source westMac_source_source_ready ready Input 1
add_interface_port westmac_source westMac_source_startofpacket startofpacket Output 1
add_interface_port westmac_source westMac_source_valid valid Output 1
add_interface_port westmac_source westMac_source_empty empty Output 3
add_interface_port westmac_source westMac_source_error error Output 1


# 
# connection point northmac_sink
# 
add_interface northmac_sink avalon_streaming end
set_interface_property northmac_sink associatedClock clock
set_interface_property northmac_sink associatedReset reset_sink
set_interface_property northmac_sink dataBitsPerSymbol 8
set_interface_property northmac_sink errorDescriptor ""
set_interface_property northmac_sink firstSymbolInHighOrderBits true
set_interface_property northmac_sink maxChannel 0
set_interface_property northmac_sink readyLatency 0
set_interface_property northmac_sink ENABLED true
set_interface_property northmac_sink EXPORT_OF ""
set_interface_property northmac_sink PORT_NAME_MAP ""
set_interface_property northmac_sink CMSIS_SVD_VARIABLES ""
set_interface_property northmac_sink SVD_ADDRESS_GROUP ""

add_interface_port northmac_sink northMac_sink_ready ready Output 1
add_interface_port northmac_sink northMac_sink_sink_data data Input 64
add_interface_port northmac_sink northMac_sink_sink_endofpacket endofpacket Input 1
add_interface_port northmac_sink northMac_sink_sink_error error Input 6
add_interface_port northmac_sink northMac_sink_sink_startofpacket startofpacket Input 1
add_interface_port northmac_sink northMac_sink_sink_valid valid Input 1
add_interface_port northmac_sink northMac_sink_sink_empty empty Input 3


# 
# connection point eastmac_sink
# 
add_interface eastmac_sink avalon_streaming end
set_interface_property eastmac_sink associatedClock clock
set_interface_property eastmac_sink associatedReset reset_sink
set_interface_property eastmac_sink dataBitsPerSymbol 8
set_interface_property eastmac_sink errorDescriptor ""
set_interface_property eastmac_sink firstSymbolInHighOrderBits true
set_interface_property eastmac_sink maxChannel 0
set_interface_property eastmac_sink readyLatency 0
set_interface_property eastmac_sink ENABLED true
set_interface_property eastmac_sink EXPORT_OF ""
set_interface_property eastmac_sink PORT_NAME_MAP ""
set_interface_property eastmac_sink CMSIS_SVD_VARIABLES ""
set_interface_property eastmac_sink SVD_ADDRESS_GROUP ""

add_interface_port eastmac_sink eastMac_sink_ready ready Output 1
add_interface_port eastmac_sink eastMac_sink_sink_data data Input 64
add_interface_port eastmac_sink eastMac_sink_sink_endofpacket endofpacket Input 1
add_interface_port eastmac_sink eastMac_sink_sink_error error Input 6
add_interface_port eastmac_sink eastMac_sink_sink_startofpacket startofpacket Input 1
add_interface_port eastmac_sink eastMac_sink_sink_valid valid Input 1
add_interface_port eastmac_sink eastMac_sink_sink_empty empty Input 3


# 
# connection point southmac_sink
# 
add_interface southmac_sink avalon_streaming end
set_interface_property southmac_sink associatedClock clock
set_interface_property southmac_sink associatedReset reset_sink
set_interface_property southmac_sink dataBitsPerSymbol 8
set_interface_property southmac_sink errorDescriptor ""
set_interface_property southmac_sink firstSymbolInHighOrderBits true
set_interface_property southmac_sink maxChannel 0
set_interface_property southmac_sink readyLatency 0
set_interface_property southmac_sink ENABLED true
set_interface_property southmac_sink EXPORT_OF ""
set_interface_property southmac_sink PORT_NAME_MAP ""
set_interface_property southmac_sink CMSIS_SVD_VARIABLES ""
set_interface_property southmac_sink SVD_ADDRESS_GROUP ""

add_interface_port southmac_sink southMac_sink_ready ready Output 1
add_interface_port southmac_sink southMac_sink_sink_data data Input 64
add_interface_port southmac_sink southMac_sink_sink_endofpacket endofpacket Input 1
add_interface_port southmac_sink southMac_sink_sink_error error Input 6
add_interface_port southmac_sink southMac_sink_sink_startofpacket startofpacket Input 1
add_interface_port southmac_sink southMac_sink_sink_valid valid Input 1
add_interface_port southmac_sink southMac_sink_sink_empty empty Input 3


# 
# connection point westmac_sink
# 
add_interface westmac_sink avalon_streaming end
set_interface_property westmac_sink associatedClock clock
set_interface_property westmac_sink associatedReset reset_sink
set_interface_property westmac_sink dataBitsPerSymbol 8
set_interface_property westmac_sink errorDescriptor ""
set_interface_property westmac_sink firstSymbolInHighOrderBits true
set_interface_property westmac_sink maxChannel 0
set_interface_property westmac_sink readyLatency 0
set_interface_property westmac_sink ENABLED true
set_interface_property westmac_sink EXPORT_OF ""
set_interface_property westmac_sink PORT_NAME_MAP ""
set_interface_property westmac_sink CMSIS_SVD_VARIABLES ""
set_interface_property westmac_sink SVD_ADDRESS_GROUP ""

add_interface_port westmac_sink westMac_sink_ready ready Output 1
add_interface_port westmac_sink westMac_sink_sink_data data Input 64
add_interface_port westmac_sink westMac_sink_sink_endofpacket endofpacket Input 1
add_interface_port westmac_sink westMac_sink_sink_error error Input 6
add_interface_port westmac_sink westMac_sink_sink_startofpacket startofpacket Input 1
add_interface_port westmac_sink westMac_sink_sink_valid valid Input 1
add_interface_port westmac_sink westMac_sink_sink_empty empty Input 3


# 
# connection point board_id
# 
add_interface board_id conduit end
set_interface_property board_id associatedClock clock
set_interface_property board_id associatedReset ""
set_interface_property board_id ENABLED true
set_interface_property board_id EXPORT_OF ""
set_interface_property board_id PORT_NAME_MAP ""
set_interface_property board_id CMSIS_SVD_VARIABLES ""
set_interface_property board_id SVD_ADDRESS_GROUP ""

add_interface_port board_id setBoardId_id id Input 4

