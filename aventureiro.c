#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Detective Quest — Nível Aventureiro
 * ------------------------------------------------------------
 * - Armazena as pistas encontradas em uma Árvore de Busca Binária (BST)
 * - Permite inserir e exibir as pistas em ordem alfabética
 * - O jogador interage com o menu:
 *      1 - Inserir nova pista
 *      2 - Exibir pistas encontradas
 *      0 - Sair
 */

// ---------------- ESTRUTURA DA ÁRVORE ----------------

typedef struct Nodo {
    char pista[64];
    struct Nodo *esq, *dir;
} Nodo;

// Cria um novo nó dinamicamente
Nodo* criarNodo(const char* nome) {
    Nodo* novo = (Nodo*) malloc(sizeof(Nodo));
    if (!novo) {
        perror("Falha ao alocar nodo");
        exit(EXIT_FAILURE);
    }
    strncpy(novo->pista, nome, sizeof(novo->pista) - 1);
    novo->pista[sizeof(novo->pista) - 1] = '\0';
    novo->esq = novo->dir = NULL;
    return novo;
}

// Inserção na árvore de busca binária (BST)
Nodo* inserir(Nodo* raiz, const char* nome) {
    if (!raiz) return criarNodo(nome);

    int cmp = strcmp(nome, raiz->pista);
    if (cmp < 0) {
        raiz->esq = inserir(raiz->esq, nome);
    } else if (cmp > 0) {
        raiz->dir = inserir(raiz->dir, nome);
    } else {
        printf("A pista '%s' já está registrada.\n", nome);
    }
    return raiz;
}

// Percurso em ordem (alfabético)
void emOrdem(Nodo* raiz) {
    if (!raiz) return;
    emOrdem(raiz->esq);
    printf("🔎 %s\n", raiz->pista);
    emOrdem(raiz->dir);
}

// Libera memória
void liberar(Nodo* raiz) {
    if (!raiz) return;
    liberar(raiz->esq);
    liberar(raiz->dir);
    free(raiz);
}

// ---------------- MENU INTERATIVO ----------------

int main(void) {
    Nodo* pistas = NULL;
    int op;
    char nome[64];

    puts("=== Detective Quest — Nível Aventureiro ===");
    puts("Organize suas pistas em uma árvore de busca binária!");
    puts("");

    do {
        puts("\nMenu de Ações:");
        puts("1 - Inserir nova pista");
        puts("2 - Exibir pistas encontradas");
        puts("0 - Sair");
        printf("Escolha: ");

        if (scanf("%d", &op) != 1) {
            puts("Entrada inválida. Encerrando...");
            break;
        }
        getchar(); // limpa o \n do buffer

        switch (op) {
            case 1:
                printf("Digite o nome da pista: ");
                if (fgets(nome, sizeof(nome), stdin)) {
                    nome[strcspn(nome, "\n")] = '\0'; // remove \n
                    pistas = inserir(pistas, nome);
                    printf("Pista '%s' adicionada com sucesso!\n", nome);
                }
                break;

            case 2:
                if (!pistas) {
                    puts("Nenhuma pista registrada ainda!");
                } else {
                    puts("\nPistas encontradas (ordem alfabética):");
                    emOrdem(pistas);
                }
                break;

            case 0:
                puts("Encerrando investigação...");
                break;

            default:
                puts("Opção inválida! Tente novamente.");
        }
    } while (op != 0);

    liberar(pistas);
    return 0;
}
