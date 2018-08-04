module divf8(clk,clk1,clk2,fsr1,fsr2,fsr3,fsr4,fsr5,
			fsr6,fsr7,fsr8,rest,fsr,fsr9,frame);
output 		clk1,clk2,fsr1,fsr2,fsr3,fsr4,
			fsr5,fsr6,fsr7,fsr8,fsr,fsr9;
output		frame;
input  		clk,rest;
reg[10:0]	cnt0;
reg[2:0]	cnt1;
reg			frame;
reg 		clk1,clk2_reg,fsr1,fsr2,fsr3,
			fsr4,fsr5,fsr6,fsr7,fsr8,fsr,fsr9;

always@(negedge clk or	posedge rest)
  begin
if(rest==1)
	clk1=0;
else
	clk1=~clk1;				//2·ÖÆµ
  end

always@(negedge clk or posedge rest)
   begin
	if(rest==1)	
			begin
				clk2_reg=0;
				cnt0=0;
				cnt1=0;
				fsr=0;
				frame=1;
				fsr1=0;fsr2=0;fsr3=0;fsr4=0;
				fsr5=0;fsr6=0;fsr7=0;fsr8=0;
			end
	else
		begin
		cnt0=cnt0+1;
		cnt1=cnt1+1;
		if(cnt1==2)
			begin
			cnt1=0;
			clk2_reg=~clk2_reg;
			end		
		if(cnt0==15)			frame=0;
		  else if(cnt0==16)		begin
								fsr1=1;
								fsr=1;
								end
		  else if(cnt0==17)		frame=1;
		  else if(cnt0==32)		fsr1=0;
		  else if(cnt0==48)		fsr2=1;
		  else if(cnt0==64)		fsr2=0;
		  else if(cnt0==80)		fsr3=1;
		  else if(cnt0==96)		fsr3=0;
		  else if(cnt0==112)	fsr4=1;
		  else if(cnt0==128)	fsr4=0;
		  else if(cnt0==144)	begin
								fsr9=1;
								fsr5=1;
								end
		  else if(cnt0==160)	fsr5=0;
		  else if(cnt0==176)	fsr6=1;
		  else if(cnt0==192)	fsr6=0;
		  else if(cnt0==208)	fsr7=1;
		  else if(cnt0==224)	fsr7=0;
		  else if(cnt0==240)	fsr8=1;
		  else if(cnt0==256)	fsr8=0;
		  else if(cnt0==528)	fsr=0;
		  else if(cnt0==656)	fsr9=0;
		  else if(cnt0==1024)	begin
								cnt0=0;
								end
		end
	end	

assign	clk2=~clk2_reg;																											
endmodule

