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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//----
#include <members.h>

/* Aloca uma lista de membros vazia e retorna um ponteiro para o seu endereço */
struct memberlist *allocMemberList() {
    struct memberlist *memlist;

    memlist = (struct memberlist *) malloc(sizeof(struct memberlist));

    if(memlist != NULL) {
        memlist->members = 0;
        memlist->base = NULL;
        memlist->last = NULL;
    }

    return memlist;
}

/* Adiciona um arquivo a uma lista de membros */
void addFile(struct memberlist *memlist, const char *filename) {
    struct member *m;

    m = (struct member *) malloc(sizeof(struct member));

    if(m != NULL) {
        /* Atribuimos os dados do membro */
        memcpy(m->filename, filename, sizeof(m->filename));
        m->next = NULL;

        /*
         * Se a base da lista não existir, então fazemos de m a base,
         * do contrário, definimos m como o próximo elemento do último e,
         * em seguida, m como o último membro, mantendo a lista ligada.
         */

        if(!memlist->base) {
            memlist->base = memlist->last = m;
        } else {
            memlist->last->next = m;
            memlist->last = m;
        }

        /* Aumenta a quantidade de membros da lista */
        ++memlist->members;
    }
}

/* Libera a memória ocupada pela lista de membros apontada por memlist */
void freeMemberList(struct memberlist *memlist) {
    struct member *m, *nextmem;

    if(memlist != NULL) {

        /*
         * Percorremos a lista de membros e vamos liberando cada membro
         * um por um, após isto, já podemos liberar o espaço de memória
         * apontado por memlist.
         */

        m = memlist->base;

        while(m != NULL) {
            nextmem = m->next;
            free(m);
            m = nextmem;
        }

        free(memlist);
    }
}
