#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int getmagic(FILE *input_file, int magic)
{
    if (!input_file || ferror(input_file))
        return -2;

    int c = getc(input_file);
    if (c == 'P'){
        switch (getc(input_file)) {
            case '1': return 1;
            case '2': return 2;
            case '3': return 3;
            case '4': return 4;
            case '5': return 5;
            case '6': return 6;
            default: return -1;
        }
    } else{
        printf("Not a valid netpbm image\n"); return -1;}
}

void worker(char *input, char *output, char** *type) {

    //get the type of image to create from extension from *output
    const char dot = '.';
    char *extension;
    int magic;
    extension = strrchr(output, dot);

    //load the input file
    FILE* input_file = fopen(input , "r+");

    //empty file check
    if (input_file == NULL) {
        perror("\nStrange file");
        return -1;
    } else if (strcmp(type, "B") == 1){
        printf("\nrequested a %s binary\n", extension);
    } else if (strcmp(type, "A") == 1){
        printf("\nrequested a %s ascii file\n", extension);
    }

    //get the magic Px value, run the correct functions to convert with
    magic = getmagic(input_file, magic);
    printf("input image is P%d\n", magic);

    //if the input file is binary, converts and works with .temp file
    bin_to_ascii(input_file, input, magic);

    if (magic == 1 || magic == 4){
        if (strcmp(extension, ".pgm") == 0){
            pbm_to_pgm(input_file,output,magic,type);
        } else if (strcmp(extension, ".ppm") == 0){
            pbm_to_ppm(input_file,output,magic, type);
        }
    } else if (magic == 2 || magic == 5){
        if (strcmp(extension, ".pbm") == 0){
            pgm_to_pbm(input_file,output,magic,type);
        } else if (strcmp(extension, ".ppm") == 0){
            pgm_to_ppm(input_file,output,magic,type);
        }
    } else if (magic == 3 || magic == 6){
        if (strcmp(extension, ".pgm") == 0){
            ppm_to_pgm(input_file,output,magic, type);
        } else if (strcmp(extension, ".pbm") == 0){
            ppm_to_pbm(input_file,output,magic);
        }
    }

    fclose(input_file);

}

void bin_to_ascii(FILE *input_file, char *input, int magic, char** *type){

    // 4 -> 1
    if (magic == 5 || magic == 6){
        printf("\ninput is binary file!\n");
        char buffer[256];
        char junk[8];
        char byte[8];
        int num = 10;
        unsigned char val;
        int counter;
        int width = 6;
        fclose(input_file);
        FILE* input_file = fopen(input , "rb");
        FILE* temp_file = fopen(".temp", "wé+");
        fread(&junk, 3, 1, input_file);
        if (magic == 5)
        {
            num = 11;
        }
        for(int i=0;i<=num;i++){
            fread(&val, 1, 1, input_file);
            fwrite(&val, sizeof(val), 1, temp_file);
        }

        fprintf(temp_file, "\n");

        while(fread(&val, 1, 1, input_file) == 1){
            fprintf(temp_file, "%d ", val);
            if (counter >= 70){
               fprintf(temp_file, "\n");
               counter = 0;
            }
            counter ++;
        }
        fclose(temp_file);
        fclose(input_file);
        input_file = fopen(".temp" , "r+");
    }

    if (magic == 4){
        printf("cannot convert pbm binaries");
        return -1;
    }

    return 0;
}

void pbm_to_pgm(FILE *input_file, char *output,int magic, char *type){
    short int white = 255;
    int counter = 0;
    int width = 0;
    FILE* pgm_file;

    if (strcmp(type, "B") == 1){
        pgm_file = fopen(output, "wb");
        fprintf(pgm_file, "P5\n");
    } else if (strcmp(type, "A") == 1){
        pgm_file = fopen(output, "w+");
        fprintf(pgm_file, "P2\n");
    }
    if (pgm_file == NULL) {
        perror("Strange file");
        return -1;
    }

    char width_height_temp[255];

    fscanf(input_file, "%s", width_height_temp);
    fprintf(pgm_file, "%s", width_height_temp);

    fscanf(input_file, "%d", &width);
    fprintf(pgm_file, " %d\n", width);
    fprintf(pgm_file, "%d\n", white);

//BIN
    if (strcmp(type, "B") == 1){
        printf("writing bin");
        char temp[256];
        unsigned char white1 = 255;
        unsigned char black1 = 0;

        while(fscanf(input_file,"%s", temp) == 1){
            if(temp[0]=='0'){
                  fwrite(&white1, sizeof(white1), 1, pgm_file);
            }
            //fprintf(pgm_file,"%d ", white);
            if(temp[0]=='1'){
                fwrite(&black1, sizeof(black1), 1, pgm_file);
            }

        }
    } else
//ASCII
    if (strcmp(type, "A") == 1){
        char temp[256];
        printf("writin ascii");
        while(fscanf(input_file,"%s", temp) == 1){
            if(temp[0]=='0')
                fprintf(pgm_file,"%d ", white);
            if(temp[0]=='1')
                fprintf(pgm_file,"%d ", 0);
            if (counter >= width){
                fprintf(pgm_file,"\n");
                counter = 1;
            }
            counter++;
        }
    }

    fclose(pgm_file);
}

