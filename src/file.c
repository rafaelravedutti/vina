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
#include <stdlib.h>
#include <unistd.h>
//----
#include <vina.h>
#include <file.h>

/*
 * Escreve o conteúdo do arquivo dentro do arquivador,
 * atualizando também os cabeçalhos dentro do mesmo
 */
int addFileData(FILE *arcp, struct fileheader **fheaders, struct archeader *archdr, struct fileheader *fhdr) {
  FILE *fp;
  char buffer[MAX_BUFFER];
  off_t blksize;
  int ret;
  unsigned int min;

  /* Posicionamos o apontador do arquivo para o final da parte
   * de conteúdos, onde iremos concatenar o conteúdo do nosso arquivo */
  fseek(arcp, -(archdr->nfiles * sizeof(struct fileheader)), SEEK_END);

  /* Se houver falha em abrir ao arquivo, retornamos um código de erro */
  if((fp = fopen(fhdr->fname, "rb")) == NULL) {
    fprintf(stderr, "Falha ao abrir o arquivo \"%s\".\n", fhdr->fname);
    return ERR_FOPEN;
  }

  /* Adicionamos o cabeçalho para a lista em memória */
  if((ret = addHeader(fheaders, fhdr, &(archdr->nfiles)))) {
    fclose(fp);
    return ret;
  }

  /* Transferimos em disco o conteúdo do arquivo para o arquivador
   * em blocos de tamanho MAX_BUFFER                 */
  for(blksize = fhdr->fsize; blksize > 0; blksize -= MAX_BUFFER) {
    min = MIN(blksize, MAX_BUFFER);
    fread(buffer, min, sizeof(char), fp);
    fwrite(buffer, min, sizeof(char), arcp);
  }

  /* Escrevemos a lista de cabeçalhos atualizada */
  fwrite(*fheaders, archdr->nfiles, sizeof(struct fileheader), arcp);

  /* Contudo, atualizamos o cabeçalho principal do arquivador */
  fseek(arcp, 0, SEEK_SET);
  fwrite(archdr, sizeof(struct archeader), 1, arcp);

  fclose(fp);
  return 0;
}

/*
 * Remove o conteúdo de um arquivo do arquivador, atualizando
 * também os cabeçalhos do mesmo
 */
int deleteFileData(FILE *arcp, int pos, struct fileheader **fheaders, struct archeader *archdr) {
  char buffer[MAX_BUFFER];
  struct fileheader *fhdr;
  off_t blksize, size, offset, arcsize;
  unsigned int h, min;

  /* A posição é válida dentro do arquivador? */
  if(pos < 0 || pos >= archdr->nfiles) {
    return 0;
  }

  /*
   * Calculamos o deslocamento em bytes até o conteúdo do
   * arquivo que iremos remover
   */
  offset = sizeof(struct archeader);
  for(fhdr = *fheaders, h = 0; h < pos; ++fhdr, ++h) {
    offset += fhdr->fsize;
  }

  /* Tamanho do conteúdo a ser removido */
  size = (*fheaders + pos)->fsize;

  /* Tamanho do arquivador sem os cabeçalhos dos membros */
  fseek(arcp, 0, SEEK_END);
  arcsize = ftell(arcp) - (archdr->nfiles * sizeof(struct fileheader));


  /* Se não há nada para sobrescrever no arquivo, apenas colocamos seu
   * apontador onde serão escritos os cabeçalhos dos membros      */
  if(arcsize <= offset + size) {
    fseek(arcp, offset, SEEK_SET);
  }

  /*
   * Vamos sobrescrevendo o conteúdo que vai ser removido com o
   * que vem após ele em blocos de tamanho MAX_BUFFER
   */
  for(blksize = arcsize - (offset + size); blksize > 0; blksize -= MAX_BUFFER) {
    min = MIN(blksize, MAX_BUFFER);

    /* Posicionamos o apontador na parte após o conteúdo e
     * então lêmos o arquivo                 */
    fseek(arcp, offset + size, SEEK_SET);
    fread(buffer, min, sizeof(char), arcp);

    /* Voltamos o apontador para o conteúdo e o sobrescrevemos
     * com o que foi lido                       */
    fseek(arcp, offset, SEEK_SET);
    fwrite(buffer, min, sizeof(char), arcp);

    offset += min;
  }

  /* Deletamos o cabeçalho do arquivo em memória e escrevemos
   * a lista atualizada no arquivador               */
  deleteHeader(fheaders, pos, &(archdr->nfiles));
  fwrite(*fheaders, archdr->nfiles, sizeof(struct fileheader), arcp);

  /* Contudo, atualizamos o cabeçalho principal do arquivador */
  fseek(arcp, 0, SEEK_SET);
  fwrite(archdr, sizeof(struct archeader), 1, arcp);

  /* Truncamos o arquivo, para retirar o excesso de dados */
  ftruncate(fileno(arcp), offset + (archdr->nfiles * sizeof(struct fileheader)));
  return 0;
}

