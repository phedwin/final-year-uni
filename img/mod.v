

module basic_logic(
    in_1, in_2, in_3, out_1, out_2
);
    input in_1;
    input in_2;
    input in_3;

    output out_1;
    output out_2;
    
    assign out_2 = in_1 | in_2 | in_3;
    assign out_1 = in_1 & in_2 & in_3;
endmodule



module intermediate_write_signal(
    a, b, c, out_a, out_b
);
    input a;
    input b;
    input c;

    output out_a;
    output out_b;
    wire signal_wire;

    assign signal_wire =  a & b;

    assign out_a = c & signal_wire;
    assign out_b = c | signal_wire;
endmodule