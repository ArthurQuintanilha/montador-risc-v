#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "montador.h"

int RegistradorParaInt(char *reg) {
    if (reg[0] == 'x') return atoi(reg + 1);
    return -1;
}

void EscreverBinario(unsigned int instrucao, FILE *saida) {
    char buffer[33];
    for (int i = 31; i >= 0; i--) {
        buffer[31 - i] = ((instrucao >> i) & 1) + '0';
    }
    buffer[32] = '\0';

    fprintf(saida, "%s\n", buffer);   // Escreve no arquivo
    printf("%s\n", buffer);           // Imprime no terminal
}

// --- R-TYPE ---

void MontarAdd(char *rd, char *rs1, char *rs2, FILE *saida) {
    int opcode = 0b0110011, funct3 = 0b000, funct7 = 0b0000000;
    int rd_i = RegistradorParaInt(rd);
    int rs1_i = RegistradorParaInt(rs1);
    int rs2_i = RegistradorParaInt(rs2);
    unsigned int bin = (funct7 << 25) | (rs2_i << 20) | (rs1_i << 15) |
                       (funct3 << 12) | (rd_i << 7) | opcode;
    EscreverBinario(bin, saida);
}

void MontarOr(char *rd, char *rs1, char *rs2, FILE *saida) {
    int opcode = 0b0110011, funct3 = 0b110, funct7 = 0b0000000;
    int rd_i = RegistradorParaInt(rd);
    int rs1_i = RegistradorParaInt(rs1);
    int rs2_i = RegistradorParaInt(rs2);
    unsigned int bin = (funct7 << 25) | (rs2_i << 20) | (rs1_i << 15) |
                       (funct3 << 12) | (rd_i << 7) | opcode;
    EscreverBinario(bin, saida);
}

void MontarSll(char *rd, char *rs1, char *rs2, FILE *saida) {
    int opcode = 0b0110011, funct3 = 0b001, funct7 = 0b0000000;
    int rd_i = RegistradorParaInt(rd);
    int rs1_i = RegistradorParaInt(rs1);
    int rs2_i = RegistradorParaInt(rs2);
    unsigned int bin = (funct7 << 25) | (rs2_i << 20) | (rs1_i << 15) |
                       (funct3 << 12) | (rd_i << 7) | opcode;
    EscreverBinario(bin, saida);
}

// --- I-TYPE ---

void MontarAndi(char *rd, char *rs1, int imm, FILE *saida) {
    int opcode = 0b0010011, funct3 = 0b111;
    int rd_i = RegistradorParaInt(rd);
    int rs1_i = RegistradorParaInt(rs1);
    unsigned int bin = ((imm & 0xFFF) << 20) | (rs1_i << 15) |
                       (funct3 << 12) | (rd_i << 7) | opcode;
    EscreverBinario(bin, saida);
}

void MontarLh(char *rd, int imm, char *rs1, FILE *saida) {
    int opcode = 0b0000011, funct3 = 0b001;
    int rd_i = RegistradorParaInt(rd);
    int rs1_i = RegistradorParaInt(rs1);
    unsigned int bin = ((imm & 0xFFF) << 20) | (rs1_i << 15) |
                       (funct3 << 12) | (rd_i << 7) | opcode;
    EscreverBinario(bin, saida);
}

// --- S-TYPE ---

void MontarSh(char *rs2, int imm, char *rs1, FILE *saida) {
    int opcode = 0b0100011, funct3 = 0b001;
    int rs1_i = RegistradorParaInt(rs1);
    int rs2_i = RegistradorParaInt(rs2);
    int imm_low = imm & 0x1F;
    int imm_high = (imm >> 5) & 0x7F;
    unsigned int bin = (imm_high << 25) | (rs2_i << 20) | (rs1_i << 15) |
                       (funct3 << 12) | (imm_low << 7) | opcode;
    EscreverBinario(bin, saida);
}

// --- SB-TYPE ---

void MontarBne(char *rs1, char *rs2, int imm, FILE *saida) {
    int opcode = 0b1100011, funct3 = 0b001;
    int rs1_i = RegistradorParaInt(rs1);
    int rs2_i = RegistradorParaInt(rs2);
    int imm12 = (imm >> 12) & 1;
    int imm10_5 = (imm >> 5) & 0x3F;
    int imm4_1 = (imm >> 1) & 0xF;
    int imm11 = (imm >> 11) & 1;
    unsigned int bin = (imm12 << 31) | (imm10_5 << 25) | (rs2_i << 20) |
                       (rs1_i << 15) | (funct3 << 12) | (imm4_1 << 8) |
                       (imm11 << 7) | opcode;
    EscreverBinario(bin, saida);
}

// --- Processador de Linha ---

void ProcessarInstrucao(char *linha, FILE *saida) {
    char rd[10], rs1[10], rs2[10];
    int imm;

    if (sscanf(linha, "add %[^,], %[^,], %s", rd, rs1, rs2) == 3)
        MontarAdd(rd, rs1, rs2, saida);
    else if (sscanf(linha, "or %[^,], %[^,], %s", rd, rs1, rs2) == 3)
        MontarOr(rd, rs1, rs2, saida);
    else if (sscanf(linha, "sll %[^,], %[^,], %s", rd, rs1, rs2) == 3)
        MontarSll(rd, rs1, rs2, saida);
    else if (sscanf(linha, "andi %[^,], %[^,], %d", rd, rs1, &imm) == 3)
        MontarAndi(rd, rs1, imm, saida);
    else if (sscanf(linha, "lh %[^,], %d(%[^)])", rd, &imm, rs1) == 3)
        MontarLh(rd, imm, rs1, saida);
    else if (sscanf(linha, "sh %[^,], %d(%[^)])", rs2, &imm, rs1) == 3)
        MontarSh(rs2, imm, rs1, saida);
    else if (sscanf(linha, "bne %[^,], %[^,], %d", rs1, rs2, &imm) == 3)
        MontarBne(rs1, rs2, imm, saida);
    else {
        fprintf(saida, "00000000000000000000000000000000 --> instrucao invalida\n");
        printf("00000000000000000000000000000000 --> instrucao invalida\n");
    }
}

// --- Interfaces Públicas ---

void MontarParaTerminal(const char *arquivoEntrada) {
    FILE *arq = fopen(arquivoEntrada, "r");
    if (!arq) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    char linha[100];
    while (fgets(linha, sizeof(linha), arq)) {
        ProcessarInstrucao(linha, stdout);
    }

    fclose(arq);
}

void MontarParaArquivo(const char *arquivoEntrada, const char *arquivoSaida) {
    FILE *arq_in = fopen(arquivoEntrada, "r");
    FILE *arq_out = fopen(arquivoSaida, "w");
    if (!arq_in || !arq_out) {
        perror("Erro ao abrir arquivos");
        return;
    }

    char linha[100];
    while (fgets(linha, sizeof(linha), arq_in)) {
        ProcessarInstrucao(linha, arq_out);
    }

    fclose(arq_in);
    fclose(arq_out);
}
