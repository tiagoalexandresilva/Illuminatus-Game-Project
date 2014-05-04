#ifndef ___ERROS_H___
#define ___ERROS_H___

#define E_NOERR			 			0 		/**             Tudo normal              **/
#define E_COMMAND			 			1 		/**          Comando nao existe          **/
#define E_ARGS				 			2 		/** Erro de argumentos a mais ou a menos **/
#define E_NO_BOARD		 			3 		/**        Ainda nao ha tabuleiro        **/
#define E_NO_FILE			 			4 		/**          Ficheiro nao existe         **/
#define E_FILE_FORMAT				5 		/**      Formato do ficheiro errado      **/
#define E_WRONG_SOLUTION		 	6 		/**          Ha erro na solucao          **/
#define E_NO_MOVES			 		7 		/**  Nao ha mais movimentos para anular  **/
#define E_COORDS			 			8 		/**         Coordenadas erradas          **/
#define E_SAVE				 			9 		/**  Nao se consegue gravar o ficheiro   **/
#define E_INVARGS						10 	/**         Argumentos invalidos         **/
#define E_MEM							11 	/**     Erro de alocacao de memoria      **/
#define E_BLOC							12 	/**            Casa bloqueada            **/

/**
* Reporta o erro escrevendo uma mensagem no standard error
* @param erro_num O número do erro que vem da tabela de erros. Usar SEMPRE o define e não o valor numérico para maior legibilidade.
*/
int mensagem_de_erro(int erro_num);

#endif

