#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

typedef struct {
    int id;
    char *nome;
    char *senha;
    float saldo;
}usuario;

//Assinaturas
int main();
void menu(int);
void login();
void cadastro_usuario(usuario **, char **);
void sacar(int);
void consultar_saldo(int);
void transferir(int);
void verificacao_proximo_id();

//variaveis globais
int proximo_id;
int qt_usuarios_cadastrados;

void cadastro_usuario(usuario **user, char **linhas) {
    char buffer_nome[100];
    char buffer_senha[20];
    char copia[130];
    char linha[130];

    *linhas = realloc(*linhas, (qt_usuarios_cadastrados + 1) * sizeof(linha));

    *user = realloc(*user, (qt_usuarios_cadastrados + 1) * sizeof(usuario));

    system("cls");

    printf("============ CADASTRO DE USUARIO ============\n");

    printf("Usuario: ");
    fgets(buffer_nome, 100, stdin);
    buffer_nome[strcspn(buffer_nome, "\n")] = '\0';

    printf("Senha: ");
    fgets(buffer_senha, 20, stdin);
    buffer_senha[strcspn(buffer_senha, "\n")] = '\0';

    FILE *arq_login = fopen("Login.txt", "r");

    while (fgets(linha, sizeof(linha), arq_login)) {

        strcpy(copia, linha);

        char *nome_temp = strtok(copia, ";");

        if (strcmp(buffer_nome, nome_temp) == 0) {
            fclose(arq_login);

            printf("Nome de usuario ja existente!\n");
            printf("Digite ENTER para voltar...");
            getchar();
            return;
        }
    }

    (*user)[qt_usuarios_cadastrados].nome = (char*) malloc(strlen(buffer_nome) + 1);
    strcpy((*user)[qt_usuarios_cadastrados].nome, buffer_nome);

    (*user)[qt_usuarios_cadastrados].senha = (char*) malloc(strlen(buffer_senha) + 1);
    strcpy((*user)[qt_usuarios_cadastrados].senha, buffer_senha);

    verificacao_proximo_id();

    (*user)[qt_usuarios_cadastrados].id = proximo_id;
    (*user)[qt_usuarios_cadastrados].saldo = 0;

    arq_login = fopen("Login.txt", "a");

    fprintf(arq_login, "%s;%s;%d;%.2f#\n", (*user)[qt_usuarios_cadastrados].nome, (*user)[qt_usuarios_cadastrados].senha, (*user)[qt_usuarios_cadastrados].id,
            (*user)[qt_usuarios_cadastrados].saldo);

    fclose(arq_login);

}

void login() {
    
    int id;
    char linha[130];
    char nome[100], senha[20];

    FILE *arq_login = fopen("Login.txt", "r");

    system("cls");
    printf("============ LOGIN ============\n");

    printf("Usuario: ");
    fgets(nome, 100, stdin);
    nome[strcspn(nome, "\n")] = '\0';

    printf("Senha: ");
    fgets(senha, 20, stdin);
    senha[strcspn(senha, "\n")] = '\0';

    int encontrado = 0;

    while (fgets(linha, sizeof(linha), arq_login)) {
        char *usuario_arquivo = strtok(linha, ";");
        char *senha_arquivo = strtok(NULL, ";");
        char *id_temp = strtok(NULL, ";");

        if (usuario_arquivo && senha_arquivo) {
            if (strcmp(nome, usuario_arquivo) == 0 && strcmp(senha, senha_arquivo) == 0) {
                encontrado = 1;

                id = atoi(id_temp); 
                break;
            }
        }
    }

    if (encontrado) {
        printf("Login bem-sucedido!");
        Sleep(2000);
        menu(id);
    } else {
        printf("Usuario ou senha incorretos!");
        Sleep(2000);
    }

    fclose(arq_login);

}

