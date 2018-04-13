#include <stdio.h>

//
// Created by Zhangqi Zha on 3/20/18.
//
int main(){
    FILE *fp = fopen("in.bin","wb");
    int buffer[250];
    for (int i = 250; i >=0 ; --i) {
        buffer[i] = 250-i;
    }
    fwrite(buffer, sizeof(int), 250, fp);
    fclose(fp);
}