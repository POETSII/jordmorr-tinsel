// Copyright (C) 1991-2016 Altera Corporation. All rights reserved.
// Your use of Altera Corporation's design tools, logic functions 
// and other software and tools, and its AMPP partner logic 
// functions, and any output files from any of the foregoing 
// (including device programming or simulation files), and any 
// associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License 
// Subscription Agreement, the Altera Quartus Prime License Agreement,
// the Altera MegaCore Function License Agreement, or other 
// applicable license agreement, including, without limitation, 
// that your use is for the sole purpose of programming logic 
// devices manufactured by Altera and sold by Altera or its 
// authorized distributors.  Please refer to the applicable 
// agreement for further details.

// Generated by Quartus.
// With edits from Matthew Naylor, November 2016.

// synopsys translate_off
`timescale 1 ps / 1 ps
// synopsys translate_on
module AlteraSignedMult (
  CLK,
  dataa,
  datab,
  res);

  parameter WIDTH = 1;

  input  CLK;
  input  [WIDTH-1:0] dataa;
  input  [WIDTH-1:0] datab;
  output [(2*WIDTH)-1:0] res;

  lpm_mult lpm_mult_component (
        //.clock (CLK),
        .clock (1'b0),
        .dataa (dataa),
        .datab (datab),
        .result (res),
        .aclr (1'b0),
        .clken (1'b1),
        .sclr (1'b0),
        .sum (1'b0));
  defparam
    lpm_mult_component.lpm_hint = "MAXIMIZE_SPEED=5",
    lpm_mult_component.lpm_pipeline = 0,
    lpm_mult_component.lpm_representation = "SIGNED",
    lpm_mult_component.lpm_type = "LPM_MULT",
    lpm_mult_component.lpm_widtha = WIDTH,
    lpm_mult_component.lpm_widthb = WIDTH,
    lpm_mult_component.lpm_widthp = 2*WIDTH;

endmodule