void sacar(int id) {

    float saque;
    char linha[130];
    char linhas[100][130];
    int total_linhas = 0;

    system("cls");

    printf("============ SAQUE ============\n");

    printf("Quanto voce deseja sacar: ");
    
    if (scanf("%f", &saque) == 0) {

        int c;
        while((c = getchar()) != '\n' && c != EOF);

        printf("Digite um numero valido!");
        Sleep(2000);
        return;
    }

    FILE *arq_login = fopen("Login.txt", "r");

    if (arq_login == NULL) {
        printf("Erro ao abrir arquivo!\n");
        Sleep(2000);
        return;
    }

    while (fgets(linha, sizeof(linha), arq_login)) {

        char copia[130];
        strcpy(copia, linha);

        char *nome = strtok(copia, ";");
        char *senha = strtok(NULL, ";");
        char *id_temp = strtok(NULL, ";");
        char *saldo_temp = strtok(NULL, "#");

        if (atoi(id_temp) == id) {

            float saldo = atof(saldo_temp);

            if (saldo <= 0) {
                printf("Saldo insuficiente!\n");
                fclose(arq_login);
                Sleep(2000);
                return;
            }

            if (saque > saldo) {
                printf("Voce nao pode sacar mais do que possui!\n");
                fclose(arq_login);
                Sleep(2000);
                return;
            }

            saldo -= saque;

            sprintf(
                linha,
                "%s;%s;%d;%.2f#\n",
                nome,
                senha,
                id,
                saldo
            );
        }

        strcpy(linhas[total_linhas], linha);
        total_linhas++;
    }

    fclose(arq_login);

    arq_login = fopen("Login.txt", "w");

    for (int i = 0; i < total_linhas; i++) {
        fputs(linhas[i], arq_login);
    }

    fclose(arq_login);

    printf("Saque realizado com sucesso!\n");
    Sleep(2000);
}

void depositar(int id) {
    
    char linha[130];
    char linhas[100][130];
    float deposito;
    int total_linhas = 0;

    system("cls");

    printf("============ DEPOSITAR DINHEIRO ============\n");

    printf("Quanto voce deseja depositar: ");
    
    if (scanf("%f", &deposito) == 0) {

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        printf("Digite um numero valido!");
        Sleep(2000);
        return;
    }

    FILE *arq_login = fopen("Login.txt", "r");

    while (fgets(linha, sizeof(linha), arq_login)) {
        char copia[130];
        strcpy(copia, linha);

        char *nome_temp = strtok(copia, ";");
        char *senha_temp = strtok(NULL, ";");
        char *id_temp = strtok(NULL, ";");
        char *saldo_temp = strtok(NULL, ";");

        if (atoi(id_temp) == id) {

            float saldo = atof(saldo_temp);
            saldo += deposito;

            sprintf(linha, 
                "%s;%s;%d;%.2f#\n",
                nome_temp,
                senha_temp,
                id,
                saldo);
        }

        strcpy(linhas[total_linhas], linha);
        total_linhas++;
    }

    fclose(arq_login);

    arq_login = fopen("Login.txt", "w");

    for (int i = 0; i < total_linhas; i++) {
        fputs(linhas[i], arq_login);
    }

    fclose(arq_login);

    printf("Deposito realizado com sucesso!\n");
    Sleep(2000);
}

void consultar_saldo(int id) {

    float saldo;
    char linha[130];

    system("cls");
    
    printf("============ CONSULTAR SALDO ============\n");

    FILE *arq_saldo = fopen("Login.txt", "r");

    while (fgets(linha, sizeof(linha), arq_saldo)) {

        strtok(linha, ";");
        strtok(NULL, ";");      
        char *id_temp = strtok(NULL, ";");         
        char *saldo_temp = strtok(NULL, "#");

        if (id == atoi(id_temp)) {
            saldo = atof(saldo_temp);
            break;
        }
    }

    printf("Saldo: %.2f\n", saldo);
    printf("Pressione ENTER para voltar...");
    getchar();

    fclose(arq_saldo);
}

