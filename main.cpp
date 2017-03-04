#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#define BUF_SIZE 256
#define NUM_CMD 2

char *commands[]={
    "cd",
    "exit"
};

int cd(char** args){
    if(args[1]==NULL){
        fprintf(stderr,"shell:few arguments\n");
    }
    else{
        if(chdir(args[1])!=0){
            perror("shell");
        }
    }
    return 1;
}

int sysexit(char** args){
    return 0;
}

int (*cmd_func[])(char**)={
    &cd,
    &sysexit
};

char* shell_getline(void){
    int ind=0;
    char* buffer = (char*)malloc(sizeof(char)*BUF_SIZE);
    int CUR_BS=BUF_SIZE;
    if(!buffer){
        fprintf(stderr,"shell:buffer could not be allocated with memory:(\n");
        exit(EXIT_FAILURE);
    }
    int c=getchar();
    while(1){
        if(c==EOF || c=='\n'){
            buffer[ind]='\0';
            return buffer;
        }
        else{
            buffer[ind]=c;
            ind++;
        }
        if(ind>=CUR_BS){
            CUR_BS+=BUF_SIZE;
            buffer=(char*)realloc(buffer,sizeof(char)*CUR_BS);
            if(!buffer){
                fprintf(stderr,"shell:buffer could not be allocated with memory:(\n");
                exit(EXIT_FAILURE);
            }
        }
        c=getchar();
    }
}

char** shell_parsecmd(char* cmd){
    int CUR_BS=BUF_SIZE/4;
    int ind=0;
    char **tokens=(char**)malloc(CUR_BS*sizeof(char*));
    if(!tokens){
        fprintf(stderr,"shell:buffer could not be allocated with memory:(\n");
        exit(EXIT_FAILURE);
    }
    const char delim[]={' ','\t','\n','\a','\r','\0'};
    char *token=strtok(cmd,delim);
    while(token!=NULL){
        tokens[ind]=token;
        ind++;
        if(ind>=CUR_BS){
            CUR_BS+=(BUF_SIZE/4);
            tokens=(char**)realloc(tokens,CUR_BS*sizeof(char*));
            if(!tokens){
                fprintf(stderr,"shell:buffer could not be allocated with memory:(\n");
                exit(EXIT_FAILURE);
            }
        }
        token=strtok(NULL,delim);
    }
    tokens[ind]=NULL;
    return tokens;
}

int shell_launch(char** args){
    pid_t pid;
    int statcode;
    pid=fork();
    if(!pid){
        if(execvp(args[0],args)==-1){
            perror("shell");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid<0){
        perror("shell");
    }
    else{
        do{
            waitpid(pid,&statcode,WUNTRACED);
        }while(!WIFEXITED(statcode)&&!WIFSIGNALED(statcode));
    }
    return 1;
}

int shell_exec(char** args){
    int code;
    if(args[0]==NULL){
        return 1;
    }
    for(int i=0;i<NUM_CMD;i++){
        if(strcmp(args[0],commands[i])==0){
            return (*cmd_func[i])(args);
        }
    }
    return shell_launch(args);
}

void start_shell(void){
    char *cmd;
    char **args;
    int statcode=0;
    do{
        printf("parv> ");
        cmd=shell_getline();
        args=shell_parsecmd(cmd);
        statcode=shell_exec(args);
    }while(statcode);
    return;
}

int main(int argc,char** argv){
    start_shell();
    return EXIT_SUCCESS;
}
