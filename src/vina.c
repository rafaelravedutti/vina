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
#include <unistd.h>
//----
#include <vina.h>
#include <archive.h>
#include <fileheaders.h>
#include <members.h>
#include <parser.h>

/* Exibe instruções de uso do comando */
void displayHelp(const char *command) {
    printf( "Uso: %s <opção> <archive> [membro1 membro2 ...]\n\n"
            "Manipula arquivos do tipo VINA, que são arquivadores de estruturas simples\n"
            "e oferecem funções como inserção, remoção, extração, listagem e atualização\n"
            "de seus membros.\n\n"
            "Opções:\n"
            "\t-i\t\t Insere um membro no archive.\n"
            "\t-a\t\t Atualiza um membro no archive (substitui apenas se ele for o mais recente).\n"
            "\t-m targ\t\t Move o membro indicado para a posição após o membro targ que deve existir no arquivador.\n"
            "\t-x\t\t Extrai os membros indicados (se nenhum membro for indicado, todos serão extraidos).\n"
            "\t-r\t\t Remove os membros indicados.\n"
            "\t-c\t\t Lista o conteúdo do archive com as propriedades de cada membro.\n", command);
}

/*
 * Realiza uma operação indicada por vopt no arquivador de nome
 * archive, memlist é uma estrutura com os membros que farão
 * parte da operação, targ é usado apenas na opção de
 * mover um membro (VINA_OPTION_MOVE)
 */
int do_vina_opt(const char *archive, unsigned char vopt, struct memberlist *memlist, const char *targ) {
    FILE *arcp;
    struct archeader archdr;
    struct fileheader *fheaders;
    int ret;

    if((arcp = openArchive(archive, &fheaders, &archdr, &ret)) == NULL) {
        return ret;
    }

    switch(vopt) {
        case VINA_OPTION_INSERT:
            ret = insertMembers(arcp, &fheaders, &archdr, memlist);
            break;
        case VINA_OPTION_UPDATE:
            ret = updateMembers(arcp, &fheaders, &archdr, memlist);
            break;
        case VINA_OPTION_MOVE:
            ret = moveMembers(arcp, fheaders, &archdr, memlist, targ);
            break;
        case VINA_OPTION_EXTRACT:
            ret = extractMembers(arcp, fheaders, &archdr, memlist);
            break;
        case VINA_OPTION_REMOVE:
            ret = deleteMembers(arcp, &fheaders, &archdr, memlist);
            break;
        case VINA_OPTION_CONTENT:
            ret = printMembers(fheaders, &archdr);
    }

    fclose(arcp);
    return ret;
}

/* Função principal */
int main(int argc, char *argv[]) {
    char *archive = NULL;
    char *targ = NULL;
    int ret;
    char opt;
    unsigned char vopt;
    struct memberlist *memlist;

    /*
     * Aqui verificamos qual foi a opção passada na linha de comando.
     * Como a opção -m possui o parâmetro targ, colocamos ':' após
     * o caractere 'm' na lista de parâmetros.
     */

    while((opt = getopt(argc, argv, "iaxm:rch")) != -1) {
        switch(opt) {
            case 'h':
                displayHelp(argv[0]);
                return 0;

            case 'i':
                vopt = VINA_OPTION_INSERT;
                break;

            case 'a':
                vopt = VINA_OPTION_UPDATE;
                break;

            case 'x':
                vopt = VINA_OPTION_EXTRACT;
                break;

            case 'm':
                vopt = VINA_OPTION_MOVE;

                /* Aqui duplicamos a string optarg no ponteiro targ
                 * para evitar conflitos nessa posição de memória */

                if((targ = strdup(optarg)) == NULL) {
                    /* Caso não seja possível alocar targ, retornamos um código de erro */
                    fprintf(stderr, "Falha ao alocar memória!\n");
                    return ERR_MEMALLOC;
                }

                break;

            case 'r':
                vopt = VINA_OPTION_REMOVE;
                break;

            case 'c':
                vopt = VINA_OPTION_CONTENT;
                break;

            case '?':
                displayHelp(argv[0]);
                break;
        }
    }

    /* Alocamos uma lista de membros vazia */
    memlist = allocMemberList();

    /* A lista de membros foi alocada com sucesso? */
    if(!memlist) {
        fprintf(stderr, "Erro ao alocar lista de membros\n");
        return ERR_MEMALLOC;
    }

    /*
     * Se nenhum erro ocorreu ao processar os argumentos
     * continuamos a execução do programa com a operação
     * indicada, do contrário não continuamos e atribuimos
     * a ret o código do erro para ser retornado posteriormente
     */

    if(!(ret = parse_vina_args(argc, argv, &archive, memlist))) {
        ret = do_vina_opt(archive, vopt, memlist, targ);
    }

    /* Aqui liberamos a memória usada por targ, se foi alocada. */
    if(targ != NULL) {
        free(targ);
    }

    /* Aqui liberamos a memória usada por archive, se foi alocada */
    if(archive != NULL) {
        free(archive);
    }

    /* Liberamos a memória ocupada pela lista de membros */
    freeMemberList(memlist);

    return ret;
}
