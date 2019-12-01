//
// Created by art on 10/11/2019.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cabecalho.h"

int main() {

    FILE* arqMetaClt = fopen("metaClientes.dat", "a+b");
    FILE*  arqClientes = fopen("clientes.dat", "a+b");
    FILE* arqNoClt = fopen("niClientes.dat", "a+b");

    ArvB* arvCl;
    int sel = 0;

    arvCl = criaVazia();

    printf("1- Registro de novo cliente \n"
           "2- Excluir registro de cliente\n"
           "3- Buscar cliente cadastrado\n"
           "4- Busca por nome\n"
           "5- Exibir tabela \n"
           "6- Imprime todos os arquivos\n"
           "0- encerrar\n");


    scanf("%d", &sel);
    while(sel != 0) {

        if (sel == 1)
            insercao(arvCl, arqMetaClt, arqClientes, arqNoClt);
        else if (sel == 2)
            exclusao(arvCl, arqMetaClt, arqClientes, arqNoClt);
        else if (sel == 3)
            procura(arvCl);
        else if (sel == 4)
            procuraNome(arvCl);
        else if (sel == 5)
            imprimeClientes(arqClientes);
        else if (sel == 6)
            internoCliente(arqClientes, arqNoClt, arqMetaClt);

        printf("1- Registro de novo cliente \n"
               "2- Excluir registro de cliente\n"
               "3- Buscar cliente cadastrado\n"
               "4- Busca por nome\n"
               "5- Exibir tabela \n"
               "6- Imprime todos os arquivos\n"
               "0- encerrar\n");

        scanf("%d", &sel);

    }

    fclose(arqClientes);
    fclose(arqMetaClt);
    fclose(arqNoClt);
    return 0;
}

ArvB* criaVazia()
{
    ArvB* arv = (ArvB*) malloc(sizeof(ArvB));
    arv->numChaves = 0;
    arv->pai = NULL;
    arv->pos = 0;

    for(int i = 0; i  < 2*M + 1; i++)
        arv->filho[i] = NULL;
    for(int i = 0; i < 2*M; i++)
    {
        arv->chave[i] = 0;
        arv->clientes[i] = NULL;
    }

    return arv;
}

int tamanhoCliente()
{
    return( sizeof(int)+
            sizeof(char)*100 +
            sizeof(char)*20) * 4;
}

int tamanhoNoInterno()
{
    return sizeof(int)*12;
}

int proxPosicaoFolha(FILE* arqMeta)
{
    int pos;
    fseek(arqMeta, sizeof(int)*2, SEEK_SET);
    fread(&pos, sizeof(int), 1, arqMeta);
    int prox =  pos+ 1;
    fseek(arqMeta, sizeof(int)*2, SEEK_SET);
    fwrite(&prox, sizeof(int), 1, arqMeta);

    return pos;
}

int verificaFolha(ArvB* arv)
{
    for(int i = 0; i < 2*M+1; i++)
    {
        if(arv->filho[i] != NULL)
            return 0;
    }
    return 1;
}

int proxPosicaoInterna(FILE* arqMeta)
{
    int pos;
    fseek(arqMeta, sizeof(int)*3, SEEK_SET);
    fread(&pos, sizeof(int), 1, arqMeta);
    int prox = pos + 1;
    fseek(arqMeta, sizeof(int)*3, SEEK_SET);
    fwrite(&prox, sizeof(int), 1, arqMeta);

    return pos;
}

void escreveNoInterno(ArvB* arv, FILE* arqNoClt, int pos)
{

    int num  = 1;
    //  printf("\n\n\n POS: --------> %d\n", pos);
    fseek(arqNoClt, tamanhoNoInterno()*pos, SEEK_SET);

    fwrite(&arv->numChaves, sizeof(int), 1, arqNoClt);

    if(verificaFolha(arv) == 1 || (arv->filho[0] != NULL && verificaFolha(arv->filho[0]) == 1 && verificaFolha(arv) == 0) )// se 1 , aponta pra folha , 0 aponta pra nó interno
        fwrite(&num, sizeof(int), 1, arqNoClt);
    else
    {
        num = 0;
        fwrite(&num, sizeof(int), 1, arqNoClt);
    }

    if(arv->pai != NULL)
        fwrite(&arv->pai->pos, sizeof(int), 1, arqNoClt);// escreve a posição do pai
    else
    {
        num = -1;
        fwrite(&num, sizeof(int), 1, arqNoClt);
    }

    for(int i = 0; i < 2*M; i++)  // escre as chaves
        fwrite(&arv->chave[i], sizeof(int), 1, arqNoClt);

    for(int i = 0; i < 2*M+1; i++) // escreve a posição dos filhos
    {
        if(arv->filho[i] != NULL)
            fwrite(&arv->filho[i]->pos, sizeof(int), 1, arqNoClt);
        else
        {
            num = -1;
            fwrite(&num, sizeof(int), 1, arqNoClt);
        }
    }
}

