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

#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#include <members.h>
#include <fileheaders.h>

/* Identificador de arquivadores VINA */
#define VINA_IDENTIFIER         "VINA\0"

/* Tamanho da string definida em VINA_IDENTIFIER */
#define VINA_IDENTIFIER_SIZE    5

/* Estrutura com os dados do cabeçalho do arquivador */
struct archeader {
    char identifier[VINA_IDENTIFIER_SIZE];    /* Identificador VINA */
    unsigned int nfiles;                    /* Quantidade de arquivos */
};

/** @brief Abre um arquivador Vina
 *
 * Abre um arquivador Vina, carrega os dados do cabeçalho
 * principal em archdr e os dados dos cabeçalhos dos membros
 * em fheaders
 *
 * @param archive Nome do arquivo Vina a ser aberto
 * @param fheaders Lista de cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param err Código de erro
 * 
 * @return Se não houver erros retorna um ponteiro para um arquivador Vina, caso contrário retorna NULL
 */
FILE *openArchive(const char *archive, struct fileheader **fheaders, struct archeader *archdr, int *err);

/** @brief Insere membros
 *
 * Função que insere os membros de uma lista de membros em um arquivador, se o membro já existe no arquivador, ele é substituído
 *
 * @param arcp Ponteiro para um arquivador Vina
 * @param fheaders Lista de cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param memlist Lista de membros a serem adicionados
 *
 * @return Se nenhum erro ocorrer retornamos 0
 */
int insertMembers(FILE *arcp, struct fileheader **fheaders, struct archeader *archdr, struct memberlist *memlist);

/** @brief Atualiza membros
 *
 * Função que atualiza os membros de uma lista de membros em um arquivador, a substituição só ocorre se o arquivo é mais recente que o encontrado no arquivador
 *
 * @param arcp Ponteiro para um arquivador Vina
 * @param fheaders Lista de cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param memlist Lista de membros a serem atualizados
 *
 * @return Se nenhum erro ocorrer retornamos 0
 */
int updateMembers(FILE *arcp, struct fileheader **fheaders, struct archeader *archdr, struct memberlist *memlist);

/** @brief Move membros
 *
 * Função que move os membros de uma lista de membros para a posição após o parâmetro targ
 *
 * @param arcp Ponteiro para um arquivador Vina
 * @param fheaders Lista de cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param memlist Lista de membros a serem atualizados
 * @param targ Nome do arquivo o qual os arquivos serão movidos para baixo dele
 *
 * @return Se nenhum erro ocorrer retornamos 0
 */
int moveMembers(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr, struct memberlist *memlist, const char *targ);

/** @brief Extrai membros
 *
 * Função que extrai os membros de uma lista de membros em um arquivador, caso os membros não forem especificados, todos os arquivos serão extraidos
 *
 * @param arcp Ponteiro para um arquivador Vina
 * @param fheaders Lista de cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param Lista de membros a serem atualizados
 *
 * @return Se nenhum erro ocorrer retornamos 0
 */
int extractMembers(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr, struct memberlist *memlist);

/** @brief Remove membros
 *
 * Função que remove os membros de uma lista de membros de um arquivador, caso o arquivo não exista, uma mensagem de erro é exibida
 *
 * @param arcp Ponteiro para um arquivador Vina
 * @param fheaders Lista de cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param memlist Lista de membros a serem removidos
 *
 * @return Se nenhum erro ocorrer retornamos 0
 */
int deleteMembers(FILE *arcp, struct fileheader **fheaders, struct archeader *archdr, struct memberlist *memlist);

/** @brief Imprime membros
 *
 * Função que imprime os dados de todos os membros encontrados no arquivador. Esta função irá imprimir a permissão em octal, o tamanho do arquivo, a data de modificação, a UID e o nome do arquivo
 *
 * @param fheaders Lista de cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 *
 * @return Se nenhum erro ocorrer retornamos 0
 */
int printMembers(struct fileheader *fheaders, struct archeader *archdr);

#endif // __ARCHIVE_H__
