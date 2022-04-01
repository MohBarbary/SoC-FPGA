module counter(rst, clk, led);
input rst, clk;
output led;
reg led;
reg [31:0]cnt;

always@(posedge clk or negedge rst)
begin
    if(rst == 0)
    begin
        cnt <= 0;
        led <= 0;
    end
    else
    begin
        if(cnt < 500000)
            cnt <= cnt + 1;
        else
        begin
            cnt <= 0;
            led <= ~led;
        end
    end    
end

endmodule
    