/*
 *  Copyright (C) 2013  Gabriel Moro Sabedotti Chemim
 *                      Rafael Ravedutti Lucio Machado
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

#ifndef __MEMBERS_H__
#define __MEMBERS_H__

#include <vina.h>

/* Estrutura com os dados de cada membro */
struct member {
    char filename[MAX_FILENAME];  /* Nome do arquivo/membro */
    struct member *next;          /* Ponteiro para o próximo membro */
};

/* Estrutura com os dados de cada lista de membros */
struct memberlist {
    unsigned int members;     /* Quantidade de membros da lista */
    struct member *base;    /* Ponteiro para a base (primeiro membro) da lista */
    struct member *last;    /* Ponteiro para o último membro da lista */
};

/* Aloca uma lista de membros vazia */
struct memberlist *allocMemberList();

/** @brief Adiciona arquivo
 *
 * Função que adiciona arquivo a uma lista de membros
 *
 * @param memlist Lista de membros a serem removidos
 * @param filename Nome do arquivo
 *
 * @return Se nenhum erro ocorrer retornamos 0
 */
void addFile(struct memberlist *memlist, const char *filename);

/** @brief Libera memória
 *
 * Libera a memória ocupada pela lista de membros apontada por memlist
 *
 * @param memlist Lista de membros a serem removidos
 *
 * @return Se nenhum erro ocorrer retornamos 0
 */
void freeMemberList(struct memberlist *memlist);

#endif // __MEMBERS_H__