/* Move o conteúdo de um arquivo dentro do arquivador para outra posição */
int moveFileData(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr, int from, int to) {
  char buffer[MAX_BUFFER];
  struct fileheader *fhdr;
  off_t offset_from, offset_to, blksize, mvpos;
  unsigned int h, min, tsize;
  
  /* As posições são válidas dentro do arquivador? */
  if(from < 0 || from >= archdr->nfiles || to < 0 || to >= archdr->nfiles) {
    return 0;
  }

  /* As posições do movimento são diferentes? */
  if(from == to || from == to + 1) {
    return 0;
  }

  /* Calculamos os deslocamentos no arquivador de onde
   * o conteúdo está e para onde ele será movido    */
  offset_from = offset_to = sizeof(struct archeader);
  for(fhdr = fheaders, h = 0; h <= to || h < from; ++fhdr, ++h) {
    if(h <= to) {
      offset_to += fhdr->fsize;
    }

    if(h < from) {
      offset_from += fhdr->fsize;
    }
  }

  /* Segmentamos o conteúdo a ser movido em blocos de tamanho MAX_BUFFER / 2 bytes */
  for(blksize = (fheaders + from)->fsize; blksize > 0; blksize -= MAX_BUFFER / 2) {
    min = MIN(blksize, MAX_BUFFER / 2);

    /*
     * Aqui separamos em dois casos, se a posição de origem for menor que a
     * posição de destino e se a posição de origem for maior que a posição
     * de destino
     *
     * Para mover o conteúdo, vamos percorrendo o arquivo da posição de origem
     * até a posição de destino em blocos e, segmentando o buffer em duas partes,
     * conseguimos inverter a posição destes blocos, até transferirmos o conteúdo
     * para a posição de destino
     */

    if(from < to) {
      for(mvpos = offset_from + blksize - min; mvpos < offset_to - min; mvpos += tsize) {
        tsize = MIN(offset_to - (mvpos + min), MAX_BUFFER - min);
        fseek(arcp, mvpos, SEEK_SET);
        fread(buffer, min + tsize, sizeof(char), arcp);

        fseek(arcp, mvpos, SEEK_SET);
        fwrite(buffer + min, tsize, sizeof(char), arcp);
        fwrite(buffer, min, sizeof(char), arcp);
      }

      offset_to -= min;
    } else {
      for(mvpos = offset_from + ((fheaders + from)->fsize - blksize) + min; mvpos > offset_to + min; mvpos -= tsize) {
        tsize = MIN((mvpos - min) - offset_to, MAX_BUFFER - min);
        fseek(arcp, mvpos - (tsize + min), SEEK_SET);
        fread(buffer, tsize + min, sizeof(char), arcp);

        fseek(arcp, mvpos - (tsize + min), SEEK_SET);
        fwrite(buffer + tsize, min, sizeof(char), arcp);
        fwrite(buffer, tsize, sizeof(char), arcp);
      }

      offset_to += min;
    }
  }

  /* Movemos os cabeçalhos e atualizamos o conteúdo no arquivador */
  moveHeader(fheaders, from, to, archdr->nfiles);
  writeHeaders(arcp, fheaders, archdr->nfiles);
  return 0;
}

/* Extrai o membro de uma determinada posição do arquivador */
int extractFileData(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr, int pos) {
  FILE *fp;
  char buffer[MAX_BUFFER];
  struct fileheader *fhdr;
  off_t blksize, offset;
  int ret;
  unsigned int h, min;

  /* A posição é válida dentro do arquivador? */
  if(pos < 0 || pos >= archdr->nfiles) {
    return 0;
  }

  /*
   * Calculamos o deslocamento em bytes até o conteúdo do
   * arquivo que iremos remover
   */
  offset = sizeof(struct archeader);
  for(fhdr = fheaders, h = 0; h < pos; ++fhdr, ++h) {
    offset += fhdr->fsize;
  }

  /* Tentamos abrir o arquivo para escrever o conteúdo e
   * caso não consigamos, exibimos uma mensagem de erro    */
  if((fp = fopen(fhdr->fname, "wb")) == NULL) {
    fprintf(stderr, "Falha ao abrir o arquivo \"%s\".", fhdr->fname);
    return ERR_FOPEN;
  }

  /* Posicionamos o apontador do arquivo para o inicio do conteúdo */
  fseek(arcp, offset, SEEK_SET);

  /* Vamos transferindo o conteúdo do arquivador para o arquivo
   * em blocos de tamanho MAX_BUFFER                */
  for(blksize = fhdr->fsize; blksize > 0; blksize -= MAX_BUFFER) {
    min = MIN(blksize, MAX_BUFFER);
    fread(buffer, min, sizeof(char), arcp);
    fwrite(buffer, min, sizeof(char), fp);
  }

  /* Muda os atributos do arquivo extraido */
  if((ret = changeHeader(fhdr->fname, fhdr))) {
    return ret;
  }

  fclose(fp);
  return 0;
}

/* Extrai todos os membros de um arquivador */
int extractAllFileData(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr) {
  FILE *fp;
  char buffer[MAX_BUFFER];
  struct fileheader *fhdr;
  off_t blksize;
  int ret;
  unsigned int h, min;

  /* Posicionamos o apontador do arquivo na parte de conteúdos */
  fseek(arcp, sizeof(struct archeader), SEEK_SET);

  /* Percorremos todos os cabeçalhos dos membros do arquivador */
  for(fhdr = fheaders, h = 0; h < archdr->nfiles; ++fhdr, ++h) {
    /* Tentamos abrir o arquivo para escrever o conteúdo e
     * caso não consigamos, exibimos uma mensagem de erro    */
    if((fp = fopen(fhdr->fname, "wb")) == NULL) {
      fprintf(stderr, "Falha ao abrir o arquivo \"%s\".", fhdr->fname);
      return ERR_FOPEN;
    }

    /* Vamos transferindo o conteúdo do arquivador para o arquivo
     * em blocos de tamanho MAX_BUFFER                */
    for(blksize = fhdr->fsize; blksize > 0; blksize -= MAX_BUFFER) {
      min = MIN(blksize, MAX_BUFFER);
      fread(buffer, min, sizeof(char), arcp);
      fwrite(buffer, min, sizeof(char), fp);
    }

    /* Muda os atributos do arquivo extraido */
    if((ret = changeHeader(fhdr->fname, fhdr))) {
      return ret;
    }

    fclose(fp);
  }

  return 0;
}