void escreveCliente(ArvB* arv, FILE* arqClientes, int pos)
{
    fseek(arqClientes, tamanhoCliente()*pos, SEEK_SET);

    int num  = -1;
    char vazio[100] = "vazio";
    char telVazio[20] = "--";

    for(int i = 0; i < 2*M; i++)
    {

        if(arv->clientes[i] == NULL)
        {
            fwrite(vazio, sizeof(char), sizeof(vazio), arqClientes);
            fwrite(&num, sizeof(int), 1, arqClientes);
            fwrite(telVazio, sizeof(char), sizeof(telVazio), arqClientes);
        }

        else
        {
            fwrite(arv->clientes[i]->nome, sizeof(char), sizeof(arv->clientes[i]->nome), arqClientes);
            fwrite(&arv->clientes[i]->cod,  sizeof(int), 1, arqClientes);
            fwrite(arv->clientes[i]->tel, sizeof(char), sizeof(arv->clientes[i]->tel), arqClientes);
        }
    }

}

void escreveNovo(ArvB* arv, FILE*arqMeta, FILE*arqDados, FILE* arqNoInterno)
{
    int pos = proxPosicaoFolha(arqMeta);
    escreveCliente(arv, arqDados, pos);
    pos = proxPosicaoInterna(arqMeta);
    escreveNoInterno(arv, arqNoInterno, pos);
}

void atualizaNo(ArvB* arv, FILE* arqMeta, FILE*arqDados, FILE* arqNoInterno)
{

    escreveCliente(arv, arqDados, arv->pos);
    if(arv->pai != NULL)
        escreveNoInterno(arv->pai, arqNoInterno, arv->pai->pos);

    else
        escreveNoInterno(arv, arqNoInterno, arv->pos);

}

void imprimeClientes(FILE*arqClientes)
{
    printf("....................................... LISTA DE CLIENTES ....................\n");

    int cod;
    char nome[100], tel[20];
    int cont = 0, pos = 0 ;

    rewind(arqClientes);

    while( fread(nome, sizeof(char), sizeof(nome), arqClientes) > 0)
    {
        fread(&cod, sizeof(int), 1, arqClientes);
        fread(tel, sizeof(char), sizeof(tel), arqClientes);

        if(cod != -1)
        {
            printf("Nome: %s\n", nome);
            printf("Código: %d\n", cod);
            printf("Telefone: %s\n", tel);
            printf("_____________________________________________________\n");
        }
    }
}

void atualizaPai(FILE* arqMeta, int pos)
{
    rewind(arqMeta);
    fwrite(&pos, sizeof(int), 1, arqMeta);
}

void inserePaginaNaoCheia(ArvB* arv, Cliente* cliente, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno)
{

    int i = arv->numChaves, pai;
    int num = cliente->cod;

    while(i > 0 && num < arv->clientes[i- 1]->cod)
    {
        arv->clientes[i] = arv->clientes[i- 1];
        arv->chave[i] = arv->chave[i-1];
        i--;
    }


    arv->clientes[i] = cliente;
    arv->chave[i] = cliente->cod;
    arv->numChaves++;


    int pos;
    fseek(arqMeta, sizeof(int)*2, SEEK_SET);
    fread(&pos, sizeof(int), 1, arqMeta);


    if(arv->pai != NULL)
    {
        i= 0;
        while(arv->pai->filho[i]->clientes[0]->cod != arv->clientes[0]->cod)
            i++;

        arv->proxPag =  arv->pai->filho[i+1];

    }

    if(pos == 0)// raiz ainda é nula
    {

        arv->pos  = 0;

        escreveNovo(arv, arqMeta, arqDados, arqNoInterno);
        atualizaPai(arqMeta, arv->pos);
        //  escreveNoInterno(arv, arqNoInterno, arv->pos);

    }
    else
    {
        atualizaNo(arv, arqMeta, arqDados, arqNoInterno);
        //escreveNoInterno(arv, arqNoInterno ,arv->pos);
    }
}

ArvB* insereFilho(ArvB* pai, ArvB* filho, Cliente* cliente, FILE* arqMeta, FILE*arqDados, FILE* arqNoInterno)
{
    int i = pai->numChaves;
    int num = filho->clientes[0]->cod;

    while(i > 0 && num < pai->chave[i - 1])
    {
        pai->filho[i+1] = pai->filho[i];
        pai->chave[i] = pai->chave[i - 1];
        i--;
    }

    pai->chave[i] = cliente->cod;
    pai->filho[i+ 1]  = filho;
    pai->filho[i+1]->pai = pai;
    pai->numChaves++;
    pai->filho[i]->proxPag = pai->filho[i+1];
    pai->filho[i+1]->proxPag = pai->filho[i+2];
    pai->filho[i + 1]->pos = proxPosicaoFolha(arqMeta);
    atualizaNo(pai->filho[i + 1], arqMeta, arqDados, arqNoInterno);
    //atualizaNo(pai, arqMeta, arqDados);
    return pai;
}

