#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "erro.h"
#include "main.h"
#include "listas_ligadas.h"

/**Função que guarda todas as jogadas que mudam o estado do tabuleiro.
 * @param jogo Recebe o tabuleiro.
 * @param col Indica a coluna da célula.
 * @param lin Indica a linha da célula.
 * @param ant Indica o estado anterior da célula.
 * @param est Indica o estado da célula (1 ou 0), se é iluminado ou não-iluminado.
 */
void insere_Jogadas(Tabuleiro jogo, int col, int lin, char ant, int est)
{
  Jogadas aux=jogo->jogadas;   	
  jogo->jogadas = (Jogadas)malloc(sizeof(NodoJogadas));
  jogo->jogadas -> coluna = col;
  jogo->jogadas -> linha = lin;
  jogo->jogadas -> anterior = ant;
  jogo->jogadas -> n_comando = jogo->n_comando;
  jogo->jogadas -> estado = est;
  jogo->jogadas -> seg = aux;
}

/**Função que remove todas as jogadas que mudaram o estado do tabuleiro e volta a por como estava o tabuleiro antes das execuções dos comandos, a cada execução do anula feita pelo utilizador.
 *@param jogo Recebe o tabuleiro.
 *@return aux Variável que é retornada no final da remoção de todas as jogadas.
 */
void remove_Jogadas(Tabuleiro jogo)
{
  int coluna, linha, est;
  char car;
  Jogadas aux=NULL;
  if(jogo->jogadas)
    {
      aux=jogo->jogadas->seg;
      coluna = jogo->jogadas->coluna;
      linha = jogo->jogadas->linha;
      car = jogo->jogadas->anterior;
      est = jogo->jogadas->estado;
      jogo->marcas[coluna][linha] = est;
      jogo->tabuleiro[coluna][linha] = car;
      free(jogo->jogadas);
      jogo->jogadas=aux;
    }
}

/**Função responsável por anular cada jogada efectuada pelo utilizador a cada chamada da função.
 *@param jogo Recebe o tabuleiro.
 *@return 1 Tudo correu conforme o esperado.
 */
int anula(Tabuleiro jogo)
{
  int comando;
  if(!verifica_tab(jogo)){ 
    return mensagem_de_erro(E_NO_BOARD);
  }
  if(jogo->jogadas==NULL)
    {
      impressao_ecra(jogo);
      return mensagem_de_erro(E_NO_MOVES);
    }
  else
    {
      comando = jogo->jogadas->n_comando;
      while(jogo->jogadas && jogo->jogadas->n_comando == comando)
   	{
	  		remove_Jogadas(jogo);
   	}		
      impressao_ecra(jogo);
      return 1;
    }
  return 1;
}
