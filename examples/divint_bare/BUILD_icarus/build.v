`define ICARUS 1

`timescale 1ns / 1ps

module top;

reg clk;
reg rst_n;

wire [7:0] __main_leds;

`ifdef VGA  
wire __main_video_clock;
wire __main_video_hs;
wire __main_video_vs;
wire [5:0] __main_video_r;
wire [5:0] __main_video_g;
wire [5:0] __main_video_b;
`endif

`ifdef UART
wire __main_uart_tx;
wire __main_uart_rx = 0;
`endif

`ifdef HDMI
wire [3:0] __main_out_gpdi_dp;
wire [3:0] __main_out_gpdi_dn;
`endif

`ifdef SDCARD
wire        __main_sd_clk;
wire        __main_sd_csn;
wire        __main_sd_mosi;
`endif

initial begin
  clk = 1'b0;
  rst_n = 1'b0;
  $display("icarus framework started");
  $dumpfile("icarus.fst");
`ifdef DUMP_TOP_ONLY
  $dumpvars(1,top); // dump only top (faster and smaller)
`else
  $dumpvars(0,top); // dump all (for full debugging)
`endif
`ifdef CLOCK_25MHz
  // generate a 25 MHz clock
  repeat(4) #20 clk = ~clk; 
  rst_n = 1'b1;
  forever #20 clk = ~clk;
`else
  // generate a 100 MHz clock
  repeat(4) #5 clk = ~clk; 
  rst_n = 1'b1;
  forever #5 clk = ~clk;   
`endif
end

reg ready = 0;
reg [3:0] RST_d;
reg [3:0] RST_q;

always @* begin
  RST_d = RST_q >> 1;
end

always @(posedge clk) begin
  if (ready) begin
    RST_q <= RST_d;
  end else begin
    ready <= 1;
    RST_q <= 4'b1111;
  end
end

wire run_main;
assign run_main = 1'b1;
wire done_main;

M_main __main(
  .clock(clk),
  .reset(RST_d[0]),
  .out_leds(__main_leds),
`ifdef VGA  
  .out_video_clock(__main_video_clock),
  .out_video_r(__main_video_r),
  .out_video_g(__main_video_g),
  .out_video_b(__main_video_b),
  .out_video_hs(__main_video_hs),
  .out_video_vs(__main_video_vs),  
`endif  
`ifdef SDCARD
  .out_sd_csn    (__main_sd_csn),
  .out_sd_clk    (__main_sd_clk),
  .out_sd_mosi   (__main_sd_mosi),
  .in_sd_miso    (1'b0),
`endif  
`ifdef UART
  .out_uart_tx(__main_uart_tx),
  .in_uart_rx(__main_uart_rx),
`endif  
`ifdef HDMI
  .out_gpdi_dp  (__main_out_gpdi_dp),
  .out_gpdi_dn  (__main_out_gpdi_dn),
`endif  
  .in_run(run_main),
  .out_done(done_main)
);

always @* begin
  if (done_main && ~RST_d[0]) $finish;
end

endmodule




module M_mul_cmp16_0__div0_mc0 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>0);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_1__div0_mc1 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>1);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_2__div0_mc2 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>2);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_3__div0_mc3 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>3);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_4__div0_mc4 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>4);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_5__div0_mc5 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>5);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_6__div0_mc6 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>6);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_7__div0_mc7 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>7);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_8__div0_mc8 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>8);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_9__div0_mc9 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>9);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_10__div0_mc10 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>10);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_11__div0_mc11 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>11);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_12__div0_mc12 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>12);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_13__div0_mc13 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>13);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule


module M_mul_cmp16_14__div0_mc14 (
in_num,
in_den,
out_beq,
out_clock,
clock
);
input  [15:0] in_num;
input  [15:0] in_den;
output  [0:0] out_beq;
output out_clock;
input clock;
assign out_clock = clock;
reg  [16:0] _t_nk;
reg  [0:0] _t_beq;

assign out_beq = _t_beq;

always @(posedge clock) begin
end