void posicionaFilho(ArvB* pai, ArvB* filho, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno)
{

    int num = filho->clientes[0]->cod;
    int i = pai->numChaves;
    while(i > 0 && num < pai->chave[i - 1])
        i--;

    if ( (pai->filho[i] != NULL)  && ( pai->filho[i]->chave[0]!= num  ||  (pai->filho[i]->chave[0] == num && verificaFolha(pai->filho[i]) != 1)   ) )
        posicionaFilho(pai->filho[i], filho, arqMeta, arqDados, arqNoInterno);


    else
    {
        pai->filho[i] = filho;
        if(pai->filho[i-1] != NULL)
            pai->filho[i-1]->proxPag = pai->filho[i];

        pai->filho[i]->proxPag = pai->filho[i+1];
        pai->filho[i]->pai = pai;

        atualizaNo(pai->filho[i], arqMeta, arqDados, arqNoInterno);
    }

}

ArvB* particionaNoInterno(ArvB* arv, int num, FILE*arqMeta, FILE*arqDados, FILE* arqNoInterno)
{

    ArvB*  filho1 = criaVazia();
    ArvB* filho2 = criaVazia();

    int *v = (int*) malloc(sizeof(int) * (2*M+1)), pos = arv->pos;
    int i = 2*M;

    v = arv->chave;
    v[i] = num;


    for(int i = 0; i < M; i++)
    {

        filho1->chave[i] = v[i];
        filho2->chave[i] = v[i + M + 1];
        filho2->filho[i] = arv->filho[i + M + 1];
        filho1->filho[i] = arv->filho[i];

        if(filho1->filho[i-1] != NULL)
            filho1->filho[i-1]->proxPag = filho1->filho[i];
        if(filho2->filho[i-1] != NULL)
            filho2->filho[i-1]->proxPag = filho2->filho[i];
    }


    filho1->filho[M] = arv->filho[M];
    filho1->filho[M-1]->proxPag = filho1->filho[M];
    filho1->filho[M]->proxPag = filho2->filho[0];
    filho1->numChaves = M;
    filho2->numChaves = M;

    if(arv->pai == NULL)// caso a página seja a raiz
    {
        arv->numChaves = 1;
        arv->chave[0] = v[M];
        arv->clientes[0] = NULL;

        arv->pai = NULL;
        arv->pos = pos;
        filho2->pai = arv;
        filho1->pai = arv;


        for(int i = 0; i < filho1->numChaves+1; i++)
            if(filho1->filho[i]!= NULL)
                filho1->filho[i]->pai = filho1;

        for(int i = 0; i < filho2->numChaves+1; i++)
            if(filho2->filho[i]!= NULL)
                filho2->filho[i]->pai = filho2;


        arv->filho[0] = filho1;
        arv->filho[1]= filho2;

        arv->filho[0]->filho[arv->filho[0]->numChaves]->proxPag = arv->filho[1]->filho[0];

        for(int i = 1; i < 2*M; i++)// zera as clientes restantes da arvore (clientes a partir do M)
        {
            arv->chave[i] = 0;
            arv->clientes[i] = NULL;
            arv->filho[i + 1] = NULL;
        }

        arv->filho[0]->pos = proxPosicaoInterna(arqMeta);
        arv->filho[1]->pos = proxPosicaoInterna(arqMeta);

        atualizaPai(arqMeta, pos);
        escreveNoInterno(arv, arqNoInterno, pos);
        escreveNoInterno(arv->filho[0], arqNoInterno, arv->filho[0]->pos);
        escreveNoInterno(arv->filho[1], arqNoInterno, arv->filho[1]->pos);

    }

    return arv;
}

