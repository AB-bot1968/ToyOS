/*
 * Toy OS v62: host-side regression test для standalone path.c.
 *
 * Этот тест запускается до создания FAT16-образа. Он проверяет только
 * детерминированную строковую часть подсистемы: канонизацию, объединение,
 * parent и basename. FAT16 и BIOS здесь не участвуют, поэтому ошибка в
 * обработке пути обнаруживается раньше, чем образ будет записан.
 */
#include <stdio.h>
#include <string.h>
#include "../src/path.h"

static int expect_norm(const char *cwd,const char *path,const char *want){
    char out[128];
    if(!path_normalize(cwd,path,out,sizeof(out))){
        fprintf(stderr,"FAIL normalize: %s + %s returned failure\n",cwd,path);
        return 0;
    }
    if(strcmp(out,want)!=0){
        fprintf(stderr,"FAIL normalize: %s + %s -> %s, expected %s\n",cwd,path,out,want);
        return 0;
    }
    return 1;
}

static int expect_join(const char *a,const char *b,const char *want){
    char out[128];
    if(!path_join(a,b,out,sizeof(out))||strcmp(out,want)!=0){
        fprintf(stderr,"FAIL join: %s + %s -> %s, expected %s\n",a,b,out,want);
        return 0;
    }
    return 1;
}

static int expect_parent(const char *path,const char *want){
    char out[128];
    if(!path_parent(path,out,sizeof(out))||strcmp(out,want)!=0){
        fprintf(stderr,"FAIL parent: %s -> %s, expected %s\n",path,out,want);
        return 0;
    }
    return 1;
}

static int expect_base(const char *path,const char *want){
    char out[128];
    if(!path_basename(path,out,sizeof(out))||strcmp(out,want)!=0){
        fprintf(stderr,"FAIL basename: %s -> %s, expected %s\n",path,out,want);
        return 0;
    }
    return 1;
}

int main(void){
    int ok=1;
    ok&=expect_norm("/",".","/");
    ok&=expect_norm("/BIN","..","/");
    ok&=expect_norm("/BIN","./../BIN/./","/BIN");
    ok&=expect_norm("/BIN","DOC/../HELLO.EXE","/BIN/HELLO.EXE");
    ok&=expect_norm("/BIN","/DOC/NET.CFG","/DOC/NET.CFG");
    ok&=expect_norm("/A/B","../../C","/C");
    ok&=expect_norm("/","A//B///C","/A/B/C");
    ok&=expect_norm("/","aBc.TxT","/ABC.TXT");
    ok&=expect_norm("/","AUTOSTART.SH","/AUTOSTART.SH");
    ok&=expect_norm("/","autostart.sh","/AUTOSTART.SH");
    ok&=expect_join("/BIN","HELLO.EXE","/BIN/HELLO.EXE");
    ok&=expect_join("/BIN","../DOC","/DOC");
    ok&=expect_join("/BIN","/DOC","/DOC");
    ok&=expect_parent("/BIN/HELLO.EXE","/BIN");
    ok&=expect_parent("/BIN","/");
    ok&=expect_parent("/","/");
    ok&=expect_base("/BIN/HELLO.EXE","HELLO.EXE");
    ok&=expect_base("HELLO.EXE","HELLO.EXE");
    ok&=expect_base("AUTOSTART.SH","AUTOSTART.SH");
    if(!ok)return 1;
    puts("PASS: v62 path module regression");
    return 0;
}
