#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // MAX_PATH
#include <string.h> // strtok
#include <unistd.h> // access

// definindo tamanho do buffer
#define BUFFER_SIZE 1024


// função para calcular a média de um vetor
float mean(float dados[], int N) {
    float sum = 0;
    for (int i = 0; i < N; i++) {
        sum += dados[i];
    }
    return sum / N;
}


// função para calcular a variância de um vetor
float variance(float data[], int N) {
    float data_mean = mean(data, N);
    float MSE = 0;

    for (int i = 0; i < N; i++) {
        MSE += (data[i] - data_mean) * (data[i] - data_mean);
    }

    return MSE / N;
}


// função para calcular a covariância entre dois vetores
float covariance(float x[], float y[], int N, float X_mean, float Y_mean) {

    float sum = 0;
    for (int i = 0; i < N; i++) {
        sum += (x[i] - X_mean) * (y[i] - Y_mean);
    }

    return sum / (N - 1);
}


// função para calcular o coeficiente angular (inclinação) da reta de regressão
float slope(float XY_covariance, float X_variance) {
    if (X_variance == 0) {
        return 0; // evitar divisão por zero
    }
    return XY_covariance / X_variance;
}


// função para calcular o coeficiente linear (intercepto) da reta de regressão
float intercept(float X_mean, float Y_mean, float coef_slope) {
    return Y_mean - (coef_slope * X_mean);
}


// função principal
int main(int argc, char *argv[]) {

    // verificando argumentos
    if (argc < 2) {
        printf("Uso: %s <file_path> <target_feature>\n", argv[0]);
        exit(1);
    }

    printf("argc = %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    // verificando se o arquivo existe
    if (access(argv[1], F_OK) != 0) {
        printf("Arquivo %s não encontrado.\n", argv[1]);
        exit(1);
    }

    // carregando parâmetros
    char file_path[MAX_PATH];
    char target_feature[100];
    strcpy(file_path, argv[1]);
    strcpy(target_feature, argv[2]);

    // definindo parâmetros
    char buffer[BUFFER_SIZE];
    int array_size = 100;   
    float* y = malloc(sizeof(float) * array_size);
    float* x = malloc(sizeof(float) * array_size);

    // checando se a memória foir alocada corretamente
    if (y == NULL || x == NULL) {
        printf("Memory not allocated.\n");
        exit(1);
    }

    // carregando arquivo
    printf("carregando arquivo...\n");
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    // lendo títulos das features
    fgets(buffer, sizeof(buffer), fp);

    // removendo \r e \n do buffer
    buffer[strcspn(buffer, "\r\n")] = 0;

    char* token = strtok(buffer, ","); // primeiro token da linha
    int k = 0;
    int col_target_feature = -1;
    while (token != NULL) {
        if (strcmp(token, target_feature) == 0) {
            col_target_feature = k;
            break;
        }
        token = strtok(NULL, ",");
        k++;
    }

    if (col_target_feature == -1) {
        printf("Feature %s não encontrada.\n", target_feature);
        exit(1);
    } 

    // lendo restante das linhas
    int i = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {

        // removendo \r e \n do buffer
        buffer[strcspn(buffer, "\r\n")] = 0;

        // realocando a memória se necessário
        if (i + 2 == array_size) {

            array_size = array_size * 2;
            float* y_res = realloc(y, sizeof(float) * array_size);
            float* x_res = realloc(x, sizeof(float) * array_size);

            // checando se a memória foi realocada coorretamente
            if (y_res == NULL || x_res == NULL) {
                printf("Memory not reallocated.\n");
                exit(1);
            }

            // atribuindo novos pointers
            y = y_res;
            x = x_res;
        }

        // iterando até 'target_feature'
        for (int j = 0; j <= k; j++) {
            if (j == 0) {
                token = strtok(buffer, ",");
            } else {
                token = strtok(NULL, ",");
            }
        }

        // lendo 'target_feature'
        y[i] = atof(token);
        x[i] = i + 1; // populando array x

        i++;
    }

    const int N = i;
    printf("%s: %d observações carregadas\n", target_feature, N);

    // fechando o arquivo
    fclose(fp);

    printf("calculando regresão linear simples...\n");
    float X_mean = mean(x, N);
    float Y_mean = mean(y, N);

    // calculando a covariância e variância de X
    float XY_covariance = covariance(x, y, N, X_mean, Y_mean);
    float X_variance = variance(x, N);

    // calculando o coeficiente angular (inclinação) e linear (intercepto)
    float coef_slope = slope(XY_covariance, X_variance);
    printf("inclinação: %f\n", coef_slope);
    float coef_intercept = intercept(X_mean, Y_mean, coef_slope);
    printf("intercepto: %f\n", coef_intercept);

    // resultados
    printf("equação da reta de regressão: y = %.2f + %.2fx\n", coef_intercept, coef_slope);

    // desalocando
    free(x);
    free(y);

    return 0;
}