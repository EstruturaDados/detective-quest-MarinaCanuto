#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define STR 64
#define HT  31  

// ------------------------- BST de Pistas -------------------------
typedef struct Nodo {
    char pista[STR];
    struct Nodo *esq, *dir;
} Nodo;

Nodo* bstCriar(const char* s){
    Nodo* n = (Nodo*)malloc(sizeof(Nodo));
    if(!n){ perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(n->pista, s, STR-1); n->pista[STR-1]='\0';
    n->esq = n->dir = NULL;
    return n;
}
Nodo* bstInserir(Nodo* r, const char* s){
    if(!r) return bstCriar(s);
    int cmp = strcmp(s, r->pista);
    if(cmp<0) r->esq = bstInserir(r->esq, s);
    else if(cmp>0) r->dir = bstInserir(r->dir, s);
    return r; /* se igual, ignora duplicata */
}
void bstInOrdem(Nodo* r){
    if(!r) return;
    bstInOrdem(r->esq);
    printf("- %s\n", r->pista);
    bstInOrdem(r->dir);
}
void bstLiberar(Nodo* r){
    if(!r) return;
    bstLiberar(r->esq); bstLiberar(r->dir);
    free(r);
}

// ---------------------- Hash pista -> suspeito -------------------
typedef struct Entry {
    char pista[STR];
    char suspeito[STR];
    struct Entry* next;
} Entry;

unsigned long hashStr(const char* s){
    unsigned long h = 5381;
    int c;
    while((c = (unsigned char)*s++)) h = ((h << 5) + h) + (unsigned)c;
    return h % HT;
}

typedef struct {
    Entry* tab[HT];
} HashPS;

void hpsInit(HashPS* h){ memset(h->tab, 0, sizeof(h->tab)); }

int hpsInsert(HashPS* h, const char* pista, const char* suspeito){
    unsigned long k = hashStr(pista);
    for(Entry* e=h->tab[k]; e; e=e->next){
        if(strcmp(e->pista, pista)==0){
            return 0; /* já existe */
        }
    }
    Entry* e = (Entry*)malloc(sizeof(Entry));
    if(!e){ perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(e->pista, pista, STR-1); e->pista[STR-1]='\0';
    strncpy(e->suspeito, suspeito, STR-1); e->suspeito[STR-1]='\0';
    e->next = h->tab[k];
    h->tab[k] = e;
    return 1;
}

const char* hpsGet(HashPS* h, const char* pista){
    unsigned long k = hashStr(pista);
    for(Entry* e=h->tab[k]; e; e=e->next){
        if(strcmp(e->pista, pista)==0) return e->suspeito;
    }
    return NULL;
}

void hpsList(HashPS* h){
    for(int i=0;i<HT;i++){
        for(Entry* e=h->tab[i]; e; e=e->next){
            printf("'%s' -> %s\n", e->pista, e->suspeito);
        }
    }
}

void hpsFree(HashPS* h){
    for(int i=0;i<HT;i++){
        Entry* e=h->tab[i];
        while(e){
            Entry* nx=e->next;
            free(e);
            e=nx;
        }
        h->tab[i]=NULL;
    }
}

// ----------------- Hash de contagem por suspeito -----------------
typedef struct SNode {
    char nome[STR];
    int  count;
    struct SNode* next;
} SNode;

typedef struct {
    SNode* tab[HT];
} HashSus;

void hsInit(HashSus* h){ memset(h->tab, 0, sizeof(h->tab)); }

int hsInc(HashSus* h, const char* suspeito){
    unsigned long k = hashStr(suspeito);
    for(SNode* n=h->tab[k]; n; n=n->next){
        if(strcmp(n->nome, suspeito)==0){
            n->count++;
            return n->count;
        }
    }
    SNode* n=(SNode*)malloc(sizeof(SNode));
    if(!n){ perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(n->nome, suspeito, STR-1); n->nome[STR-1]='\0';
    n->count=1; n->next=h->tab[k]; h->tab[k]=n;
    return 1;
}

void hsList(HashSus* h){
    for(int i=0;i<HT;i++){
        for(SNode* n=h->tab[i]; n; n=n->next){
            printf("%s: %d\n", n->nome, n->count);
        }
    }
}

const char* hsTop(HashSus* h, int* outCount){
    const char* top=NULL; int best=0;
    for(int i=0;i<HT;i++){
        for(SNode* n=h->tab[i]; n; n=n->next){
            if(n->count > best){ best=n->count; top=n->nome; }
        }
    }
    if(outCount) *outCount=best;
    return top;
}

void hsFree(HashSus* h){
    for(int i=0;i<HT;i++){
        SNode* n=h->tab[i];
        while(n){
            SNode* nx=n->next;
            free(n);
            n=nx;
        }
        h->tab[i]=NULL;
    }
}

// ------------------------- Utilidades I/O ------------------------
void lerLinha(char* buf, size_t n){
    if(!fgets(buf, (int)n, stdin)){ buf[0]='\0'; return; }
    buf[strcspn(buf, "\n")] = '\0';
}
void trim(char* s){
    char* p = s;
    while(isspace((unsigned char)*p)) p++;
    if(p!=s) memmove(s,p,strlen(p)+1);
    size_t L=strlen(s);
    while(L>0 && isspace((unsigned char)s[L-1])) s[--L]='\0';
}

// ------------------------------ Main ----------------------------
int main(void){
    Nodo* bstPistas = NULL;
    HashPS mapa; HashSus cont;
    hpsInit(&mapa); hsInit(&cont);

    puts("=== Detective Quest — Nível Mestre ===");
    puts("Armazene pistas (BST), relacione pista->suspeito (hash),");
    puts("e acompanhe contagens de citação por suspeito.\n");

    int op;
    char pista[STR], suspeito[STR];

    do{
        puts("\nMenu");
        puts("1 - Adicionar pista e suspeito");
        puts("2 - Listar pistas (ordem alfabética)");
        puts("3 - Consultar suspeito por pista");
        puts("4 - Listar suspeitos (contagem)");
        puts("5 - Mostrar suspeito mais citado");
        puts("0 - Sair");
        printf("Escolha: ");

        if(scanf("%d",&op)!=1){ puts("Entrada inválida. Encerrando."); break; }
        getchar(); // consome \n

        switch(op){
            case 1: {
                printf("Pista: ");
                lerLinha(pista, sizeof(pista)); trim(pista);
                if(pista[0]=='\0'){ puts("Pista vazia."); break; }

                printf("Suspeito: ");
                lerLinha(suspeito, sizeof(suspeito)); trim(suspeito);
                if(suspeito[0]=='\0'){ puts("Suspeito vazio."); break; }

                if(!hpsInsert(&mapa, pista, suspeito)){
                    printf("A pista '%s' já existe (associação mantida).\n", pista);
                    break;
                }
                bstPistas = bstInserir(bstPistas, pista);
                int c = hsInc(&cont, suspeito);
                printf("OK: '%s' -> %s (contagem de %s = %d)\n",
                       pista, suspeito, suspeito, c);
            } break;

            case 2: {
                if(!bstPistas) puts("Nenhuma pista registrada.");
                else{
                    puts("\nPistas em ordem alfabética:");
                    bstInOrdem(bstPistas);
                }
            } break;

            case 3: {
                printf("Pista a consultar: ");
                lerLinha(pista, sizeof(pista)); trim(pista);
                const char* s = hpsGet(&mapa, pista);
                if(s) printf("'%s' -> %s\n", pista, s);
                else  printf("Pista '%s' não encontrada.\n", pista);
            } break;

            case 4: {
                puts("\nSuspeitos e contagens:");
                hsList(&cont);
            } break;

            case 5: {
                int n=0; const char* top = hsTop(&cont, &n);
                if(top) printf("Mais citado: %s (%d)\n", top, n);
                else    puts("Nenhum suspeito registrado ainda.");
            } break;

            case 0: puts("Encerrando..."); break;
            default: puts("Opção inválida.");
        }

    }while(op!=0);

    bstLiberar(bstPistas);
    hpsFree(&mapa);
    hsFree(&cont);
    return 0;
}
