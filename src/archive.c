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
#include <string.h>
#include <time.h>
//----
#include <archive.h>
#include <file.h>

/*
 * Abre um arquivador Vina, carrega os dados do cabeçalho
 * principal em archdr e os dados dos cabeçalhos dos membros
 * em fheaders
 */
FILE *openArchive(const char *archive, struct fileheader **fheaders, struct archeader *archdr, int *err) {
  FILE *arcp;

  *err = 0;
  *fheaders = NULL;
  archdr->nfiles = 0;

  /*
   * Caso o arquivador não abra em modo de leitura,
   * assumimos que o mesmo não existe e tentamos abrir
   * em modo de escrita para criá-lo, se falhar,
   * retornamos um código de erro.
   */

  if((arcp = fopen(archive, "rb+")) != NULL) {
    fread(archdr, sizeof(struct archeader), 1, arcp);

    /* Através dos primeiros bytes, verificamos se o arquivador é válido */
    if(strcmp(archdr->identifier, VINA_IDENTIFIER)) {
      fprintf(stderr, "%s não é um arquivador vina válido!\n", archive);
      fclose(arcp);
      return NULL;
    }

    /* Lê os cabeçalhos dos membros do arquivador */
    if((*err = readHeaders(arcp, fheaders, archdr->nfiles))) {
      fclose(arcp);
      return NULL;
    }
  } else if((arcp = fopen(archive, "wb+")) != NULL) {
    /* Como o arquivador acabou de ser criado, escrevemos o cabeçalho principal */
    memcpy(archdr->identifier, VINA_IDENTIFIER, sizeof(archdr->identifier));
    fwrite(archdr, sizeof(struct archeader), 1, arcp);
  } else {
    *err = ERR_FOPEN;
    fprintf(stderr, "Falha ao abrir o arquivador %s.\n", archive);
    return NULL;
  }

  return arcp;
}

/* Insere os membros de uma lista de membros em um arquivador */
int insertMembers(FILE *arcp, struct fileheader **fheaders, struct archeader *archdr, struct memberlist *memlist) {
  struct member *m;
  struct fileheader fhdr;
  int p, ret;

  /* Percorremos todos os membros da lista */
  for(m = memlist->base; m != NULL; m = m->next) {
    /* Buscamos o membro pela lista de cabeçalhos */
    p = findHeaderPos(m->filename, *fheaders, archdr->nfiles);

    /* Criamos o cabeçalho do membro e, se algum erro ocorrer
     * retornamos seu código e saímos              */
    if((ret = makeHeader(m->filename, &fhdr))) {
      fclose(arcp);
      return ret;
    }

    /* Se já existe um membro com o mesmo nome no arquivador,
     * então o substituimos, do contrário, apenas o adicionamos */
    if(p != -1) {
      ret = deleteFileData(arcp, p, fheaders, archdr);
    }

    /* Se nenhum erro ocorreu, adicionamos o arquivo no final */
    if(!ret) {
      ret = addFileData(arcp, fheaders, archdr, &fhdr);
    }
  }

  return ret;
}

/* Atualiza os membros de lista de membros no arquivador */
int updateMembers(FILE *arcp, struct fileheader **fheaders, struct archeader *archdr, struct memberlist *memlist) {
  struct member *m;
  struct fileheader fhdr;
  int p, ret;

  /* Percorremos todos os membros da lista */
  for(m = memlist->base; m != NULL; m = m->next) {
    /* Buscamos o membro pela lista de cabeçalhos */
    p = findHeaderPos(m->filename, *fheaders, archdr->nfiles);

    /* Criamos o cabeçalho do membro e, se algum erro ocorrer
     * retornamos seu código e saímos             */
    if((ret = makeHeader(m->filename, &fhdr))) {
      fclose(arcp);
      return ret;
    }

    /* O membro é mais atual do que o arquivo já contido? */
    if(p == -1 || fhdr.fmodtime >= fheaders[p]->fmodtime) {
      /* Se já existe um membro com o mesmo nome no arquivador,
         * então o substituimos, do contrário, apenas o adicionamos */
      if(p != -1) {
        ret = deleteFileData(arcp, p, fheaders, archdr);
      }

      /* Se nenhum erro ocorreu, adicionamos o arquivo no final */
      if(!ret) {
        ret = addFileData(arcp, fheaders, archdr, &fhdr);
      }
    }
  }

  return ret;
}