always @* begin
_t_nk = 0;
_t_beq = 0;
// _always_pre
// __block_1
_t_nk = (in_num>>14);
_t_beq = (_t_nk>in_den);
// __block_2
// _always_post
end
endmodule

module M_div16__div0 (
in_inum,
in_iden,
out_ret,
in_run,
out_done,
reset,
out_clock,
clock
);
input signed [15:0] in_inum;
input signed [15:0] in_iden;
output signed [15:0] out_ret;
input in_run;
output out_done;
input reset;
output out_clock;
input clock;
assign out_clock = clock;
wire  [0:0] _w_mc0_beq;
wire  [0:0] _w_mc1_beq;
wire  [0:0] _w_mc2_beq;
wire  [0:0] _w_mc3_beq;
wire  [0:0] _w_mc4_beq;
wire  [0:0] _w_mc5_beq;
wire  [0:0] _w_mc6_beq;
wire  [0:0] _w_mc7_beq;
wire  [0:0] _w_mc8_beq;
wire  [0:0] _w_mc9_beq;
wire  [0:0] _w_mc10_beq;
wire  [0:0] _w_mc11_beq;
wire  [0:0] _w_mc12_beq;
wire  [0:0] _w_mc13_beq;
wire  [0:0] _w_mc14_beq;
wire  [0:0] _c_num_neg;
assign _c_num_neg = 0;
wire  [0:0] _c_den_neg;
assign _c_den_neg = 0;
reg  [15:0] _t_concat;

reg  [15:0] _d_reminder;
reg  [15:0] _q_reminder;
reg  [15:0] _d_num;
reg  [15:0] _q_num;
reg  [15:0] _d_den;
reg  [15:0] _q_den;
reg signed [15:0] _d_ret;
reg signed [15:0] _q_ret;
reg  [2:0] _d_index,_q_index;
assign out_ret = _q_ret;
assign out_done = (_q_index == 6);

always @(posedge clock) begin
_q_reminder <= (reset || !in_run) ? 0 : _d_reminder;
_q_num <= (reset || !in_run) ? 0 : _d_num;
_q_den <= (reset || !in_run) ? 0 : _d_den;
_q_ret <= _d_ret;
_q_index <= (reset || !in_run) ? ( ~reset ? 0 : 6)  : _d_index;
end

M_mul_cmp16_0__div0_mc0 mc0 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc0_beq),
.clock(clock)
);
M_mul_cmp16_1__div0_mc1 mc1 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc1_beq),
.clock(clock)
);
M_mul_cmp16_2__div0_mc2 mc2 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc2_beq),
.clock(clock)
);
M_mul_cmp16_3__div0_mc3 mc3 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc3_beq),
.clock(clock)
);
M_mul_cmp16_4__div0_mc4 mc4 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc4_beq),
.clock(clock)
);
M_mul_cmp16_5__div0_mc5 mc5 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc5_beq),
.clock(clock)
);
M_mul_cmp16_6__div0_mc6 mc6 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc6_beq),
.clock(clock)
);
M_mul_cmp16_7__div0_mc7 mc7 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc7_beq),
.clock(clock)
);
M_mul_cmp16_8__div0_mc8 mc8 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc8_beq),
.clock(clock)
);
M_mul_cmp16_9__div0_mc9 mc9 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc9_beq),
.clock(clock)
);
M_mul_cmp16_10__div0_mc10 mc10 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc10_beq),
.clock(clock)
);
M_mul_cmp16_11__div0_mc11 mc11 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc11_beq),
.clock(clock)
);
M_mul_cmp16_12__div0_mc12 mc12 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc12_beq),
.clock(clock)
);
M_mul_cmp16_13__div0_mc13 mc13 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc13_beq),
.clock(clock)
);
M_mul_cmp16_14__div0_mc14 mc14 (
.in_num(_q_reminder),
.in_den(_q_den),
.out_beq(_w_mc14_beq),
.clock(clock)
);