void particiona(ArvB* arv,  Cliente* cliente, FILE*arqMeta, FILE*arqDados, FILE* arqNoInterno)
{
    int pos = arv->pos;

    ArvB*  filho1 = criaVazia();
    ArvB* filho2 = criaVazia();

    Cliente** v = (Cliente**) malloc(sizeof(Cliente*));
    for(int i = 0; i < 1+2*M; i++)
        v[i] = (Cliente*) malloc(sizeof(Cliente));

    int i = 2*M ;
    for(int j = 0; j < 2*M; j++)
        v[j] = arv->clientes[j];

    v[i] = cliente;

    while(i > 0 && v[i]->cod < v[i-1]->cod)
    {
        Cliente* aux= v[i];
        v[i] = v[i- 1];
        v[i - 1] = aux;
        i--;
    }

    for(int i = 0; i < M; i++)
    {
        filho1->clientes[i] = v[i];
        filho1->chave[i] = v[i]->cod;
        filho2->clientes[i+1] = v[i + M+ 1];
        filho2->chave[i+1] = v[i+M+1]->cod;
    }

    filho1->pos = pos;
    filho1->filho[M] = arv->filho[M];
    filho2->clientes[0] = v[M];
    filho2->chave[0] = v[M]->cod;
    filho1->numChaves = M;
    filho2->numChaves = M+1;


    if(arv->pai == NULL)// caso a página seja a raiz
    {

        arv->numChaves = 1;
        arv->chave[0] = v[M]->cod;
        arv->clientes[0] = NULL;

        arv->pai = NULL;
        filho2->pai = arv;
        filho1->pai = arv;


        for(int i = 0; i < filho1->numChaves+1; i++)
            if(filho1->filho[i]!= NULL)
                filho1->filho[i]->pai = filho1;

        for(int i = 0; i < filho2->numChaves+1; i++)
            if(filho2->filho[i]!= NULL)
                filho2->filho[i]->pai = filho2;


        arv->filho[0] = filho1;
        arv->filho[1]= filho2;
        arv->filho[0]->proxPag = arv->filho[1];
        arv->pos = pos;

        for(int i = 1; i < 2*M; i++)// zera as clientes restantes da arvore (clientes a partir do M)
        {
            arv->chave[i] = 0;
            arv->clientes[i] = NULL;
            arv->filho[i + 1] = NULL;
        }

        arv->filho[1]->pos = proxPosicaoFolha(arqMeta);

        atualizaPai(arqMeta, pos);
        atualizaNo(arv->filho[0], arqMeta, arqDados, arqNoInterno);
        atualizaNo(arv->filho[1], arqMeta, arqDados, arqNoInterno);
    }

    else
    {


        if(arv->pai->numChaves == 2*M)// Caso a página pai também esteja cheia
        {

            filho2->pos = proxPosicaoFolha(arqMeta);

            arv->pai =  particionaNoInterno(arv->pai, v[M]->cod, arqMeta, arqDados, arqNoInterno);

            posicionaFilho(arv->pai, filho1, arqMeta, arqDados, arqNoInterno);
            posicionaFilho(arv->pai, filho2, arqMeta, arqDados, arqNoInterno);

            arv->pai->filho[0]->filho[arv->pai->filho[0]->numChaves]->proxPag = arv->pai->filho[1]->filho[0];

        }

        else
        {
            filho2->pai = arv->pai;
            filho1->pai = arv->pai;

            int i = arv->pai->numChaves;

            while(i > 0 && filho1->chave[0] < arv->pai->chave[i - 1])
                i--;


            arv->pai->filho[i]  = filho1;
            if(arv->pai->filho[i-1] != NULL)
                arv->pai->filho[i-1]->proxPag = arv->pai->filho[i];

            arv->pai =  insereFilho(arv->pai, filho2, v[M] , arqMeta, arqDados, arqNoInterno);

            atualizaNo(arv->pai->filho[i], arqMeta, arqDados, arqNoInterno);
        }

    }

}

void insere(ArvB* arv, Cliente* cliente, FILE* arqMeta, FILE* arqDados, FILE*arqNoInterno)
{
    int i = arv->numChaves;

    while(i > 0 &&  cliente->cod < arv->chave[i-1])
        i--;

    if(arv->chave[i-1] == cliente->cod)
        printf("CODIGO %d JA REGISTRADO! \n", cliente->cod);

    else
    {
        if(arv->numChaves == 2*M && verificaFolha(arv) == 1)
        {
            particiona(arv, cliente, arqMeta, arqDados, arqNoInterno);
        }

        else
        {

            if(verificaFolha(arv) == 0)
            {
                int i = arv->numChaves;
                while(i > 0 && cliente->cod < arv->chave[i - 1])
                    i--;
                insere(arv->filho[i], cliente, arqMeta, arqDados, arqNoInterno);
            }

            else
                inserePaginaNaoCheia(arv, cliente, arqMeta, arqDados, arqNoInterno);

        }

    }
}

void concatenacao(ArvB* arv, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno, int posIrmao)
{
    printf("\n\n\nCONCATENAÇÃO PROLIFERADA  \n");

    int i = 0, posTroca = arv->pai->pos;
    while(arv->pai->filho[i]->chave[0]!= arv->chave[0])
        i++;

    if(arv->numChaves - 1+ arv->pai->filho[posIrmao]->numChaves < 2*M)// condição para fazer a concatenação
    {

        arv->chave[arv->numChaves] = arv->pai->chave[i]; //recebe a chave do pai
        arv->numChaves++;
        arv->pai->numChaves--;
        arv->filho[arv->numChaves] = arv->pai->filho[posIrmao]->filho[0];// recebe o primeiro filho do irmão adjacente

        for(int j = 0; j < arv->pai->filho[posIrmao]->numChaves; j++)
        {
            arv->chave[arv->numChaves] = arv->pai->filho[posIrmao]->chave[j];
            arv->filho[arv->numChaves +1] = arv->pai->filho[posIrmao]->filho[j+1];// o primeiro filho é inserido depois
            arv->numChaves++;
        }


        if(arv->pai->numChaves == 0)
        {
            if(arv->pai->pai == NULL) // Caso o nó pai seja a raiz, deve- se atualizar o novo nó raiz no arquivo
                atualizaPai(arqMeta, arv->pos);


            ArvB* vazia = criaVazia();
            vazia->pos = arv->pai->filho[posIrmao]->pos;
            escreveNoInterno(vazia, arqNoInterno, vazia->pos);// escreve vazio no arquivo., na posição do irmão adjacente

            arv->pai->filho[posIrmao] = NULL;
            arv->pai = NULL;

            vazia->pos = posTroca; // escreve vazio no aruivo , na posição do pai
            escreveNoInterno(vazia, arqNoInterno, vazia->pos);
            escreveNoInterno(arv, arqNoInterno, arv->pos); // atualiza no aqrquivo, o próprio nó da árvore
        }
        else
            escreveNoInterno(arv, arqNoInterno, arv->pos);

    }

}