void pgm_to_pbm(FILE *input_file, char *output,int magic, char *type){
    //tento prevod je samo sebou ponekud nepekny, rozhodl jsem se pro reseni: tmavsi polovina je cerna a svetlejsi polovina je bila

    int white,width;
    FILE* pbm_file;

    if (strcmp(type, "B") == 1){
        pbm_file = fopen(output, "wb");
        fprintf(pbm_file, "P3\n");
    } else if (strcmp(type, "A") == 1){
        pbm_file = fopen(output, "w+");
        fprintf(pbm_file, "P1\n");
    }

    char width_height_temp[255];
    fscanf(input_file, "%d", &width);
    fprintf(pbm_file, "%d", width);
    fscanf(input_file, "%s", width_height_temp);

    fprintf(pbm_file, " %s\n", width_height_temp);
    fscanf(input_file, "%d", &white);
//BIN
    if (strcmp(type, "B") == 1){
        printf("not finished");
        unsigned char white1 = 1;
        unsigned char black1 = 0;
        unsigned char temp;
        int maxwidth;
        char arr[8];
        int whichbit = 8;
        if (width > 8)
            whichbit = 16;
        //some file line limits...
        if (width > 16)
            for (int maxwidth = 24;maxwidth >=1024;maxwidth+8){
                if (width > maxwidth)
                continue;
                else whichbit = maxwidth;
            }
        maxwidth = whichbit;

        while(fscanf(input_file,"%s", &temp) == 1){
        for (int i = 0;i<= 7;i++){
        if (whichbit < 8){
            arr[i] |= 0 << (i%8);
            break;
        }
        arr[i] |= 1 << (i%8);
        whichbit--;
        }
        fwrite(&arr, sizeof(arr), 1, pbm_file);
        }
}
//ASCII
    else if (strcmp(type, "A") == 1){
        int temp;
        int i,counter = 0;
        while(fscanf(input_file,"%d", &temp) == 1){
            if(temp >= (white / 2)){
                fprintf(pbm_file,"%d ", 0);
                counter++;
            }
            if(temp < (white / 2)){
                fprintf(pbm_file,"%d ", 1);
                counter++;
            }
            if(counter >= 255){
                fprintf(pbm_file, "\n");
                counter = 0;
            }
        }
    }
    fclose(pbm_file);
}

void pgm_to_ppm(FILE *input_file, char *output,int magic,char *type){
    short int white = 0;
    int width;
    char width_height_temp[255];
    FILE* pgm_file;

    if (strcmp(type, "B") == 1){
        pgm_file = fopen(output, "wb");
        fprintf(pgm_file, "P6\n");
    } else if (strcmp(type, "A") == 1){
            pgm_file = fopen(output, "w+");
            fprintf(pgm_file, "P3\n");
      }

    fscanf(input_file, "%s", width_height_temp);
    fprintf(pgm_file, "%s", width_height_temp);
    fscanf(input_file, "%d", &width);
    fprintf(pgm_file, " %d\n", width);
    fscanf(input_file, "%d", &white);
    fprintf(pgm_file, "%d\n", white);

//BIN
    if (strcmp(type, "B") == 1){
        printf("writing binary");
        unsigned char val;

        while(fscanf(input_file,"%d", &val) == 1){
            val = val^1;
            fwrite(&val, sizeof(val), 1, pgm_file);
            fwrite(&val, sizeof(val), 1, pgm_file);
            fwrite(&val, sizeof(val), 1, pgm_file);
        }
    }

//ASCII
    else if (strcmp(type, "A") == 1){
        printf("writing ascii");

        int pixel;
        int avg, counter = 0;
        while(fscanf(input_file,"%d", &pixel) == 1){
            fprintf(pgm_file, "%d %d %d ", pixel, pixel, pixel);

            if (counter >= width){
                fprintf(pgm_file,"\n");
                counter = 1;
            }
            counter++;
        }
    }
    fclose(pgm_file);
}

