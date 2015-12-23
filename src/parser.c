/*
 *  Copyright (C) 2013  Gabriel Moro Sabedotti Chemim
 *            Rafael Ravedutti Lucio Machado
 *
 *  This file is part of Vina
 *
 *  Vina is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Vina is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Vina.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
//----
#include <vina.h>
#include <parser.h>

/* 
 * Processa os argumentos passados na execução do programa 
 * e os atribue a archive (nome do arquivador) e memlist
 * (lista ligada com os membros passados)
 */
int parse_vina_args(int argc, char *argv[], char **archive, struct memberlist *memlist) {
  unsigned int i;

  *archive = NULL;

  for(i = 1; i < argc; ++i) {
    /* Verificamos se o parâmetro passado não é uma opção */
    if(argv[i][0] != '-') {

      /*
       * Se archive ainda não foi definido, então o definimos
       * como o primeiro argumento, do contrário, o argumento
       * é um membro, então o adicionamos a lista de membros
       */

      if(*archive == NULL) {
        *archive = strdup(argv[i]);

        /* Caso archive não foi alocado, retornamos um código de erro */
        if(*archive == NULL) {
          fprintf(stderr, "Erro ao alocar memória\n");
          return ERR_MEMALLOC;
        }
      } else {
        addFile(memlist, argv[i]);
      }
    /* Se a opção -m (mover) for especificada, então pulamos
     * o parâmetro targ especificado nela            */
    } else if(argv[i][1] == 'm') {
      ++i;
    }
  }

  return 0;
}