void redistribuicao(ArvB* arv, int pos, int posIrmao, int posPai, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno)
{

    printf("REDISTRIBUIÇÂO  FEITA\n");
    int i = 0,  posArv = arv->pos,  posArvAnt = arv->pai->filho[posIrmao]->pos;

    while(arv->pai->filho[i]->clientes[0]->cod != arv->clientes[0]->cod)
        i++;

    arv->filho[pos+1] = NULL;
    arv->chave[pos] = 0;
    arv->clientes[pos] = NULL;

    if(posIrmao == i+1)
    {
        printf("IRMÃO ADJACENTE À DIREITA\n");
        arv->pai->chave[posPai] = arv->pai->filho[posIrmao]->chave[1];
        // printf("\n\n%d\n",  arv->pai->filho[posIrmao]->chave[1]);
        arv->chave[pos] = arv->pai->filho[posIrmao]->chave[0];
        arv->clientes[pos] = arv->pai->filho[posIrmao]->clientes[0];

        arv->pai->filho[posIrmao]->chave[0] = 0;
        arv->pai->filho[posIrmao]->clientes[0] = NULL;

        if(arv->filho[pos+1]!= NULL)
            arv->filho[pos+1] = arv->pai->filho[posIrmao]->filho[0];


        for(int i = 0; i <  arv->pai->filho[posIrmao]->numChaves; i++)
        {
            arv->pai->filho[posIrmao]->chave[i] = arv->pai->filho[posIrmao]->chave[i+1];
            arv->pai->filho[posIrmao]->clientes[i] = arv->pai->filho[posIrmao]->clientes[i+1];
            arv->pai->filho[posIrmao]->filho[i] = arv->pai->filho[posIrmao]->filho[i+1];

            arv->pai->filho[posIrmao]->chave[i+1] = 0;
            arv->pai->filho[posIrmao]->clientes[i+1] = NULL;
            arv->pai->filho[posIrmao]->filho[i+1] = NULL;
        }

        arv->pai->filho[posIrmao]->numChaves--;

        if(pos != arv->numChaves-1)
        {
            int k = arv->numChaves-1;

            for(int i = 0; i < arv->numChaves; i++)
            {
                for(int j = 0; j < k; j++)
                {
                    if(arv->chave[j] > arv->chave[j + 1])
                    {
                        Cliente* aux  = arv->clientes[j];
                        int auxChave = arv->chave[j];
                        arv->clientes[j] = arv->clientes[j + 1];
                        arv->chave[j] = arv->chave[j+1];
                        arv->clientes[j+ 1] = aux;
                        arv->chave[j+1] = auxChave;
                    }
                }
                k--;
            }

        }

        // for(int i = 0; i < arv->pai->filho[posIrmao]->numChaves; i++)
        // {
        //         printf("IRMAOOO: %d\n", arv->pai->filho[posIrmao]->clientes[i]->cod);
        // }
    }

    else if(posIrmao == i-1)
    {
        printf("IRMÃO ADJACENTE À ESQUERDA\n");
        int numChavesIrmao = arv->pai->filho[posIrmao]->numChaves;

        arv->pai->chave[posPai] = arv->pai->filho[posIrmao]->chave[numChavesIrmao-1]; // Transfiro a ultima chave do irmão para o nó pai

        //avançamos as chaves, os filhos e clientes

        if(pos != 0)
        {
            for(int i = 2*M-1; i >0; i--)
            {
                arv->chave[i] = arv->chave[i-1];
                arv->clientes[i] = arv->clientes[i-1];
                arv->filho[i] = arv->filho[i-1];
            }
        }

        // printf("\n\n\n chave de 111= ==== %d\n", arv->chave[1]);

        arv->chave[0] = arv->pai->filho[posIrmao]->chave[numChavesIrmao-1];
        arv->clientes[0] = arv->pai->filho[posIrmao]->clientes[numChavesIrmao-1];
        arv->filho[0] = arv->pai->filho[posIrmao]->filho[numChavesIrmao];

        arv->pai->filho[posIrmao]->clientes[numChavesIrmao-1] = NULL;
        arv->pai->filho[posIrmao]->chave[numChavesIrmao-1] = 0;
        arv->pai->filho[posIrmao]->filho[numChavesIrmao] = NULL;
        arv->pai->filho[posIrmao]->numChaves--;

    }

    arv->pos = posArv;
    arv->pai->filho[posIrmao]->pos = posArvAnt;

    escreveCliente(arv, arqDados, arv->pos);
    escreveCliente(arv->pai->filho[posIrmao], arqDados, arv->pai->filho[posIrmao]->pos);
    escreveNoInterno(arv->pai, arqNoInterno, arv->pai->pos);

}

