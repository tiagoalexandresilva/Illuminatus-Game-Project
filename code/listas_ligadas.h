#ifndef ___LISTAS_LIGADAS_H___
#define ___LISTAS_LIGADAS_H___

/**
*Estrutura de dados onde são guardadas todas as alterações efectuadas no tabuleiro das marcas.
*/
typedef struct sJogadas
{
 /**Variável que guarda o estado anterior da célula.*/
 char anterior;
 /**Variável que guarda o estado da célula (1 ou 0), se é iluminado ou não-iluminado.*/ 
 int estado;
 /**Variável que guarda a coluna da célula.*/
 int coluna;
 /**Variável que guarda a linha da célula.*/
 int linha;
 /**Variável que guarda uma certa identidade de um comando*/
 int n_comando;
 /**Apontador para a próxima jogada.*/
 struct sJogadas *seg;
} *Jogadas, NodoJogadas;

void insere_Jogadas(Tabuleiro jogo, int coluna, int linha, char ant, int est);
void remove_Jogadas(Tabuleiro jogo);
int anula(Tabuleiro jogo);

#endif