/* Move os membros de uma lista de membros no arquivador */
int moveMembers(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr, struct memberlist *memlist, const char *targ) {
  struct member *m;
  int from, to, ret;

  if((to = findHeaderPos(targ, fheaders, archdr->nfiles)) == -1) {
    printf("Não foi encontrado o arquivo \"%s\".", targ);
    return 0;
  }

  for(m = memlist->base; m != NULL; m = m->next) {
    from = findHeaderPos(m->filename, fheaders, archdr->nfiles);

    if(from != -1) {
      ret = moveFileData(arcp, fheaders, archdr, from, to);
    } else {
      printf("Não foi encontrado o arquivo \"%s\".", m->filename);
    }
  }

  return ret;
}

/* Extrai os membros de uma lista de membros de um arquivador */
int extractMembers(FILE *arcp, struct fileheader *fheaders, struct archeader *archdr, struct memberlist *memlist) {
  struct member *m;
  int p, ret;

  /* Se não forem especificados membros para serem extraidos, então
   * extraimos todos os arquivos do arquivador            */
  if(memlist == NULL || !memlist->members) {
    return extractAllFileData(arcp, fheaders, archdr);
  }

  /* Percorremos todos os membros da lista */
  for(m = memlist->base; m != NULL; m = m->next) {
    /* Buscamos o membro pela lista de cabeçalhos */
    p = findHeaderPos(m->filename, fheaders, archdr->nfiles);

    /* Se o arquivo existe no arquivador, então o extraimos,
     * senão informamos que ele não existe          */
    if(p != -1) {
      ret = extractFileData(arcp, fheaders, archdr, p);
    } else {
      printf("Não foi encontrado o arquivo \"%s\".", m->filename);
    }
  }

  return ret;
}

/* Remove os membros de uma lista de membros de um arquivador */
int deleteMembers(FILE *arcp, struct fileheader **fheaders, struct archeader *archdr, struct memberlist *memlist) {
  struct member *m;
  int p, ret;

  /* Percorremos todos os membros da lista */
  for(m = memlist->base; m != NULL; m = m->next) {
    /* Buscamos o membro pela lista de cabeçalhos */
    p = findHeaderPos(m->filename, *fheaders, archdr->nfiles);

    /* Se o arquivo existe no arquivador, então o deletamos,
     * senão informamos que ele não existe          */
    if(p != -1) {
      ret = deleteFileData(arcp, p, fheaders, archdr);
    } else {
      printf("Não foi encontrado o arquivo \"%s\".", m->filename);
    }
  }

  return ret;
}

/* Imprime os membros de um arquivador e seus dados */
int printMembers(struct fileheader *fheaders, struct archeader *archdr) {
  char modtime[32];
  struct fileheader *fhdr;
  struct tm *tptr;
  long long int size;
  unsigned int count;

  /* Legenda dos dados */
  printf("Perm   UID Tamanho   Data de Mod  Arquivo\n");

  /* Percorre todos os membros do arquivador */
  for(count = 0, fhdr = fheaders; count < archdr->nfiles; ++count, ++fhdr) {
    /* Imprime a permissão em octal e o UID em 5 casas decimais */
    printf("0%o %5d ", fhdr->fperm & 0777, fhdr->fuid);

    /* Convertemos o valor da estrutura do cabeçalho do arquivo de
     * off_t para long long int, pois assim podemos especificar o tipo
     * a ser imprimido posteriormente sem problemas            */
    size = (long long int) fhdr->fsize;

    /* Convertemos, caso seja mais conveniente, o tamanho do arquivo
     * para Kilobytes (KB), Megabytes (MB) ou Gigabytes (GB) e o
     * exibimos na unidade convertida em 5 casas decimais        */
    if(size >= 1024 * 1024 * 1024) {
      printf("%6lldG ", size / (1024 * 1024 * 1024));
    } else if(size >= 1024 * 1024) {
      printf("%6lldM ", size / (1024 * 1024));
    } else if(size >= 1024) {
      printf("%6lldK ", size / 1024);
    } else {
      printf("%7lld ", size);
    }

    /* Imprimimos a data de modificação no formato especificado em 12
     * casas de caracteres e em seguida o nome do arquivo        */
    tptr = localtime(&(fhdr->fmodtime));
    strftime(modtime, sizeof modtime, "%b %d %H:%M", tptr);
    printf("%13s  %s\n", modtime, fhdr->fname);
  }

  return 0;
}
