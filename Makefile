#
#   Copyright (C) 2013  Gabriel Moro Sabedotti Chemim
#                       Rafael Ravedutti Lucio Machado
#
#   This file is part of Vina
#
#   Vina is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   Vina is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with Vina.  If not, see <http://www.gnu.org/licenses/>.
#

# Compilador a ser usado
CC=gcc
# Localização dos arquivos de cabeçalho
INCDIR=src
# Localização dos arquivos fonte
SRCDIR=src
# Localização dos arquivos objeto
OBJDIR=src
# Nome do executável
EXEC=vina
# Opções do compilador
CFLAGS=-c -Wall -I$(INCDIR)
# Opções do linkador
LDFLAGS=

.PHONY : doc

all: $(EXEC)

$(EXEC): $(OBJDIR)/archive.o $(OBJDIR)/file.o $(OBJDIR)/fileheaders.o $(OBJDIR)/members.o $(OBJDIR)/parser.o $(OBJDIR)/vina.o
  @echo "> Linkando arquivos objetos..."
  $(CC) $? $(LDFLAGS) -o $(EXEC)

$(OBJDIR)/archive.o: $(SRCDIR)/archive.c
  @echo "> Compilando $<..."
  $(CC) $< $(CFLAGS) -o $(OBJDIR)/archive.o

$(OBJDIR)/file.o: $(SRCDIR)/file.c
  @echo "> Compilando $<..."
  $(CC) $< $(CFLAGS) -o $(OBJDIR)/file.o

$(OBJDIR)/fileheaders.o: $(SRCDIR)/fileheaders.c
  @echo "> Compilando $<..."
  $(CC) $< $(CFLAGS) -o $(OBJDIR)/fileheaders.o

$(OBJDIR)/members.o: $(SRCDIR)/members.c
  @echo "> Compilando $<..."
  $(CC) $< $(CFLAGS) -o $(OBJDIR)/members.o

$(OBJDIR)/parser.o: $(SRCDIR)/parser.c
  @echo "> Compilando $<..."
  $(CC) $< $(CFLAGS) -o $(OBJDIR)/parser.o

$(OBJDIR)/vina.o: $(SRCDIR)/vina.c
  @echo "> Compilando $<..."
  $(CC) $< $(CFLAGS) -o $(OBJDIR)/vina.o

doc:
  @echo "> Gerando documentação..."
  cd doc && doxygen Doxyfile
  @echo "> Documentação gerada em doc/html"

clean:
  @echo "> Removendo arquivos..."
  rm -f $(OBJDIR)/*.o $(EXEC)
