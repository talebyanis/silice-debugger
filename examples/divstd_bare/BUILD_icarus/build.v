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
wire  [15:0] _w_diff;
wire  [15:0] _w_num;
wire  [15:0] _w_den;

reg  [15:0] _d_ac;
reg  [15:0] _q_ac;
reg  [4:0] _d_i;
reg  [4:0] _q_i;
reg signed [15:0] _d_ret;
reg signed [15:0] _q_ret;
reg  [1:0] _d_index,_q_index;
assign out_ret = _q_ret;
assign out_done = (_q_index == 3);

always @(posedge clock) begin
_q_ac <= _d_ac;
_q_i <= (reset || !in_run) ? 0 : _d_i;
_q_ret <= (reset || !in_run) ? 0 : _d_ret;
_q_index <= (reset || !in_run) ? ( ~reset ? 0 : 3)  : _d_index;
end



assign _w_num = in_inum;
assign _w_den = in_iden;
assign _w_diff = _q_ac-_w_den;

always @* begin
_d_ac = _q_ac;
_d_i = _q_i;
_d_ret = _q_ret;
_d_index = _q_index;
// _always_pre
(* full_case *)
case (_q_index)
0: begin
// _top
_d_ac = {{15{1'b0}},_w_num[15+:1]};
_d_ret = {_w_num[0+:15],1'b0};
_d_index = 1;
end
1: begin
// __while__block_1
if (_q_i!=16) begin
// __block_2
// __block_4
if (_w_diff[15+:1]==0) begin
// __block_5
// __block_7
_d_ac = {_w_diff[0+:15],_q_ret[15+:1]};
_d_ret = {_q_ret[0+:15],1'b1};
// __block_8
end else begin
// __block_6
// __block_9
_d_ac = {_q_ac[0+:15],_q_ret[15+:1]};
_d_ret = {_q_ret[0+:15],1'b0};
// __block_10
end
// __block_11
_d_i = _q_i+1;
// __block_12
_d_index = 1;
end else begin
_d_index = 2;
end
end
2: begin
// __block_3
_d_index = 3;
end
3: begin // end of div16
end
default: begin 
_d_index = 3;
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
wire signed [15:0] _c_num;
assign _c_num = 20043;
wire signed [15:0] _c_den;
assign _c_den = 41;
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
_d_div0_inum = _c_num;
_d_div0_iden = _c_den;
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
$display("%d / %d = %d",_c_num,_c_den,_t_result);
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