void ppm_to_pgm(FILE *input_file, char *output,int magic, char *type){

    char width_height_temp[255];
    short int white = 255;
    int R,G,B, width, avg, counter = 0;

    FILE* pgm_file;

    if (strcmp(type, "B") == 1){
        pgm_file = fopen(output, "wb");
        fprintf(pgm_file, "P5\n");
    } else if (strcmp(type, "A") == 1){
        pgm_file = fopen(output, "w+");
        fprintf(pgm_file, "P2\n");
      }

    fscanf(input_file, "%s", width_height_temp);
    fprintf(pgm_file, "%s", width_height_temp);
    fscanf(input_file, "%s", width_height_temp);
    fprintf(pgm_file, " %s\n", width_height_temp);
    fscanf(input_file, "%d", &white);
    fprintf(pgm_file, "%d\n", white);

    if (strcmp(type, "B") == 1){
        printf("writing bin");
        char temp[256];
        unsigned char avgBin;
        unsigned char R,G,B;

        while(fscanf(input_file,"%d", &R) == 1){
            fscanf(input_file,"%d", &G);
            fscanf(input_file,"%d", &B);
            avgBin = (R+G+B) / 3;

            fwrite(&avgBin, sizeof(avgBin), 1, pgm_file);
        }

    } else {
        printf("writing ascii");

        while(fscanf(input_file,"%d", &R) == 1){
            fscanf(input_file,"%d", &G);
            fscanf(input_file,"%d", &B);
            avg = (R+G+B) / 3;
            fprintf(pgm_file, "%d ", avg);

            if (counter >= width){
                fprintf(pgm_file,"\n");
                counter = 1;
            }
            counter++;
            //printf("%d\n", counter);
        }
    }
    fclose(pgm_file);
}
void ppm_to_pbm(FILE *input_file, char *output,int magic){
    //tento prevod je samo sebou ponekud nepekny, rozhodl jsem se pro reseni: tmavsi polovina je cerna a svetlejsi polovina je bila
    int white,width,R,G,B,avg;
    FILE* pbm_file = fopen(output, "w+");
    fprintf(pbm_file, "P1\n");

    char width_height_temp[255];
    fscanf(input_file, "%d", &width);
    fprintf(pbm_file, "%d", width);
    fscanf(input_file, "%s", width_height_temp);

    fprintf(pbm_file, " %s\n", width_height_temp);
    fscanf(input_file, "%d", &white);
    printf("%d", white);
    int temp;
    int i,counter = 0;
    while(fscanf(input_file,"%d", &R) == 1){
        fscanf(input_file,"%d", &G);
        fscanf(input_file,"%d", &B);
        avg = (R+G+B) / 3;
        if(avg >= (white / 2)){
            fprintf(pbm_file,"%d ", 0);
            counter++;
        }
        if(avg < (white / 2)){
            fprintf(pbm_file,"%d ", 1);
            counter++;
        }
        if(counter >= 255){
            fprintf(pbm_file, "\n");
            counter = 0;
        }
    }
    fclose(pbm_file);
}

void pbm_to_ppm(FILE *input_file, char *output,int magic,char *type){
    short int white = 0;
    int pixel,width = 1;
    char width_height_temp[255];
    FILE* ppm_file;

        if (strcmp(type, "B") == 1){
            ppm_file = fopen(output, "wb");
            fprintf(ppm_file, "P6\n");

        } else if (strcmp(type, "A") == 1){
            ppm_file = fopen(output, "w+");
            fprintf(ppm_file, "P3\n");
        }

    fscanf(input_file, "%s", width_height_temp);
    fprintf(ppm_file, "%s ", width_height_temp);
    fscanf(input_file, "%s", width_height_temp);
    fprintf(ppm_file, "%s ", width_height_temp);
    fscanf(input_file, "%d", &width);
    fprintf(ppm_file, " %d\n", width);

//BIN
    if (strcmp(type, "B") == 1){
        printf("writing binary");
        unsigned char val;

        while(fscanf(input_file,"%d", &val) == 1){
            val = val ^ 1;
            fwrite(&val, sizeof(val), 1, ppm_file);
            fwrite(&val, sizeof(val), 1, ppm_file);
            fwrite(&val, sizeof(val), 1, ppm_file);
        }
    }
//ASCII
    else if (strcmp(type, "A") == 1){
        printf("writing ascii");
        int avg, counter = 0;
        while(fscanf(input_file,"%d", &white) == 1){

            if(white == 0)
                fprintf(ppm_file, "%d %d %d ", 1, 1, 1);
            else fprintf(ppm_file, "%d %d %d ", 0, 0, 0);

            if (counter >= width){
                fprintf(ppm_file,"\n");
                counter = 1;
            }
            counter++;
        }
    }
    fclose(ppm_file);
}

int main(void)
{
    char input[80];
    char output[80];
    char type[8];
    printf("--------------------------------------------------------------\n");
        printf(" na linuxu funguje dobre, pouze jsem nestihnul BINARNI prebody z/do PBM.\n\n");
        printf(" prosim zadavat cele nazvy obrazku 'jmeno.typ', umisteni ve stejnem adresari jako main.c\n");
    printf("--------------------------------------------------------------\n\n");

    printf("Input file:\n");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    printf("\nOutput file:\n");
    fgets(output, sizeof(output), stdin);
    output[strcspn(output, "\n")] = '\0';

    printf("\nB (binary), A (ascii):\n");
    fgets(type, sizeof(type), stdin);

    worker(input, output,type);

    printf("\nWorker func OK\n");

    return 0;
}