void excluiRegistro(ArvB* arv, int pos, FILE* arqMeta, FILE* arqDados, FILE* arqNoInterno){
    int i = 0;
    int posIrmao, posExclusao, posPai;

    if(arv->pai != NULL)
    {
        while(arv->pai->filho[i]->chave[pos] != arv->clientes[pos]->cod)
            i++;


        if(arv->pai->filho[i-1] != NULL && i-1 >=0)
        {
            posIrmao = i-1;
            posExclusao = i+1;
            posPai = i-1;
        }
        else if(arv->pai->filho[i+1] != NULL)
        {
            posIrmao = i+1;
            posExclusao = i-1;
            posPai = i;
        }

    }

    if(verificaFolha(arv) == 1 && arv->numChaves-1 >= M ||  (verificaFolha(arv) == 1 && arv->pai == NULL) )// Exclui registro de nó folha (numero de clientes - 1 >= M)
    {
        printf("EXCLUSÃO SIMPLES\n");
        arv->clientes[pos] = NULL;
        arv->chave[pos] = 0;


        for(int i = pos; i < arv->numChaves; i++)
        {
            arv->clientes[i] = arv->clientes[i + 1];
            arv->chave[i] = arv->chave[i+1];
            arv->clientes[i +  1] = NULL;
            arv->chave[i+1] = 0;
        }
        arv->numChaves--;

        //printf("arv->pos :: %d\n", arv->pos);

        atualizaNo(arv, arqMeta, arqDados, arqNoInterno);
    }

    else if(arv->numChaves - 1 < M) // concatena
    {

        //printf("ENTREI NA SEGUNDA CONDIÇÂO\n");
        int posTroca = arv->pos;
        int posTrocaPai = arv->pai->pos;

        if(arv->numChaves -  1+ arv->pai->filho[posIrmao]->numChaves  < 2*M)
        {
            printf("\n\n\nCONCATENENAÇÃO FEITA !\n");
            int posAnt = arv->pai->filho[posIrmao]->numChaves;


            arv->pai->chave[posPai] = 0; // "apaga " a chave do nó pai, já que ela não carrega dados
            arv->pai->numChaves--;

            for(int k = pos; k < arv->numChaves; k++) // reordena as clientes aós a exclusão do elemento
            {
                arv->clientes[k] = arv->clientes[k + 1];
                arv->chave[k] = arv->chave[k+1];
                arv->clientes[k +  1] = NULL;
                arv->chave[k+1] = 0;
                // arv->filho[k] = arv->filho[k+1];
                // arv->filho[k+1] = NULL;
                //printf("ANTES NUMCHAVES   %d\n", arv->numChaves);
            }

            arv->numChaves--;

            for(int j = 0; j < arv->numChaves; j++)// recebe as clientes e os filhos do nó que será eliminado
            {
                arv->pai->filho[posIrmao]->clientes[posAnt] = arv->clientes[j];
                arv->pai->filho[posIrmao]->chave[posAnt] = arv->chave[j];
                arv->pai->filho[posIrmao]->filho[posAnt + 1] = arv->filho[j + 1];///////

                if(arv->pai->filho[posIrmao]->filho[posAnt+ 1] != NULL)
                    arv->pai->filho[posIrmao]->filho[posAnt+1]->pai = arv->pai->filho[posIrmao];
                arv->pai->filho[posIrmao]->numChaves++;
                arv->clientes[j] = NULL;
                arv->chave[j] = 0;
                arv->filho[j+1] = NULL;
                arv->numChaves--;
                posAnt++;
            }


            int k = arv->pai->filho[posIrmao]->numChaves -1;
            for(int i = 0; i < arv->pai->filho[posIrmao]->numChaves; i++) // ordena os clientes após a concatenação
            {
                for(int j = 0; j < k; j++)
                {
                    if(arv->pai->filho[posIrmao]->clientes[j + 1]->cod < arv->pai->filho[posIrmao]->clientes[j]->cod)
                    {
                        Cliente* aux = arv->pai->filho[posIrmao]->clientes[j];
                        int auxChave = arv->pai->filho[posIrmao]->chave[j];
                        arv->pai->filho[posIrmao]->clientes[j] = arv->pai->filho[posIrmao]->clientes[j + 1];
                        arv->pai->filho[posIrmao]->clientes[j + 1]  = aux;
                        arv->pai->filho[posIrmao]->chave[j] = arv->pai->filho[posIrmao]->chave[j + 1];
                        arv->pai->filho[posIrmao]->chave[j + 1]  = auxChave;
                    }
                }

                k--;
            }

            for(int j = i; j < arv->pai->numChaves + 1; j++) // ordena as chaves e os filhos da arvore pai após a remoção de um nó
            {
                ArvB* aux = arv->pai->filho[j+1];
                arv->pai->filho[j] = aux;

                if(i != 0)
                {
                    arv->pai->chave[j-1] = arv->pai->chave[j];
                    arv->pai->chave[j] = 0;
                }

                else
                {
                    if(arv->pai->numChaves == 1 && arv->pai->chave[j+1] == 0){
                        arv->pai->chave[j] = arv->pai->filho[0]->chave[arv->pai->filho[0]->numChaves - 1];
                        //printf("\n\n\n\n\n DOIDÃO !!!!!!!!!!!!!!!!\n");
                    }
                    else{
                        arv->pai->chave[j] = arv->pai->chave[j+1];
                        arv->pai->chave[j+1] = 0;
                    }
                }
                arv->pai->filho[j+1] = NULL;
            }


            if(posIrmao == i-1)
            {
                printf("IRMÃO ADJACENTE À ESQUERDA\n");
                if(arv->pai->filho[i-1] != NULL)
                    arv->pai->filho[posIrmao]->proxPag = arv->pai->filho[i+1];
            }
            else if(posIrmao == i+1)
            {
                printf("IRMÃO ADJACENTE À DIREITA\n");
                if(arv->pai->filho[i-1] != NULL )
                    arv->pai->filho[i-1]->proxPag = arv->pai->filho[i+1];
            }
            // printf("pos do irmão :: %d\n", posIrmao);

            ArvB* vazia = criaVazia();


               arv->pai->filho[i] = NULL;

             if(arv->pai->numChaves == 0)
             {
                  atualizaPai(arqMeta, arv->pai->filho[posIrmao]->pos);
                  arv->pai->filho[posIrmao]->pai = NULL;
                  vazia->pos = posTrocaPai;
                  atualizaNo(vazia, arqMeta, arqDados, arqNoInterno);
             }
             else
                     atualizaNo(arv->pai, arqMeta, arqDados, arqNoInterno);

            vazia->pos = posTroca;
            escreveCliente(vazia, arqDados, vazia->pos);

            if(posIrmao != 0)
                atualizaNo(arv->pai->filho[posIrmao-1], arqMeta, arqDados, arqNoInterno);
            else
                atualizaNo(arv->pai->filho[posIrmao], arqMeta, arqDados, arqNoInterno);


            if(arv->pai->numChaves < M && arv->pai->pai != NULL)// verifico se o pai é raiz
            {
                if(arv->pai->pai->filho[i+1] != NULL)
                    concatenacao(arv->pai, arqMeta, arqDados, arqNoInterno, i+1);

                else if(arv->pai->pai->filho[i-1] != NULL)
                    concatenacao(arv->pai, arqMeta, arqDados, arqNoInterno, i-1);

                // arv->pai->pai = NULL;
                // arv->pai = NULL;
                free(arv->pai->pai);
                arv->pai = arv;
                //printf("\n\n\n\n\n ARV CONCAT : %d", arv->chave[0]);
            }

            //     arv = NULL;
        }
        else
        {
            redistribuicao(arv, pos, posIrmao, posPai, arqMeta, arqDados, arqNoInterno);
        }

    }

    else
    {

        int i;

        arv->clientes[pos] = arv->filho[pos + 1]->clientes[0];

        arv->filho[pos + 1]->clientes[0] = NULL;
        for( i = 0; i < arv->filho[pos+1]->numChaves; i++)
        {
            arv->filho[pos + 1]->clientes[i] = arv->filho[pos + 1]->clientes[i + 1];
            arv->filho[pos + 1]->filho[i] = arv->filho[pos+1]->filho[i+1];
            arv->filho[pos +1]->clientes[i +  1] = NULL;
        }

        arv->filho[pos+1]->numChaves--;
        arv->filho[pos+1]->filho[i] = NULL;

        atualizaNo(arv, arqMeta, arqDados, arqNoInterno);
        atualizaNo(arv->filho[pos+1], arqMeta, arqDados, arqNoInterno);

    }
}

