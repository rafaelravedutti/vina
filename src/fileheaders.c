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
#include <sys/stat.h>
#include <unistd.h>
//----
#include <vina.h>
#include <fileheaders.h>

/* Gera o cabeçalho de um arquivo */
int makeHeader(const char *filename, struct fileheader *fhdr) {
    struct stat stats;
    unsigned int namelen;

    stat(filename, &stats);
    namelen = strlen(filename);

    /* Copia o nome com no máximo MAX_FILENAME bytes e preenche
     * o espaço que sobrar com bytes nulos                            */
    memcpy(fhdr->fname, filename, MIN(namelen, MAX_FILENAME));
    if(namelen < MAX_FILENAME) {
        memset(fhdr->fname + namelen, 0, MAX_FILENAME - namelen);
    }

    fhdr->fuid = stats.st_uid;
    fhdr->fgid = stats.st_gid;
    fhdr->fperm = stats.st_mode & 0777;
    fhdr->fsize = stats.st_size;
    fhdr->fmodtime = stats.st_mtime;
    return 0;
}

/* Altera o cabeçalho de um arquivo */
int changeHeader(const char *filename, struct fileheader *fhdr) {
    if(chown(filename, fhdr->fuid, fhdr->fgid)) {
        fprintf(stderr, "Erro ao mudar o dono do arquivo \"%s\".", filename);
        return ERR_CHOWN;
    }

    if(chmod(filename, fhdr->fperm & 0777)) {
        fprintf(stderr, "Erro ao mudar as permissões do arquivo \"%s\".", filename);
        return ERR_CHMOD;
    }

    return 0;
}

/*
 * Encontra os dados do cabeçalho de um membro em um arquivador
 * a partir do seu nome, retorna a posição que o arquivo se
 * encontra dentro do arquivador, se o membro não existir
 * é retornado o valor -1
 */
int findHeaderPos(const char *filename, struct fileheader *fheaders, unsigned int nfiles) {
    struct fileheader *fhdr;
    unsigned int count = 0;

    if(fheaders != NULL) {
        for(fhdr = fheaders; count < nfiles; ++fhdr, ++count) {
            if(!strcmp(fhdr->fname, filename)) {
                return count;
            }
        }
    }

    return -1;
}

/*
 * Lê os cabeçalhos dos membros de um arquivador e os
 * armazena em uma lista de cabeçalhos, esta função
 * irá mover o apontador do arquivo para o final
 */
int readHeaders(FILE *arcp, struct fileheader **fheaders, unsigned int nfiles) {
    /* Existem cabeçalhos para serem lidos? */
    if(!nfiles) {
        *fheaders = NULL;
        return 0;
    }

    if((*fheaders = (struct fileheader *) malloc(nfiles * sizeof(struct fileheader))) == NULL) {
        fprintf(stderr, "Falha ao alocar cabeçalhos dos membros!\n");
        return ERR_MEMALLOC;
    }

    /* Posicionamos o apontador do arquivo na parte que começam os
     * os cabeçalhos e então fazemos a leitura                        */
    fseek(arcp, -(nfiles * sizeof(struct fileheader)), SEEK_END);
    fread(*fheaders, sizeof(struct fileheader), nfiles, arcp);
    return 0;
}

/*
 * Escreve uma lista de cabeçalhos em um arquivador, esta
 * função irá mover o apontador do arquivo para o final
 */
void writeHeaders(FILE *arcp, struct fileheader *fheaders, unsigned int nfiles) {
    fseek(arcp, -(nfiles * sizeof(struct fileheader)), SEEK_END);
    fwrite(fheaders, sizeof(struct fileheader), nfiles, arcp);
}

/* Adiciona um cabeçalho a uma lista de cabeçalhos */
int addHeader(struct fileheader **fheaders, struct fileheader *fhdr, unsigned int *nfiles) {
    /* Se a lista de cabeçalhos já está alocada, então a realocamos
     * adicionando mais um bloco para o cabeçalho, se ela não está
     * alocada ainda, então a alocamos já com a posição para
     * o cabeçalho                                                    */

    if(*fheaders != NULL) {
        *fheaders = (struct fileheader *) realloc(*fheaders, (*nfiles + 1) * sizeof(struct fileheader));
    } else {
        /* Se existem mais cabeçalhos mas a lista aponta para um local
         * inválido, então retornamos um código de erro                    */
        if(*nfiles) {
            fprintf(stderr, "Lista de cabeçalhos corrompida.\n");
            return ERR_HDRLIST;
        }

        *fheaders = (struct fileheader *) malloc(sizeof(struct fileheader));
    }

    if(*fheaders == NULL) {
        fprintf(stderr, "Erro ao alocar memória para lista de cabeçalhos!\n");
        return ERR_MEMALLOC;
    }

    /* Copiamos o cabeçalho para a última posição e incrementamos
     * o número de arquivos no cabeçalho principal do arquivador         */
    memcpy(*fheaders + *nfiles, fhdr, sizeof(struct fileheader));
    ++(*nfiles);
    return 0;
}

/* Deleta um cabeçalho de uma lista de cabeçalhos dada sua posição */
int deleteHeader(struct fileheader **fheaders, int pos, unsigned int *nfiles) {
    /* A posição do cabeçalho é válida no arquivador? */
    if(pos > -1 && pos < *nfiles) {
        /* A partir de pos, transferimos os dados de todos os cabeçalhos
         * para a posição anterior e decrementamos a quantidade de
         * arquivos do cabeçalho principal                                    */
        memcpy(*fheaders + pos, *fheaders + pos + 1, (--(*nfiles) - pos) * sizeof(struct fileheader));

        /* Retiramos da parte alocada da memória o bloco removido */
        if((*fheaders = (struct fileheader *) realloc(*fheaders, *nfiles * sizeof(struct fileheader))) == NULL) {
            fprintf(stderr, "Erro ao alocar memória!\n");
            return ERR_MEMALLOC;
        }
    }

    return 0;
}

/* Move um cabeçalho de uma posição para outra */
void moveHeader(struct fileheader *fheaders, int from, int to, unsigned int nfiles) {
    struct fileheader fromhdr;
    int h;

    /* Salvamos o cabeçalho a ser movido em outro local na memória */
    memcpy(&fromhdr, fheaders + from, sizeof(struct fileheader));

    /* Se a posição de origem for menor que a de destino, então
     * movemos os cabeçalhos entre os dois uma posição para trás,
     * caso contrário, movemos uma posição para frente                */
    if(from < to) {
        memcpy(fheaders + from, fheaders + from + 1, (to - from) * sizeof(struct fileheader));
    } else {
        ++to;
        for(h = from; h > to; --h) {
            memcpy(fheaders + h, fheaders + h - 1, sizeof(struct fileheader));
        }
    }

    /* Escrevemos o cabeçalho salvo na posição de destino */
    memcpy(fheaders + to, &fromhdr, sizeof(struct fileheader));
}