always @* begin
_d_reminder = _q_reminder;
_d_num = _q_num;
_d_den = _q_den;
_d_ret = _q_ret;
_d_index = _q_index;
_t_concat = 0;
// _always_pre
(* full_case *)
case (_q_index)
0: begin
// _top
_d_den = in_iden;
_d_num = in_inum;
if (_d_den>_d_num) begin
// __block_1
// __block_3
_d_ret = 0;
_d_index = 3;
end else begin
// __block_2
_d_index = 1;
end
end
3: begin
// done
_d_index = 6;
end
1: begin
// __block_6
if (_q_den==_q_num) begin
// __block_7
// __block_9
_d_ret = 1;
_d_index = 3;
end else begin
// __block_8
_d_index = 2;
end
end
2: begin
// __block_12
if (_q_den==0) begin
// __block_13
// __block_15
if (_c_num_neg^_c_den_neg) begin
// __block_16
// __block_18
_d_ret = 16'b1111111111111111;
// __block_19
end else begin
// __block_17
// __block_20
_d_ret = 16'b0111111111111111;
// __block_21
end
// __block_22
_d_index = 3;
end else begin
// __block_14
_d_index = 4;
end
end
4: begin
// __block_25
_d_reminder = _q_num;
_d_ret = 0;
_d_index = 5;
end
5: begin
// __while__block_26
if (_q_reminder>=_q_den) begin
// __block_27
// __block_29
_t_concat = {!_w_mc14_beq&&_w_mc13_beq,!_w_mc13_beq&&_w_mc12_beq,!_w_mc12_beq&&_w_mc11_beq,!_w_mc11_beq&&_w_mc10_beq,!_w_mc10_beq&&_w_mc9_beq,!_w_mc9_beq&&_w_mc8_beq,!_w_mc8_beq&&_w_mc7_beq,!_w_mc7_beq&&_w_mc6_beq,!_w_mc6_beq&&_w_mc5_beq,!_w_mc5_beq&&_w_mc4_beq,!_w_mc4_beq&&_w_mc3_beq,!_w_mc3_beq&&_w_mc2_beq,!_w_mc2_beq&&_w_mc1_beq,!_w_mc1_beq&&_w_mc0_beq,1'b0};
  case (_t_concat)
  16'b1000000000000000: begin
// __block_31_case
// __block_32
_d_ret = _q_ret+(1<<14);
_d_reminder = _q_reminder-(_q_den<<14);
// __block_33
  end
  16'b0100000000000000: begin
// __block_34_case
// __block_35
_d_ret = _q_ret+(1<<13);
_d_reminder = _q_reminder-(_q_den<<13);
// __block_36
  end
  16'b0010000000000000: begin
// __block_37_case
// __block_38
_d_ret = _q_ret+(1<<12);
_d_reminder = _q_reminder-(_q_den<<12);
// __block_39
  end
  16'b0001000000000000: begin
// __block_40_case
// __block_41
_d_ret = _q_ret+(1<<11);
_d_reminder = _q_reminder-(_q_den<<11);
// __block_42
  end
  16'b0000100000000000: begin
// __block_43_case
// __block_44
_d_ret = _q_ret+(1<<10);
_d_reminder = _q_reminder-(_q_den<<10);
// __block_45
  end
  16'b0000010000000000: begin
// __block_46_case
// __block_47
_d_ret = _q_ret+(1<<9);
_d_reminder = _q_reminder-(_q_den<<9);
// __block_48
  end
  16'b0000001000000000: begin
// __block_49_case
// __block_50
_d_ret = _q_ret+(1<<8);
_d_reminder = _q_reminder-(_q_den<<8);
// __block_51
  end
  16'b0000000100000000: begin
// __block_52_case
// __block_53
_d_ret = _q_ret+(1<<7);
_d_reminder = _q_reminder-(_q_den<<7);
// __block_54
  end
  16'b0000000010000000: begin
// __block_55_case
// __block_56
_d_ret = _q_ret+(1<<6);
_d_reminder = _q_reminder-(_q_den<<6);
// __block_57
  end
  16'b0000000001000000: begin
// __block_58_case
// __block_59
_d_ret = _q_ret+(1<<5);
_d_reminder = _q_reminder-(_q_den<<5);
// __block_60
  end
  16'b0000000000100000: begin
// __block_61_case
// __block_62
_d_ret = _q_ret+(1<<4);
_d_reminder = _q_reminder-(_q_den<<4);
// __block_63
  end
  16'b0000000000010000: begin
// __block_64_case
// __block_65
_d_ret = _q_ret+(1<<3);
_d_reminder = _q_reminder-(_q_den<<3);
// __block_66
  end
  16'b0000000000001000: begin
// __block_67_case
// __block_68
_d_ret = _q_ret+(1<<2);
_d_reminder = _q_reminder-(_q_den<<2);
// __block_69
  end
  16'b0000000000000100: begin
// __block_70_case
// __block_71
_d_ret = _q_ret+(1<<1);
_d_reminder = _q_reminder-(_q_den<<1);
// __block_72
  end
  16'b0000000000000010: begin
// __block_73_case
// __block_74
_d_ret = _q_ret+(1<<0);
_d_reminder = _q_reminder-(_q_den<<0);
// __block_75
  end
  16'b0000000000000000: begin
// __block_76_case
// __block_77
_d_ret = _q_ret+(1<<0);
_d_reminder = _q_reminder-(_q_den<<0);
// __block_78
  end
  default: begin
// __block_79_case
// __block_80
// __block_81
  end
endcase
// __block_30
// __block_82
_d_index = 5;
end else begin
_d_index = 3;
end
end
6: begin // end of div16
end
default: begin 
_d_index = 6;
 end
endcase
// _always_post
end
endmodule

module M_main (
out_leds,
in_run,
out_done,
reset,
out_clock,
clock
);
output  [7:0] out_leds;
input in_run;
output out_done;
input reset;
output out_clock;
input clock;
assign out_clock = clock;
wire signed [15:0] _w_div0_ret;
wire _w_div0_done;
wire signed [15:0] _c_dividend;
assign _c_dividend = 20043;
wire signed [15:0] _c_divisor;
assign _c_divisor = 817;
reg signed [15:0] _t_result;

reg  [7:0] _d_leds;
reg  [7:0] _q_leds;
reg signed [15:0] _d_div0_inum,_q_div0_inum;
reg signed [15:0] _d_div0_iden,_q_div0_iden;
reg  [1:0] _d_index,_q_index;
reg  _div0_run=0;
assign out_leds = _q_leds;
assign out_done = (_q_index == 3);

always @(posedge clock) begin
_q_leds <= _d_leds;
_q_index <= (reset || !in_run) ? ( ~reset ? 0 : 0)  : _d_index;
_q_div0_inum <= _d_div0_inum;
_q_div0_iden <= _d_div0_iden;
end

M_div16__div0 div0 (
.in_inum(_q_div0_inum),
.in_iden(_q_div0_iden),
.out_ret(_w_div0_ret),
.out_done(_w_div0_done),
.in_run(_div0_run),
.reset(reset),
.clock(clock)
);



always @* begin
_d_leds = _q_leds;
_d_div0_inum = _q_div0_inum;
_d_div0_iden = _q_div0_iden;
_d_index = _q_index;
_div0_run = 1;
_t_result = 0;
// _always_pre
(* full_case *)
case (_q_index)
0: begin
// _top
_d_div0_inum = _c_dividend;
_d_div0_iden = _c_divisor;
_div0_run = 0;
_d_index = 1;
end
1: begin
// __block_1
if (_w_div0_done == 1) begin
_d_index = 2;
end else begin
_d_index = 1;
end
end
2: begin
// __block_2
_t_result = _w_div0_ret;
$display("%d / %d = %d",_c_dividend,_c_divisor,_t_result);
_d_leds = _t_result[0+:8];
_d_index = 3;
end
3: begin // end of main
end
default: begin 
_d_index = 0;
 end
endcase
// _always_post
end
endmodule