void exclui(ArvB* arv, int num, FILE* arqMeta, FILE* arqDados , FILE* arqNoInterno)
{

    if(arv == NULL)
        printf("REGISTRO NÃO ENCONTRADO\n");

    else
    {

        int i = arv->numChaves;

        while(i > 0 && num < arv->chave[i-1])
            i--;


        if(verificaFolha(arv) == 1 && num == arv->clientes[i -1]->cod)
        {
            excluiRegistro(arv, i-1, arqMeta, arqDados, arqNoInterno);
        }

        else
        {
            exclui(arv->filho[i], num, arqMeta, arqDados, arqNoInterno);
        }
    }

}

void insercao(ArvB* arv, FILE* arqMeta, FILE*arqDados, FILE*arqNoInterno)
{
    Cliente* c = (Cliente*) malloc(sizeof(Cliente));

    printf("\n\n\n______________EFETUAR REGISTRO DE CLIENTE_______________\n");
    printf("Digite o código :\n");
    scanf("%d", &c->cod);

    printf("Digite o nome : \n");
    scanf("%s", c->nome);

    printf("Insira o telefone: \n");
    scanf("%s", c->tel);

    insere(arv, c, arqMeta, arqDados, arqNoInterno);

}

void exclusao(ArvB* arv, FILE* arqMeta, FILE* arqDados ,FILE* arqNoInterno)
{
    int cod;

    printf("\n\n\n______________APAGAR REGISTRO_____________________\n");
    printf("Digite o código do cliente:\n");
    scanf("%d", &cod);

    exclui(arv, cod, arqMeta, arqDados, arqNoInterno);

}

