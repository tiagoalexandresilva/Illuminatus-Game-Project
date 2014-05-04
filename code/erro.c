#include <stdio.h>
#include "erro.h"

typedef struct erro
{
  char *err_code;
  char *mensagem;
} ERRO;

static ERRO erro[] =
{
  {"E_NOERR",     				"Tudo normal"},
  {"E_COMMAND",     				"Comando nao existe"},
  {"E_ARGS",      				"Erro de argumentos a mais ou a menos"},
  {"E_NO_BOARD",      			"Ainda nao ha tabuleiro"},
  {"E_NO_FILE",     				"Ficheiro nao existe"},
  {"E_FILE_FORMAT",     		"Formato do ficheiro errado"},
  {"E_WRONG_SOLUTION",    		"Ha erro na solucao"},
  {"E_NO_MOVES",      			"Nao ha mais movimentos para anular"},
  {"E_COORDS",      				"Coordenadas erradas"},
  {"E_SAVE",      				"Nao se consegue gravar o ficheiro"},
  {"E_INVARGS",         		"Argumentos invalidos"},
  {"E_MEM",							"Erro de alocacao de memoria"},
  {"E_BLOC",						"Casa bloqueada"}
};

/**
 * Reporta o erro escrevendo uma mensagem no standard error
 * @param erro_num O nmero do erro que vem da tabela de erros. Usar SEMPRE o define e no o valor numrico para maior legibilidade.
 */
int mensagem_de_erro(int erro_num)
{
  (void) fprintf(stderr, "%s: %s\n", erro[erro_num].err_code, erro[erro_num].mensagem);
  return erro_num;
}