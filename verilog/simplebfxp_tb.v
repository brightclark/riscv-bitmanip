module simplebfxp_tb;
	reg         clock;
	reg  [31:0] rs1;
	reg  [ 4:0] start, len, dest;
	wire [31:0] rd;

	simplebfxp uut (
		.clock (clock),
		.rs1   (rs1  ),
		.start (start),
		.len   (len  ),
		.dest  (dest ),
		.rd    (rd   )
	);

	reg [87:0] testdata_bfxp [0:999];

	initial begin
		$dumpfile("simplebfxp_tb.vcd");
		$dumpvars(0, simplebfxp_tb);

		$readmemh("testdata_bfxp.hex", testdata_bfxp);

		#5 clock = 0;
		repeat (20000) #5 clock = ~clock;

		$display("TIMEOUT");
		$stop;
	end

	integer icnt, ocnt;

	initial begin
		repeat (10) @(posedge clock);

		for (icnt = 0; icnt < 1000; icnt = icnt+1) begin
			rs1   <= testdata_bfxp[icnt][87:56];
			start <= testdata_bfxp[icnt][55:48];
			len   <= testdata_bfxp[icnt][47:40];
			dest  <= testdata_bfxp[icnt][39:32];
			@(posedge clock);
		end
	end

	initial begin
		repeat (11) @(posedge clock);

		for (ocnt = 0; ocnt < 1000; ocnt = ocnt+1) begin
			@(posedge clock);
			$display("%8d: rs1=0x%08h start=%2d len=%2d dest=%2d rd=0x%08h", ocnt,
					testdata_bfxp[ocnt][87:56], testdata_bfxp[ocnt][55:48],
					testdata_bfxp[ocnt][47:40], testdata_bfxp[ocnt][39:32], rd);
			if (rd !== testdata_bfxp[ocnt][31:0]) begin
				$display("Expected rd=0x%08h  =>  ERROR", testdata_bfxp[ocnt][31:0]);
				$stop;
			end
		end

		$display("PASSED");
		$finish;
	end
endmodule