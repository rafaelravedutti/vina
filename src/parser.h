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

#ifndef __PARSER_H__
#define __PARSER_H__

#include <members.h>

/** @brief Processa argumentos
 *
 * Processa os argumentos passados na execução do programa e os atribue a archive (nome do arquivador) e memlist (lista ligada com os membros passados)
 *
 * @param argc
 * @param argv
 * @param archive 
 * @param memlist
 *
 * @return Caso nenhum erro ocorra retorna 0, caso ocorra erro na alocação de memória é retornado o código ERR_MEMALLOC
 */
int parse_vina_args(int argc, char *argv[], char **archive, struct memberlist *memlist);

#endif // __PARSER_H__
