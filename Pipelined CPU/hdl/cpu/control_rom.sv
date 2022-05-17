import rv32i_types::*;

module control_rom
(
    input rv32i_opcode opcode,
    input [2:0] funct3,
    input [6:0] funct7,
    input rv32i_reg rs1,
    input rv32i_reg rs2,
    input rv32i_word pc_pred,
    input logic [2:0] take_lgp,
    output rv32i_control_word ctrl
);

branch_funct3_t branch_funct3;
load_funct3_t load_funct3;
arith_funct3_t arith_funct3;

assign arith_funct3 = arith_funct3_t'(funct3);
assign branch_funct3 = branch_funct3_t'(funct3);
assign load_funct3 = load_funct3_t'(funct3);

function void loadRegfile(regfilemux::regfilemux_sel_t sel = regfilemux::alu_out);
    ctrl.load_regfile = 1'b1;
    ctrl.regfilemux_sel = sel;
endfunction

function void setALU(alumux::alumux1_sel_t sel1,
                               alumux::alumux2_sel_t sel2,
                               logic setop = 1'b0, alu_ops op = alu_add);
    /* Student code here */
    ctrl.alumux1_sel = sel1;
    ctrl.alumux2_sel = sel2;
    if (setop)
        ctrl.aluop = op; // else default value
endfunction

function automatic void setCMP(cmpmux::cmpmux_sel_t sel, branch_funct3_t op);
    ctrl.cmpmux_sel = sel;
    ctrl.cmpop = op;
endfunction



always_comb
begin
    /* Default assignments */

    /* inputs */
    ctrl.opcode = opcode;
    ctrl.funct3 = funct3;
    ctrl.funct7 = funct7;
    ctrl.rs1 = rs1;
    ctrl.rs2 = rs2;
    ctrl.pc_pred = pc_pred;
    ctrl.old_l = take_lgp[0];
    ctrl.old_g = take_lgp[1];
    ctrl.old_p = take_lgp[2];
    /* WB signals */
    ctrl.load_regfile = 1'b0;
    ctrl.regfilemux_sel = regfilemux::alu_out;

    /* MEM signals */
    ctrl.br_en = 1'b0;
    ctrl.mem_read = 1'b0;
    ctrl.mem_write = 1'b0;
    ctrl.br_jal = 1'b0;

    /* EX signals */
    ctrl.aluop = alu_ops'(funct3);
    ctrl.cmpop = branch_funct3;
	ctrl.cmpmux_sel = cmpmux::rs2_out;
    ctrl.alumux1_sel = alumux::rs1_out;
    ctrl.alumux2_sel = alumux::i_imm;
    ctrl.muldiv_en = 1'b0;

    /* Assign control signals based on opcode */
    case(opcode)
        op_auipc: begin
            setALU(alumux::pc_out, alumux::u_imm, 1'b1);
            loadRegfile();
        end
        op_lui: begin
            loadRegfile(regfilemux::u_imm);
        end
        op_jal: begin
            ctrl.br_en = 1'b1;
            ctrl.br_jal = 1'b1;
            setALU(alumux::pc_out, alumux::j_imm, 1'b1);
            loadRegfile(regfilemux::pc_plus4);
        end
        op_jalr: begin
            ctrl.br_en = 1'b1;
            ctrl.br_jal = 1'b1;
            setALU(alumux::rs1_out, alumux::i_imm, 1'b1);
            loadRegfile(regfilemux::pc_plus4);
        end
        op_br: begin
            ctrl.br_en = 1'b1;
            setALU(alumux::pc_out, alumux::b_imm, 1'b1);            
        end
        op_load: begin
            ctrl.mem_read = 1'b1;
            setALU(alumux::rs1_out, alumux::i_imm, 1'b1);

            unique case (load_funct3)
                lb: loadRegfile(regfilemux::lb);
                lbu: loadRegfile(regfilemux::lbu);
                lh: loadRegfile(regfilemux::lh);
                lhu: loadRegfile(regfilemux::lhu);
                lw: loadRegfile(regfilemux::lw);
                default: loadRegfile(); 
            endcase
        end
        op_store: begin
            ctrl.mem_write = 1'b1;
            setALU(alumux::rs1_out, alumux::s_imm, 1'b1);
        end
        op_imm: begin
            unique case (arith_funct3)
                slt: begin
                    loadRegfile(regfilemux::br_en);
                    setCMP(cmpmux::i_imm, blt);
                end
                sltu: begin
                    loadRegfile(regfilemux::br_en);
                    setCMP(cmpmux::i_imm, bltu);
                end 
                sr: begin
                    if (funct7[5] == 1'b1) begin
                        loadRegfile();
                        setALU(alumux::rs1_out, alumux::i_imm, 1'b1, alu_sra);
                    end
                    else begin
                        loadRegfile();
                        setALU(alumux::rs1_out, alumux::i_imm, 1'b1, alu_srl);                        
                    end
                end          
                default: begin
                    loadRegfile();
                end
            endcase
        end
        op_reg: begin
            if (funct7[0] == 1) begin
                ctrl.muldiv_en = 1'b1;
                loadRegfile();
                setALU(alumux::rs1_out, alumux::rs2_out, 1'b0);
            end
            else begin
                unique case (arith_funct3)
                    slt: begin
                        loadRegfile(regfilemux::br_en);
                        setCMP(cmpmux::rs2_out, blt);
                    end
                    sltu: begin
                        loadRegfile(regfilemux::br_en);
                        setCMP(cmpmux::rs2_out, bltu);
                    end 
                    sr: begin
                        if (funct7[5] == 1'b1) begin
                            loadRegfile();
                            setALU(alumux::rs1_out, alumux::rs2_out, 1'b1, alu_sra);
                        end
                        else begin
                            loadRegfile();
                            setALU(alumux::rs1_out, alumux::rs2_out, 1'b1, alu_srl);                        
                        end
                    end 
                    add: begin
                        if (funct7[5] == 1'b1) begin
                            loadRegfile();
                            setALU(alumux::rs1_out, alumux::rs2_out, 1'b1, alu_sub);
                        end
                        else begin
                            loadRegfile();
                            setALU(alumux::rs1_out, alumux::rs2_out, 1'b1, alu_add);                        
                        end
                    end         
                    default: begin
                        loadRegfile();
                        setALU(alumux::rs1_out, alumux::rs2_out);
                    end
                endcase
            end
        end

        default: begin
            ctrl = 0;   /* Unknown opcode, set control word to zero */
        end
    endcase
end
endmodule : control_rom
