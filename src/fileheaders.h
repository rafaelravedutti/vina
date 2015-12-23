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

#ifndef __FILEHEADERS_H__
#define __FILEHEADERS_H__

#include <stdio.h>
//----
#include <vina.h>

/* Lista com os dados do cabeçalho de cada arquivo */
struct fileheader {
  char fname[MAX_FILENAME];
  time_t fmodtime;
  mode_t fperm;
  uid_t fuid;
  gid_t fgid;
  off_t fsize;
};

/** @brief Gera o cabeçalho de um arquivo 
 * 
 * A função gera o cabeçalho que contém o nome do arquivo, a data de modificação, a UID, a GID, o tamanho e as permissões 
 *
 * @param filename Nome do arquivo cujo cabeçalho será gerado
 * @param fhdr Estrutura que irá conter o cabeçalho
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
int makeHeader(const char *filename, struct fileheader *fhdr);

/** @brief Altera o cabeçalho de um arquivo 
 *
 * A função altera a UID e as permissões do cabeçalho de um arquivo
 *
 * @param filename Nome do arquivo cujo cabeçalho será alterado
 * @param fhdr Estrutura que contém o cabeçalho do arquivo
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
int changeHeader(const char *filename, struct fileheader *fhdr);

/** @brief Encontra o cabeçalho de um membro
 *
 * Encontra os dados do cabeçalho de um membro em um arquivador a partir do seu nome
 *
 * @param filename Nome do arquivo procurado
 * @param fheaders Estrutura que contém o cabeçalho do arquivo procurado
 * @param nfiles Número de arquivos no arquivador Vina
 *
 * @return Retorna a posição que o arquivo se encontra dentro do arquivador, se o membro não existir é retornado o valor -1
 */
int findHeaderPos(const char *filename, struct fileheader *fheaders, unsigned int nfiles);

/** @brief Lê os cabeçalhos contidos no arquivador
 *
 * Lê os cabeçalhos dos membros de um arquivador e os armazena em uma lista de cabeçalhos, esta função irá mover o apontador do arquivo para o final
 *
 * @param arcp Apontador para um arquivador Vina
 * @param fheaders Lista de cabeçalhos
 * @param nfiles Número de arquivos contidos no arquivador
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
int readHeaders(FILE *arcp, struct fileheader **fheaders, unsigned int nfiles);

/** @brief Lê os cabeçalhos contidos no arquivador
 *
 * Lê os cabeçalhos dos membros de um arquivador e os armazena em uma lista de cabeçalhos, esta função irá mover o apontador do arquivo para o final
 *
 * @param arcp Apontador para um arquivador Vina
 * @param fheaders Lista de cabeçalhos
 * @param nfiles Número de arquivos contidos no arquivador
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
void writeHeaders(FILE *arcp, struct fileheader *fheaders, unsigned int nfiles);

/** @brief Adiciona um cabeçalho para a lista 
 *
 * Adiciona um cabeçalho para a lista 
 *
 * @param fheaders Lista de cabeçalhos
 * @param fhdr Estrutura que contém o cabeçalho do arquivo
 * @param nfiles Número de arquivos contidos no arquivador
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
int addHeader(struct fileheader **fheaders, struct fileheader *fhdr, unsigned int *nfiles);

/** @brief Deleta um cabeçalho de uma lista
 *
 * Deleta um cabeçalho de uma lista de cabeçalhos atraves de sua posição
 *
 * @param fheaders Lista de cabeçalhos
 * @param pos Posição do cabeçalho a ser removido
 * @param nfiles Número de arquivos contidos no arquivador
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
int deleteHeader(struct fileheader **fheaders, int pos, unsigned int *nfiles);

/** @brief Move um cabeçalho de uma posição para outra
 *
 * Move um cabeçalho de uma posição até outra posição passada como parâmetro
 *
 * @param fheaders Lista de cabeçalhos
 * @param from Posição inicial do cabeçalho
 * @param to Posição destino do cabeçalho
 * @param nfiles Número de arquivos contidos no arquivador
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
void moveHeader(struct fileheader *fheaders, int from, int to, unsigned int nfiles);

#endif // __FILEHEADERS_H__
