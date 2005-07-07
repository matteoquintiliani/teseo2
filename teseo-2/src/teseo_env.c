/* Teseo-2 Plug-in
 * Copyright (C) 2005  Stefano Pintore, Matteo Quintiliani (the "Authors").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE Authors BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Authors of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Authors.
 */
 
#include <stdio.h>
#include <string.h>
#include "teseo_env.h"
#include <libgimp/gimp.h>

#define LEN_RIGA 1024


const char TESEO_BIN[] = "TESEO_BIN";
const char TESEO_DATA[] = "TESEO_DATA";


char *valore_parse(const char *nomefile, const char *nomevariabile) {
  static char ret_valore[255];
  char riga[LEN_RIGA];
  char *token;
  const char *delim = " =\t\n";
  FILE *f;
  int trovato = 0;
  ret_valore[0] = 0;
  // printf("\n");
  f = fopen(nomefile, "rt");
  if(f) {
    while(fgets(riga, LEN_RIGA-1, f) != NULL  &&  !trovato) {
      token = strtok(riga, delim);
      // printf("primo token : %s\n", token);
      if(token && strcmp(token, "#") != 0) {
        if(token && strcmp(token, nomevariabile) == 0) {
          token = strtok(NULL, delim);
          // printf("succ. token : %s\n", token);
          trovato = 1;
          if(token) {
            strcpy(ret_valore, token);
          }
        }
      }
    }
    fclose(f);
  } else {
    printf("\nNon riesco ad aprire \"%s\" !\n", nomefile);
  }
  return (ret_valore[0] == 0)? NULL : ret_valore;
}


char *getenv_teseo(const char *name_var) {
  char s_app[200];
  char *s;
  char *cmdconfig = "/usr/local/neuronteseo/bin/neuronteseo-config.sh";

  char nomefile[255];
  sprintf(nomefile, "%s/.teseorc", getenv("HOME"));
  s = valore_parse(nomefile, name_var);
  // g_message("newfunc %s = %s", name_var, s);

  // s = getenv(name_var);
  if(!s) {
    if(strcmp(name_var , TESEO_BIN)==0) {
      sprintf(s_app, "%s/teseo", getenv("HOME"));
    } else {
      sprintf(s_app, "%s", getenv("HOME"));
    }
    s = s_app;
    g_message("E' necessario impostare la variabile d'ambiente \"%s\" !\n\n- Se hai installato il pacchetto RPM, chiudi GIMP ed esegui lo script \"%s\".\n- Se invece hai compilato i sorgenti, hai dimenticato di eseguire \"make install\"\n\nLa variabile per ora viene impostata a \"%s\"", name_var, cmdconfig, s);
  } else {
    // g_message("Variabile d'ambiente %s = %s", name_var, s);
  }
  return s;
}