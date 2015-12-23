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

#ifndef __VINA_H__
#define __VINA_H__

/* Opções do programa */
#define VINA_OPTION_INSERT    1
#define VINA_OPTION_UPDATE    2
#define VINA_OPTION_EXTRACT   3
#define VINA_OPTION_MOVE    4
#define VINA_OPTION_REMOVE    5
#define VINA_OPTION_CONTENT   6

/* Códigos de erro do programa */
#define ERR_MEMALLOC      (-1) /* Stack overflow, Memory leak */
#define ERR_FOPEN         (-2) /* Erro ao abrir arquivo */
#define ERR_HDRLIST       (-3) /* Lista de cabeçalhos corrompida */
#define ERR_CHOWN         (-4) /* Erro ao mudar o dono do arquivo */
#define ERR_CHMOD         (-5) /* Erro ao mudar as permissões do arquivo */

/* Limites e configurações */
#define MAX_FILENAME      256 /* Tamanho máximo de nome de arquivo */
#define MAX_BUFFER        1024 /* Tamanho máximo do buffer para leitura/escrita */

/* Função mínimo */
#define MIN(a,b)        ((a) < (b) ? (a) : (b))

/**
 * \mainpage Arquivador Vina
 *
 * \section intro_sec Introdução
 *
 * Este programa é um arquivador de arquivos um programa que salva em sequência uma coleção de arquivos (denominados membros) em  um outro arquivo (denominado  archive) cuja estrutura permite   
 * recuperar os arquivos originais individualmente.
 * 
 *  \section aut_sec Autoria
 *
 *  Gabriel Moro Sabedotti Chemim e           Rafael Ravedutti Lucio Machado
 */

#endif // __VINA_H__
