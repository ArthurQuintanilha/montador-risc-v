#include <stdio.h>
#include <string.h>
#include "montador.h"

void ExibirCodigoAsm(const char *caminho) {
    FILE *arquivo = fopen(caminho, "r");

    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    printf("\n=========== Conteudo de %s ===========\n\n", caminho);

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("\t%s", linha);
    }
    fclose(arquivo);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo.asm>\n", argv[0]);
        return 1;
    }

    const char *entrada = argv[1];
    const char *saida = "saida.dat";

    ExibirCodigoAsm(entrada);

    printf("\n======= Montagem traduzida para binario =======\n\n", saida);

    MontarParaArquivo(entrada, saida);
    printf("\nMontagem salva em '%s'\n", saida);

    printf("\n==============================================\n\n");

    return 0;
}