void transferir(int id) {

    char nome[100];
    char copia[130];
    char linha[130];
    char linhas[100][130];
    int total_linhas = 0;
    int encontrado = 0;

    system("cls");

    printf("============ TRANSFERENCIA ============\n");

    printf("Digite o nome do usuario: ");
    fgets(nome, 100, stdin);
    nome[strcspn(nome, "\n")] = '\0';

    FILE *arq_login = fopen("Login.txt", "r");

    while (fgets(linha, sizeof(linha), arq_login)) {

        strcpy(copia, linha);
        char *nome_temp = strtok(copia, ";");
        char *senha_temp = strtok(NULL, ";");
        char *id_temp = strtok(NULL, ";");

        if (strcmp(nome, nome_temp) == 0) {
            if (atoi(id_temp) == id) {
                printf("Voce nao pode transferir para si mesmo!");
                Sleep(2000);
                return;
            }

            encontrado = 1;
            break;
        }
        
    }
    
    if (encontrado != 1) {
        printf("Usuario nao encontrado!");
        Sleep(2000);
        return;
    }

    fseek(arq_login, 0, SEEK_SET);

    if (encontrado == 1) {
        float valor_transferencia;

        printf("Digite o valor da transferencia: ");

        if (scanf("%f", &valor_transferencia) == 0) {

            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            printf("\nDigite um numero valido!");
            Sleep(2000);
            return;
        }

        fseek(arq_login, 0, SEEK_SET);
        while (fgets(linha, sizeof(linha), arq_login)) {
            strcpy(copia, linha);

            char *nome_temp = strtok(copia, ";");
            char *senha_temp = strtok(NULL, ";");
            char *id_temp = strtok(NULL, ";");
            char *saldo_temp = strtok(NULL, "#");

            if (atoi(id_temp) == id) {
                if (valor_transferencia > atof(saldo_temp)) {
                    printf("Saldo Insuficiente!");
                    Sleep(2000);
                    return;
                }
            }
        }

        fseek(arq_login, 0, SEEK_SET);
        while(fgets(linha, sizeof(linha), arq_login)) {

            strcpy(copia, linha);
                
            char *nome_temp = strtok(copia, ";");
            char *senha_temp = strtok(NULL, ";");
            char *id_temp = strtok(NULL, ";");
            char *saldo_temp = strtok(NULL, "#");

            if (strcmp(nome, nome_temp) == 0) {

                float saldo = atof(saldo_temp) + valor_transferencia;

                sprintf(linha, 
                    "%s;%s;%d;%.2f#\n",
                    nome_temp,
                    senha_temp,
                    atoi(id_temp),
                    saldo);
            }

            strcpy(linhas[total_linhas], linha);
            total_linhas++;
        }

        fclose(arq_login);

        arq_login = fopen("Login.txt", "w");

        for (int i = 0; i < total_linhas; i++) {
            fputs(linhas[i], arq_login);
        }

        fclose(arq_login);

        arq_login = fopen("Login.txt", "r");

        while (fgets(linha, sizeof(linha), arq_login)) {

            strcpy(copia, linha);

            char *nome_temp = strtok(copia, ";");
            char *senha_temp = strtok(NULL, ";");
            char *id_temp = strtok(NULL, ";");
            char *saldo_temp = strtok(NULL, "#");

            if (id == atoi(id_temp)) {

                float saldo = atof(saldo_temp) - valor_transferencia;

                sprintf(linha, "%s;%s;%d;%.2f#\n",
                    nome_temp,
                    senha_temp,
                    atoi(id_temp),
                    saldo);
                    total_linhas = 0;
            }

            strcpy(linhas[total_linhas], linha);
            total_linhas++;
        }

        fclose(arq_login);

        arq_login = fopen("Login.txt", "w");

        for (int i = 0; i < total_linhas; i++) {
            fputs(linhas[i], arq_login);
        }

        fclose(arq_login);
    }

    printf("Transferencia Realizada Com Sucesso!\n");
    printf("Pressione ENTER para voltar...");
    getchar();

}

void verificacao_proximo_id() {

    char copia[130];
    char linha[130];

    FILE *arq_login = fopen("Login.txt", "r");

    int ultimo_id = 100;

    while(fgets(linha, sizeof(linha), arq_login)) {

        strcpy(copia, linha);
    
        strtok(copia, ";");
        strtok(NULL, ";");

        char *id_temp = strtok(NULL, ";");

        if (id_temp != NULL) {
            ultimo_id = atoi(id_temp);
        }

    }

    fclose(arq_login);

    proximo_id = ultimo_id + 1;
}

void menu(int id) {
    int opcao;

    do {
        system("cls");

        printf("============ MENU ============\n");
        printf("1 - Consultar Saldo\n");
        printf("2 - Sacar\n3 - Depositar\n4 - Transferir\n");
        printf("5 - Voltar\n");
        printf("==============================\n");

        printf("Digite uma opcao: ");
        if (scanf("%d", &opcao) != 1) {

            int c;
            while((c = getchar()) != '\n' && c != EOF);

            opcao = 6;
            continue;
        }

        int c;
        while((c = getchar()) != '\n' && c != EOF);
        
        switch(opcao) {
            case 1:
               consultar_saldo(id);
               break;
            case 2:
                sacar(id);
                break;
            case 3:
                depositar(id);
                break;
            case 4:
                transferir(id);
                break;
            case 5:
                break;
            default:
                printf("Digite uma opcao valida!");
                Sleep(2000);
        }
    } while (opcao != 5);

}

int main() {

    usuario *user = NULL;
    char *linhas = NULL;
    int opcao;

    do {
        system("cls");

        printf("============ CADASTRO/LOGIN ============\n");
        printf("1 - Login\n2 - Cadastrar conta\n3 - Sair\n");
        printf("========================================\n");
        printf("Digite uma opcao: ");

        if (scanf("%d", &opcao) != 1) {

            int c;
            while((c = getchar()) != '\n');

            opcao = 6;
            continue;
        }

        int c;
        while((c = getchar()) != '\n');

        switch(opcao) {
            case 1:
                login();
                break;
            case 2:
                cadastro_usuario(&user, &linhas);
                break;
            case 3:
                break;
            default:
                printf("opcao invalida!");
                Sleep(2000);
        }
    } while (opcao != 3);

    return 0;
}