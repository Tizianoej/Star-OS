/*
 * Copyright (C) 2009 Niek Linnenbank, 2012 Felipe Cabrera, 2014 Dan Rulos
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <TerminalCodes.h>

int pf(int argc, char **argv, int i, char *u)
{
    dev_t dev;
    FILE *d;
    struct dirent *dent;
    struct stat st;
    char path[255], tmp[255];
    
    /* Obtenemos los argumentos, en caso de existir */
    if (argc > 1) {
        strncpy(path, u, sizeof(path));
        path[sizeof(path) - 1] = 0;
    }
    
    else
        getcwd(path, sizeof(path));

    /* Intentamos abrir el archivo. */
    if (!(d = fopen(path, "r"))) {
        printf("%s: error al abrir '%s': %s\r\n",
                argv[0], path, strerror(errno));
        return EXIT_FAILURE;
    }
    
    /* Leemos el archivo. */
    while ((dent = fread(d))) {
        if(dent->d_name[0]=='.')
            continue;
        /* Esta es la parte entretenida. A colorear! */
        switch (dent->d_type) {
            case DT_DIR:
                mkdir(argv[2], S_IWUSR | S_IRUSR);
                break;

            case DT_BLK:
            case DT_CHR:
                if (!strncmp(dent->d_name, "stdin", strlen("stdin")) ||
                    !strncmp(dent->d_name, "stdout", strlen("stdout")))
                    printf("%s", CYAN);
                else
                    mknod(argv[2], S_IWUSR | S_IRUSR, dev);
                break;
                
            case DT_REG:
            default:
                /* Construimos la ruta completa. */
                snprintf(tmp, sizeof(tmp),
                        "%s/%s", path, dent->d_name);

                /* Es un ejecutable? */
                if (stat(tmp, &st) != -1 && st.st_mode & 0100)
                    printf("%s", GREEN);
                else
                    touch(argv[2], S_IWUSR | S_IRUSR);
        }
        printf("%s ", dent->d_name);
    }
    
    printf("%s\r\n", WHITE);

    /* Cerramos. */
    fclose(d);
    
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if(argc < 3)
        printf("Uso: %s Entrada Salida");
    else {
        for(int i = 1; i < argc; i++) {
            printf("%s:\r\n",argv[i]);
            pf(argc, argv, i, argv[i]);
        }
    }
    
    /* Listo. */
    return EXIT_SUCCESS;
}
