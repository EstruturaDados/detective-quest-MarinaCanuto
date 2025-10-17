#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Detective Quest — Nível Novato
 * ------------------------------------------------------------
 * Implementa o mapa da mansão usando uma árvore binária.
 * O jogador começa no "Hall de Entrada" e escolhe:
 *   [e] esquerda, [d] direita, [s] sair
 * O jogo termina ao chegar em um cômodo sem saídas (nó-folha).
 */

typedef struct Sala {
    char nome[64];
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// Cria uma sala dinamicamente
Sala* criarSala(const char* nome) {
    Sala* s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        perror("Falha ao alocar sala");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, sizeof(s->nome) - 1);
    s->nome[sizeof(s->nome) - 1] = '\0';
    s->esq = s->dir = NULL;
    return s;
}

// Libera toda a árvore da memória
void liberarArvore(Sala* raiz) {
    if (!raiz) return;
    liberarArvore(raiz->esq);
    liberarArvore(raiz->dir);
    free(raiz);
}

// Lê escolha do jogador (e, d, s)
char lerEscolha() {
    char linha[32];
    printf("Escolha [e=esquerda, d=direita, s=sair]: ");
    if (!fgets(linha, sizeof(linha), stdin)) return 's';
    for (int i = 0; linha[i]; i++) {
        if (!isspace((unsigned char)linha[i])) {
            char c = (char)tolower((unsigned char)linha[i]);
            if (c=='e' || c=='d' || c=='s') return c;
            break;
        }
    }
    puts("Entrada inválida! Tente novamente.");
    return lerEscolha();
}

// Explora a árvore interativamente
void explorarSalas(Sala* raiz) {
    if (!raiz) {
        puts("Mapa vazio!");
        return;
    }

    Sala* atual = raiz;
    puts("\n=== Detective Quest — Mapa da Mansão ===\n");

    while (1) {
        printf("Você está em: %s\n", atual->nome);

        if (!atual->esq && !atual->dir) {
            puts("Fim do caminho! Você chegou a um cômodo sem saídas.");
            break;
        }

        printf("Caminhos disponíveis:");
        if (atual->esq) printf(" [e] %s", atual->esq->nome);
        if (atual->dir) printf(" [d] %s", atual->dir->nome);
        putchar('\n');

        char op = lerEscolha();
        if (op == 's') {
            puts("Saindo da exploração...");
            break;
        } else if (op == 'e') {
            if (atual->esq) atual = atual->esq;
            else puts("Não há caminho à esquerda.");
        } else if (op == 'd') {
            if (atual->dir) atual = atual->dir;
            else puts("Não há caminho à direita.");
        }
        puts("");
    }
}

int main(void) {
    // Monta o mapa (árvore binária)
    Sala* hall       = criarSala("Hall de Entrada");
    Sala* estar      = criarSala("Sala de Estar");
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* cozinha    = criarSala("Cozinha");
    Sala* jardim     = criarSala("Jardim");
    Sala* escritorio = criarSala("Escritório");
    Sala* galeria    = criarSala("Galeria");
    Sala* estufa     = criarSala("Estufa");
    Sala* adega      = criarSala("Adega");

    // Ligações
    hall->esq = estar;
    hall->dir = biblioteca;
    estar->esq = cozinha;
    estar->dir = jardim;
    jardim->esq = estufa;
    biblioteca->esq = escritorio;
    biblioteca->dir = galeria;
    galeria->dir = adega;

    // Exploração
    explorarSalas(hall);

    // Libera memória
    liberarArvore(hall);
    return 0;
}
