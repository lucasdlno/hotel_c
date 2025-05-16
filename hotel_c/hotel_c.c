
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct quarto {
    int num;
    char categoria;
    char status; // 'L' - Livre, 'O' - Ocupado
} quarto;

typedef struct hospede {
    int quarto;
    char nome[80];
    int acompanhante;
    char categoria;
    int dias;
} hospede;

int verifica_quarto();
int verifica_hospede();
void limpar_buffer();
void aloca_quarto(quarto** q, int qq);
void aloca_hospede(hospede** h, int qh);
void grava_quarto(quarto* q);
void grava_hospede(hospede* h, const char* aux, int pos);
void cadastro_quarto(quarto* q, int qq);
int busca_quarto(quarto* q, int qq, char cat);
int busca_vago(hospede* h, int qh);
int busca_hospede(hospede* h, int n_quarto);
void atualiza_quarto(quarto* q, int n_quarto);
void cadastro_hospede(hospede* h, quarto* q, int qq);
void check_out(hospede* h, quarto* q);
void mostra_quarto(quarto* q);
void mostra_hospede(hospede* h);

int main() {
    quarto* q = NULL;
    hospede* h = NULL;
    int qq = 10;
    int qh = 5;

    aloca_quarto(&q, qq);
    aloca_hospede(&h, qh);
    cadastro_quarto(q, qq);

    int opcao;
    do {
        system("cls");  // Limpa a tela a cada repetição
        printf("\nMenu:\n"

            "1 - Cadastrar Hóspede\n"
            "2 - Mostrar Quartos\n"
            "3 - Mostrar Hóspedes\n"
            "4 - Check-Out\n"
            "0 - Sair\n"
            "Escolha: ");
        scanf("%d", &opcao);
        limpar_buffer();

        switch (opcao) {
        case 1: cadastro_hospede(h, q, qq); break;
        case 2: mostra_quarto(q); break;
        case 3: mostra_hospede(h); break;
        case 4: check_out(h, q); break;
        case 0: printf("Saindo...\n"); break;
        default: printf("Opção inválida!\n");
        }
    } while (opcao != 0);

    free(q);
    free(h);
    return 0;
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int verifica_quarto() {
    FILE* fq = fopen("quartos.bin", "rb");
    if (!fq) return 0;
    fseek(fq, 0, SEEK_END);
    int qtde = ftell(fq) / sizeof(quarto);
    fclose(fq);
    return qtde;
}

int verifica_hospede() {
    FILE* fh = fopen("hospedes.bin", "rb");
    if (!fh) return 0;
    fseek(fh, 0, SEEK_END);
    int qtde = ftell(fh) / sizeof(hospede);
    fclose(fh);
    return qtde;
}

void aloca_quarto(quarto** q, int qq) {
    *q = (quarto*)malloc(qq * sizeof(quarto));
    if (!*q) {
        printf("Erro ao alocar quartos!\n");
        exit(1);
    }
}

void aloca_hospede(hospede** h, int qh) {
    *h = (hospede*)malloc(qh * sizeof(hospede));
    if (!*h) {
        printf("Erro ao alocar hospedes!\n");
        exit(1);
    }
}

void grava_quarto(quarto* q) {
    FILE* fq = fopen("quartos.bin", "ab");
    if (fq) {
        fwrite(q, sizeof(quarto), 1, fq);
        fclose(fq);
    }
    else {
        printf("Erro ao gravar quarto\n");
    }
}

void grava_hospede(hospede* h, const char* aux, int pos) {
    FILE* fh = fopen("hospedes.bin", aux);
    if (!fh) {
        printf("Erro ao gravar hospede\n");
        return;
    }

    if (strcmp(aux, "rb+") == 0)
        fseek(fh, pos * sizeof(hospede), SEEK_SET);

    fwrite(h, sizeof(hospede), 1, fh);
    fclose(fh);
}

void cadastro_quarto(quarto* q, int qq) {
    for (int i = 0; i < qq; i++) {
        q[i].num = i + 1;
        q[i].status = 'L';
        q[i].categoria = (i < 5) ? 'S' : 'F';
        grava_quarto(&q[i]);
    }
}

int busca_quarto(quarto* q, int qq, char cat) {
    FILE* fq = fopen("quartos.bin", "rb+");
    if (!fq) return -1;

    int pos = -1;
    for (int i = 0; i < qq; i++) {
        fseek(fq, i * sizeof(quarto), SEEK_SET);
        fread(&q[i], sizeof(quarto), 1, fq);
        if (q[i].categoria == cat && q[i].status == 'L') {
            q[i].status = 'O';
            fseek(fq, i * sizeof(quarto), SEEK_SET);
            fwrite(&q[i], sizeof(quarto), 1, fq);
            pos = q[i].num;
            break;
        }
    }

    fclose(fq);
    return pos;
}

int busca_vago(hospede* h, int qh) {
    FILE* fh = fopen("hospedes.bin", "rb");
    if (!fh) return -1;

    for (int i = 0; i < qh; i++) {
        fseek(fh, i * sizeof(hospede), SEEK_SET);
        fread(&h[i], sizeof(hospede), 1, fh);
        if (h[i].quarto == -1) {
            fclose(fh);
            return i;
        }
    }

    fclose(fh);
    return -1;
}

int busca_hospede(hospede* h, int n_quarto) {
    FILE* fh = fopen("hospedes.bin", "rb+");
    if (!fh) return -1;

    int qh = verifica_hospede();
    for (int i = 0; i < qh; i++) {
        fseek(fh, i * sizeof(hospede), SEEK_SET);
        fread(&h[i], sizeof(hospede), 1, fh);
        if (h[i].quarto == n_quarto) {
            h[i].quarto = -1;
            fseek(fh, i * sizeof(hospede), SEEK_SET);
            fwrite(&h[i], sizeof(hospede), 1, fh);
            fclose(fh);
            return i;
        }
    }

    fclose(fh);
    return -1;
}

void atualiza_quarto(quarto* q, int n_quarto) {
    FILE* fq = fopen("quartos.bin", "rb+");
    if (!fq) return;

    int qq = verifica_quarto();
    for (int i = 0; i < qq; i++) {
        fseek(fq, i * sizeof(quarto), SEEK_SET);
        fread(&q[i], sizeof(quarto), 1, fq);
        if (q[i].num == n_quarto) {
            q[i].status = 'L';
            fseek(fq, i * sizeof(quarto), SEEK_SET);
            fwrite(&q[i], sizeof(quarto), 1, fq);
            break;
        }
    }

    fclose(fq);
}

void cadastro_hospede(hospede* h, quarto* q, int qq) {
    int qhosp = verifica_hospede();
    int pos = busca_vago(h, qhosp);

    printf("\nNome: ");
    fgets(h->nome, sizeof(h->nome), stdin);
    h->nome[strcspn(h->nome, "\n")] = '\0';

    do {
        printf("Acompanhantes (máx. 3): ");
        scanf("%d", &h->acompanhante);
        limpar_buffer();
    } while (h->acompanhante < 0 || h->acompanhante > 3);

    h->categoria = (h->acompanhante == 0) ? 'S' : 'F';

    printf("Dias: ");
    scanf("%d", &h->dias);
    limpar_buffer();

    h->quarto = busca_quarto(q, qq, h->categoria);

    if (h->quarto == -1) {
        printf("Não há quartos disponíveis na categoria.\n");
    }
    else {
        printf("Cadastro realizado - Quarto %d\n", h->quarto);
        if (pos == -1) grava_hospede(h, "ab", 0);
        else grava_hospede(h, "rb+", pos);
    }

    system("pause");
}

void check_out(hospede* h, quarto* q) {
    int n_quarto;
    printf("\nQuarto para check-out: ");
    scanf("%d", &n_quarto);
    limpar_buffer();

    int pos = busca_hospede(h, n_quarto);
    if (pos == -1) {
        printf("Quarto não encontrado!\n");
    }
    else {
        float valor = (h->categoria == 'S') ?
            h->dias * 85 : h->dias * (h->acompanhante + 1) * 65;
        printf("Nome: %s\nTotal: R$ %.2f\n", h->nome, valor);
        atualiza_quarto(q, n_quarto);
    }

    system("pause");
}

void mostra_quarto(quarto* q) {
    FILE* fq = fopen("quartos.bin", "rb");
    if (!fq) {
        printf("Arquivo de quartos não encontrado!\n");
        return;
    }

    int qq = verifica_quarto();
    for (int i = 0; i < qq; i++) {
        fseek(fq, i * sizeof(quarto), SEEK_SET);
        fread(&q[i], sizeof(quarto), 1, fq);
        printf("Quarto: %d | Categoria: %c | Status: %c\n", q[i].num, q[i].categoria, q[i].status);
    }

    fclose(fq);
    system("pause");
}

void mostra_hospede(hospede* h) {
    FILE* fh = fopen("hospedes.bin", "rb");
    if (!fh) {
        printf("Arquivo de hóspedes não encontrado!\n");
        return;
    }

    int qh = verifica_hospede();
    for (int i = 0; i < qh; i++) {
        fseek(fh, i * sizeof(hospede), SEEK_SET);
        fread(&h[i], sizeof(hospede), 1, fh);
        if (h7[i].quarto != -1) {
            printf("Quarto: %d | Nome: %s | Acompanhantes: %d | Dias: %d\n", h[i].quarto, h[i].nome, h[i].acompanhante, h[i].dias);
        }
    }

    fclose(fh);
    system("pause");
}

