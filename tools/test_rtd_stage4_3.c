#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int contains(const char *file,const char *needle){
    FILE *f;
    long n;
    char *buf;
    size_t got;
    int found;
    f=fopen(file,"rb");
    if(!f)return 0;
    if(fseek(f,0,SEEK_END)!=0){fclose(f);return 0;}
    n=ftell(f);
    if(n<0){fclose(f);return 0;}
    rewind(f);
    buf=(char*)malloc((size_t)n+1u);
    if(!buf){fclose(f);return 0;}
    got=fread(buf,1,(size_t)n,f);
    fclose(f);
    buf[got]=0;
    found=strstr(buf,needle)!=NULL;
    free(buf);
    return found;
}

int main(void){
    if(!contains("src/rt_sensor_diag.c","SENSOR1: tick"))return 1;
    if(!contains("src/rt_sensor_diag.c","SENSOR2: tick"))return 2;
    if(!contains("src/rt_sensor_diag.c","SENSOR1: ESC -> stopped"))return 3;
    if(!contains("src/rt_sensor_diag.c","SENSOR2: ESC -> stopped"))return 4;
    if(!contains("build.sh","RT_SENSOR_DIAG_ID=1"))return 5;
    if(!contains("build.sh","RT_SENSOR_DIAG_ID=2"))return 6;
    if(!contains("build.sh","build/SENSOR1.EXE=SENSOR1.EXE"))return 7;
    if(!contains("build.sh","build/SENSOR2.EXE=SENSOR2.EXE"))return 8;
    puts("Stage 4.3 diagnostic source/build assertions passed");
    return 0;
}