int busca(ArvB* arv, int num)
{

    if(arv == NULL)
    {
        printf("REGISTRO NÃO ENCONTRADO\n");
        return 0;
    }
    else
    {
        int i = arv->numChaves;

        while(i > 0 && num < arv->chave[i-1])
            i--;

        if(verificaFolha(arv) == 1 && num == arv->chave[i-1])
        {
            printf("\n\n\nREGISTRO  ENCONTRADO .\n");
            printf("Nome: %s\n", arv->clientes[i-1]->nome);
            printf("Código: %d\n\n\n", arv->clientes[i-1]->cod);
            return 1;
        }

        else
            busca(arv->filho[i], num);
    }

}

void procura(ArvB* arv)
{
    int cod;

    printf("\n\n\n_______________BUSCAR REGISTRO DO CLIENTE______\n");
    printf("Digite o código do cliente: \n");
    scanf("%d", &cod);

    busca(arv, cod);
}

void buscaNome(ArvB* arv, char nome[100])
{
    printf("..\n");

    if(arv == NULL)
        printf("REGISTRO NÃO ENCONTRADO\n");

    else
    {
        if(verificaFolha(arv) == 0)
            buscaNome(arv->filho[0], nome);

        else
        {
            int find;
            ArvB* aux;
            for(aux = arv; aux != NULL && find == 0; aux = aux->proxPag)
            {
                // printf("\n\nprimeira = %d\n", aux->chave[0]);
                for(int i = 0; i < aux->numChaves && find == 0 ; i++)
                {
                    if( strcmp(nome, aux->clientes[i]->nome) == 0)
                    {
                        printf("\n\n\n____________________________REGISTRO ENCONTRADO__________________________________________\n");
                        printf("Nome: %s\nCódigo: %d\nTelefone: %s\n",  aux->clientes[i]->nome, aux->clientes[i]->cod, aux->clientes[i]->tel);
                        printf("____________________________________________________________________________________________________________\n");
                        find = 1;
                    }
                }
            }

            if(find == 0)
                printf("REGISTRO NÃO ENCONTRADO\n");
        }

    }
}

void procuraNome(ArvB* arv)
{
    char nome[100];

    printf("\n\n\n_______________BUSCA DE CLIENTE POR NOME______\n");
    printf("Digite o nome do cliente: \n");
    scanf("%s", nome);

    buscaNome(arv, nome);
}

void imprimeClienteInterno(FILE*arqDados)
{
    printf("....................................... LISTA DE CLIENTES ....................\n");

    int cod;
    char nome[100], tel[20];
    int cont = 0, pos = 0 ;

    rewind(arqDados);

    while( fread(nome, sizeof(char), sizeof(nome), arqDados) > 0)
    {
        fread(&cod, sizeof(int), 1, arqDados);
        fread(tel, sizeof(char), sizeof(tel), arqDados);

        printf("Nome: %s\n", nome);
        printf("Código: %d\n", cod);
        printf("Telefone: %s\n", tel);
        printf("_____________________________________________________\n");

        if(cont == 3)
        {
            printf("......................................POSICAO................:: %d\n", pos);
            cont = 0;
            pos++;
        }
        else
            cont++;

    }
}

void imprimeNoInternoClt(FILE* arqNoInterno)
{

    printf("...............................NÓS INTERNOS.............................\n");

    int num;
    rewind(arqNoInterno);

    while(fread(&num, sizeof(int), 1, arqNoInterno) > 0)
    {
        printf("Número de chaves: %d\n", num);

        fread(&num, sizeof(int), 1, arqNoInterno);
        if(num == 1)
            printf("Aponta pra nó folha.\n");
        else
            printf("Aponta para nó interno\n");

        fread(&num, sizeof(int), 1, arqNoInterno);
        printf("Nó pai: %d\n", num);

        printf("Chaves: \n");
        for(int i = 0; i < 2*M; i++)
        {
            fread(&num, sizeof(int), 1, arqNoInterno);
            printf("%d\n", num);
        }

        printf("Filhos: \n");
        for(int i = 0; i < 2*M+ 1; i++)
        {
            fread(&num, sizeof(int), 1, arqNoInterno);
            printf("%d\n", num);
        }

        printf("_______________________________________________________\n");
    }

}

void imprimeMetaDados(FILE* arqMeta)
{
    rewind(arqMeta);
    int num;
    printf("                            METADADOS \n");
    while(fread(&num, sizeof(int), 1, arqMeta) > 0)
    {
        printf("Raiz: %d\n", num);
        fread(&num, sizeof(int), 1, arqMeta);
        //printf("Flag: %d\n", num);
        fread(&num, sizeof(int), 1, arqMeta);
        printf("Próxima Posição de folha: %d\n", num);
        fread(&num, sizeof(int), 1, arqMeta);
        printf("Próxuma posição interna: %d\n", num);
    }
}

void internoCliente(FILE* arqDados, FILE* arqNoInterno, FILE* arqMeta)
{
    imprimeClienteInterno(arqDados);
    imprimeNoInternoClt(arqNoInterno);
    imprimeMetaDados(arqMeta);
}
