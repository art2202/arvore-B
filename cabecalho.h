//
// Created by art on 10/11/2019.
//
#define M 2

typedef struct cliente Cliente;
struct cliente
{
    int cod;
    int idade;
    char nome[100];
    char tel[20];
};

typedef struct arvB ArvB;
struct arvB
{
    ArvB* pai;
    ArvB* filho[2*M + 1];
    ArvB*proxPag;
    Cliente*clientes[2*M];
    int numChaves;
    int chave[2*M];
    int pos;
};

ArvB* criaVazia();
int tamanhoCliente();
int tamanhoNoInterno();
int proxPosicaoFolha(FILE* arqMeta);
int verificaFolha(ArvB* arv);
int proxPosicaoInterna(FILE* arqMeta);
void escreveNoInterno(ArvB* arv, FILE* arqNoClt, int pos);
void escreveNovo(ArvB* arv, FILE*arqMeta, FILE*arqDados, FILE* arqNoInterno);
void atualizaNo(ArvB* arv, FILE* arqMeta, FILE*arqDados, FILE* arqNoInterno);
void imprimeClientes(FILE*arqClientes);
void atualizaPai(FILE* arqMeta, int pos);
void inserePaginaNaoCheia(ArvB* arv, Cliente* cliente, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno);
ArvB* insereFilho(ArvB* pai, ArvB* filho, Cliente* cliente, FILE* arqMeta, FILE*arqDados, FILE* arqNoInterno);
void posicionaFilho(ArvB* pai, ArvB* filho, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno);
ArvB* particionaNoInterno(ArvB* arv, int num, FILE*arqMeta, FILE*arqDados, FILE* arqNoInterno);
void particiona(ArvB* arv,  Cliente* cliente, FILE*arqMeta, FILE*arqDados, FILE* arqNoInterno);
void insere(ArvB* arv, Cliente* cliente, FILE* arqMeta, FILE* arqDados, FILE*arqNoInterno);
void concatenacao(ArvB* arv, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno, int posIrmao);
void redistribuicao(ArvB* arv, int pos, int posIrmao, int posPai, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno);
void excluiRegistro(ArvB* arv, int pos, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno);
void exclui(ArvB* arv, int num, FILE* arqMeta, FILE* arqDados , FILE* arqNoInterno);
void insercao(ArvB* arv, FILE* arqMeta, FILE*arqDados, FILE*arqNoInterno);
void exclusao(ArvB* arv, FILE* arqMeta, FILE* arqDados ,FILE* arqNoInterno);
int busca(ArvB* arv, int num);
void procura(ArvB* arv);
void buscaNome(ArvB* arv, char nome[100]);
void procuraNome(ArvB* arv);
void imprimeClienteInterno(FILE*arqDados);
void imprimeNoInternoClt(FILE* arqNoInterno);
void imprimeMetaDados(FILE* arqMeta);
void internoCliente(FILE* arqDados, FILE* arqNoInterno, FILE* arqMeta);
