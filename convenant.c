#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum { TAM_NOME = 81, VIDA_BOSS = 200, BONUS_VITORIA = 100 };

typedef struct Pontuacao {
    char nome[TAM_NOME];
    int pontos;
    struct Pontuacao *proximo;
} Pontuacao;

//Descarta o excedente para que cada tentativa consuma uma linha inteira.
static int lerLinha(char *texto, int tamanho)
{
    if (fgets(texto, tamanho, stdin) == NULL)
        return 0;

    char *quebra = strchr(texto, '\n');
    if (quebra != NULL) {
        *quebra = '\0';
    } else {
        int caractere = getchar();
        if (caractere != '\n' && caractere != EOF) {
            while ((caractere = getchar()) != '\n' && caractere != EOF)
                ;
            return -1;
        }
    }
    return 1;
}

//Retorna -1 em fim de entrada e -2 para uma escolha inválida.
static int lerOpcao(void)
{
    char linha[32];
    int estado = lerLinha(linha, sizeof linha);
    if (estado <= 0)
        return estado == 0 ? -1 : -2;

    char *inicio = linha;
    while (isspace((unsigned char)*inicio))
        inicio++;
    if (*inicio < '0' || *inicio > '9')
        return -2;

    int opcao = *inicio++ - '0';
    while (isspace((unsigned char)*inicio))
        inicio++;
    return *inicio == '\0' ? opcao : -2;
}

static int lerNome(char nome[TAM_NOME])
{
    for (;;) {
        printf("Nome do jogador: ");
        int estado = lerLinha(nome, TAM_NOME);
        if (estado == 0)
            return 0;
        if (estado > 0 && nome[strspn(nome, " \t\r\v\f")] != '\0')
            return 1;
        puts("Digite um nome válido de até 80 bytes.");
    }
}

static int registrarPontuacao(Pontuacao **inicio, const char *nome, int pontos)
{
    Pontuacao *novo = malloc(sizeof *novo);
    if (novo == NULL)
        return 0;

    strcpy(novo->nome, nome);
    novo->pontos = pontos;

    //Encontra o elo a atualizar e mantém empates na ordem de cadastro.
    while (*inicio != NULL && (*inicio)->pontos >= pontos)
        inicio = &(*inicio)->proximo;

    novo->proximo = *inicio;
    *inicio = novo;
    return 1;
}

static void exibirRanking(const Pontuacao *atual)
{
    puts("\nRanking de pontuações");
    if (atual == NULL)
        puts("Nenhuma partida registrada.");

    size_t posicao = 1;
    for (; atual != NULL; atual = atual->proximo)
        printf("%zu. %s - %d pontos\n", posicao++, atual->nome, atual->pontos);
}

static void liberarRanking(Pontuacao *atual)
{
    //Preserva o próximo nó antes de liberar o nó atual.
    while (atual != NULL) {
        Pontuacao *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
}


//mostra  vida do jogador e do boss
void exibirstatus (float vidajogador, float vidaboss) {

    printf("\nStatus da Batalha\n");
    printf("\nSua vida: %.2f\n", vidajogador);
    printf("\nVida do Reinhard: %.2f\n", vidaboss);
}

//mostra os ataques disponiveis
void mostrarMenu(void) {

    printf("\n1- Sorcery (15 de dano)\n");
    printf("\n2- Yetzirah (10 de dano)\n");
    printf("\n3- Atziluth (20 de dano)\n");
    printf("\n4- Dura Lex Sed Lex (20 de dano)\n");
    printf("\n5- Se defender e se curar (0 de dano, 15 de HP restaurado )\n");
    printf("\nEscolha: \n");
}


//Retorna a pontuação da partida ou -1 se a entrada for encerrada.
static int batalha(float vidaJogador, float vidaboss, int turno) {

    int escolha;
    int danoJogador = 0;
    int escudo = 0;
    int danoBoss = 15;


    //verifica se o jogador foi derrotado
    if (vidaJogador <= 0) {
        printf("Derrota.");
        printf("Reinhard: Foi uma boa luta, meu amigo, mas você ainda não é páreo.");

        return VIDA_BOSS - (int)vidaboss;
    }

    printf("\nTurno %d:\n", turno);
    exibirstatus(vidaJogador, vidaboss);
    mostrarMenu();


    //escolhas dos turnos
    do {
        escolha = lerOpcao();
        if (escolha == -1)
            return -1;
        if (escolha < 1 || escolha > 5)
            printf("\nEscolha inválida. Digite uma opção de 1 a 5: ");
    } while (escolha < 1 || escolha > 5);
    switch(escolha) {
        case 1:
        danoJogador = 15;
        printf("Mercurius: Usou Sorcery");
        break;

        case 2:
        danoJogador = 10;
        printf("Mercurius: Usou Yetzirah");
        break;

        case 3:
        danoJogador = 20;
        printf("Mercurius: Usou Atziluth");
        break;

        case 4:
        danoJogador = 20;
        printf("Mercurius: Usou Dura Lex Sed Lex");
        break;

        case 5:
        danoJogador = 0;
        vidaJogador = vidaJogador + 15;
        escudo = 1;
        printf("Mercurius: Usou defender e se curou");
        break;

    }


 //aplica o dano no boss
    vidaboss = vidaboss - danoJogador;

    printf("\nVoce causou %d de dano!\n", danoJogador);

    if (vidaboss <=0)
    {
    printf("\nVITORIA!\n");
    printf("\nVoce derrotou o Boss!\n");
    printf("\nTurnos utilizados: %d\n", turno);
    return VIDA_BOSS + BONUS_VITORIA;
    }

    if (escudo == 1) {
    printf("\nVoce usou a defesa\n");
    printf("\nVoce recuperou 15 de HP!\n");
    printf("\nO ataque do Boss foi bloqueado!\n");

    return batalha(vidaJogador, vidaboss, turno + 1);
    }

 //turno do boss
    printf("\nTURNO DO BOSS\n");
    printf("\nO Reinhard atacou voce!\n");
    printf("\nO Reinhard causou %d de dano!\n", danoBoss);

    vidaJogador = vidaJogador - danoBoss;

    //recursao, chama a propria funcao para comecar um novo turno
    return batalha(vidaJogador, vidaboss, turno + 1);


}

static int jogar(void)
{
    float vidaJogador = 100;
    float vidaboss = VIDA_BOSS;

    printf("\nReinard apareceu!\n");
    printf("\nPrepare-se para a batalha!\n");

    printf("\nMercurius: Ah… finalmente, o homem que carrega o céu sobre os ombros. ");
    printf("\nReinhard: E você deve ser o homem que se esconde além dele. ");

    return batalha(vidaJogador, vidaboss, 1);
}

int main(void)
{
    Pontuacao *ranking = NULL;

    for (;;) {
        puts("\n1 - Jogar\n2 - Ver ranking\n0 - Sair");
        printf("Escolha: ");
        int opcao = lerOpcao();

        if (opcao == -1 || opcao == 0)
            break;

        if (opcao == 1) {
            char nome[TAM_NOME];
            if (!lerNome(nome))
                break;

            int pontos = jogar();
            if (pontos < 0)
                break;

            printf("\n%s obteve %d pontos.\n", nome, pontos);
            if (!registrarPontuacao(&ranking, nome, pontos))
                fputs("Memória insuficiente para registrar a pontuação.\n", stderr);
        } else if (opcao == 2) {
            exibirRanking(ranking);
        } else {
            puts("Opção inválida.");
        }
    }

    liberarRanking(ranking);
    puts("\nFim do jogo.");
    return 0;
}
