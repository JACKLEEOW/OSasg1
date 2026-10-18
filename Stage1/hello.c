#include <stdio.h>
int main(int argc, char * argv[], char * envp[]) {
    printf("---------------Test Program ---------------\n");
    printf("argc = %d\n", argc);
    for (int i = 0; argv[i] != NULL; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    printf("--------------- Envp ---------------\n");
    for (int i = 0; envp[i] != NULL; i++) {
        printf("envp[%d] = %s\n", i, envp[i]);
    }
    return 0;
}