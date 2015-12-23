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

#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>
//----
#include <archive.h>
#include <fileheaders.h>

/** @brief Escreve conteúdo
 *
 * Escreve o conteúdo do arquivo dentro do arquivador, atualizando também os cabeçalhos dentro do mesmo
 *
 *
 * @param arcp Apontador para o arquivo
 * @param fheaders Lista de Cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param fhdr Estrutura com os dados do cabeçalho do arquivo a ser adicionado
 * 
 * @return Se nenhum erro ocorrer retorna 0
 */
int addFileData(FILE *arcp, struct fileheader **fheaders, struct archeader *archdr, struct fileheader *fhdr);

/** @brief Remove conteúdo
 *
 * Remove o conteúdo de um arquivo do arquivador, atualizando também os cabeçalhos do mesmo
 *
 * @param arcp Apontador para o arquivo 
 * @param pos Posição do arquivo a ser removido
 * @param fheaders Lista de Cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * 
 * @return Se nenhum erro ocorrer retorna 0
 */
int deleteFileData(FILE *arcp, int pos, struct fileheader **fheaders, struct archeader *archdr);

/** @brief Move conteúdo
 *
 * Move o conteúdo de um arquivo dentro do arquivador para outra posição
 *
 * @param arcp Apontador para o arquivo 
 * @param fheaders Lista de Cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param from Posição atual do arquivo 
 * @param to Posição destino do arquivo 
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
int moveFileData(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr, int from, int to);

/** @brief Extrai membro
 *
 * Extrai o membro de uma determinada posição do arquivador
 *
 * @param arcp Apontador para o arquivo 
 * @param fheaders Lista de Cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 * @param pos Posição do arquivo 
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
int extractFileData(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr, int pos);

/** @brief Extrai todos os membros
 *
 * Extrai todos os membros que estão contidos no arquivador
 *
 * @param arcp Apontador para o arquivo 
 * @param fheaders Lista de Cabeçalhos
 * @param archdr Estrutura com os dados do cabeçalho do arquivador
 *
 * @return Se nenhum erro ocorrer retorna 0
 */
int extractAllFileData(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr);

#endif // __FILE_H__
