module	decode(decode1,decode2,cs,p2,ale);
input	cs;
input	ale;
input	[3:0]p2;
output	[3:0]decode1;
output	[4:0]decode2;
reg		[3:0]decode1;
reg		[4:0]decode2;
reg		[3:0]data;


always@(posedge ale)
	begin
		data<=p2;
	end
				
always@(posedge ale)
	begin
		if(!cs)
		case(data)
		4'd9:	decode1[0]<=1;//playËø´æ1
		4'd10:	decode1[0]<=0;//playËø´æ0
		4'd11:	decode1[1]<=1;//recÓÐÐ
		4'd12:	decode1[1]<=0;//recÎÞÐ
		4'd5:	decode1[2]<=0;//9200cs0ÓÐÐ
		4'd13:	decode1[2]<=1;//9200cs0ÎÞÐ
		4'd6:	decode1[3]<=0;//9200cs1ÓÐÐ
		4'd14:	decode1[3]<=1;//9200cs1ÎÞÐ
		4'd15:	decode1<=4'b1100;
		endcase
		//end
	end
reg		cs1;
reg		cs2;
reg		cs3;
always@(posedge ale)
	begin
		if(!cs&&cs3)			//when cs negedge
		begin
		case(data)
		4'd0:	decode2[0]<=1;	//trst
		4'd1:	decode2[1]<=1;	//toneck
		4'd2:	decode2[2]<=1;	//comeck
		//4'd3:	decode2[3]<=1;	//toeo1
		//4'd4:	decode2[4]<=1;	//toe23
		4'd7:	decode2[3]<=1;	//relay_extck
		4'd8:	decode2[4]<=1;	//relay_trkck
		4'd15:	decode2<=5'b00000;
		endcase
		end
		else if(cs&&!cs3)		//when cs	posedge	
				decode2<=5'b00000;
		cs1<=cs;
		cs2<=cs1;
		cs3<=cs2;
	end
endmodule

