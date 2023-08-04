/* 
Programa para realizar operações básicas em imagens no formato 
PGM (Portable Gray Map). Ele permite a leitura de um arquivo de 
imagem PGM, realização de operações como logaritmo, esticamento de 
contraste e preenchimento de regiões (similar ao "baldinho" do Paint).
Além disso, tem a capacidade de aplicar filtros à imagem, como o Mean
Filter, Median Filter e Gaussian Smoothing. Por fim, salva a imagem
resultante em um novo arquivo.

Autor: Matheus Paiva Angarola (2023)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define KERNEL 3 
#define stdDevation 1 // Desvio Padrão (standard deviation) 

typedef struct{
    char formato[2]; // Tipo da imagem (binário ou ASCII)
    char comentario[100]; // Comentário da imagem
    int dimX, dimY; // Dimensões X e Y da imagem
    int maxVAL, minVAL; // Valores do pixel de maior valor e o de menor valor da imagem
    int **pixels; // Pixels da imagem (estrutura bi-dimensional)
} formatoImagem;


// Protótipos de funções
void lerImagem(formatoImagem *imagem, char *nome);
void operacoes(formatoImagem *imagem, int operacao);
void opLog (formatoImagem *imagem);
void opEsticar (formatoImagem *imagem);
void opBaldinho (formatoImagem *imagem, int cor_nova);
void opExp (formatoImagem *imagem);
void opNegativo (formatoImagem *imagem);
void floodFill (formatoImagem *imagem, int i, int j, int cor_antiga, int cor_nova);
void preencherPixels (formatoImagem *imagem, FILE *arquivo, char *nome);
int termoCentral (int *lista, int n);
void preencherBordas(formatoImagem *imagem);
void copiarImagem(formatoImagem *imagem, int **imagemSaida, int meioKernel);
void opMeanFilter(formatoImagem *imagem, int kernel);
void opMedianFilter(formatoImagem *imagem, int kernel);
int **alocarMatriz(int x, int y);
void desalocar(int ***matriz, int y);

int **alocarMatriz(int x, int y){
  int **matriz,i;
  
  matriz = (int **) malloc(sizeof(int *)*y);
  if (matriz == NULL) {
    printf("Nao deu\n");
    return(NULL);
  }

  for (i = 0; i < y; i++) {
    matriz[i] = (int *) malloc(sizeof(int)*x);
    if (matriz[i] == NULL) {
      printf("Nao deu\n");
      return(NULL);
    }
  }

    return matriz;
}

void desalocar(int ***matriz, int y){
    int i;
  
    for (i = 0; i < y; i++) {
        free((*matriz)[i]);
        (*matriz)[i] = NULL;
    }

    free(*matriz);
    *matriz = NULL;

    return;
}

void maiorPixel (formatoImagem *imagem) {
    int maior = 0;
    for (int i = 0; i < (*imagem).dimY; i++){
        for (int j = 0; j < (*imagem).dimX; j++){
            if (maior < (*imagem).pixels[i][j]){
                maior = (*imagem).pixels[i][j];
            }
        }
    }
    (*imagem).maxVAL = maior;
}

// Protótipo de histograma
void histograma(formatoImagem *imagem) {
    int hist [(*imagem).maxVAL+1], pos;

    // Preenche a matriz com o valor inicial de zero, para evitar erros 
    for(int i = 0; i < (*imagem).maxVAL; i++){
        hist[i] = 0;
    }

    // Percorre a imagem e contabiliza a quantidade de pixel no intervalo das cores
    for (int i = 0; i < (*imagem).dimY; i++){
        for (int j = 0; j < (*imagem).dimX; j++){
            pos = (*imagem).pixels[i][j];
            hist[pos]++;
        }
    }

    for (int i = 0; i <= (*imagem).maxVAL; i++)
    {
        int count = hist[i] / 50; // Divisão para apresentar apenas os valores mais relevantes
        printf("%d |", i); // Valor da cor
        for (int j = 0; j < count; j++)
        {
            printf("%c", (char)254u); // Printa um cubinho (tentativa de plotar um gráfico)
        }
        printf("\n");
    }

}

// Funções Point Operations

void opLog (formatoImagem *imagem){
    int c, i, j; 
    c = 255 / (log(1+(*imagem).maxVAL));
    for (i = 0; i < (*imagem).dimY; i++){
        for (j = 0; j < (*imagem).dimX; j++){
            (*imagem).pixels[i][j] = c * (log(1+(*imagem).pixels[i][j]));
        }
    }
}

void opEsticar (formatoImagem *imagem){
    int i, j; 
    for (i = 0; i < (*imagem).dimY; i++){
        for (j = 0; j < (*imagem).dimX; j++){
            (*imagem).pixels[i][j] = ((*imagem).pixels[i][j] - (*imagem).minVAL) * ((255)/((*imagem).maxVAL - (*imagem).minVAL));
        }
    }
}

void opBaldinho (formatoImagem *imagem, int cor_nova){
    int i = (*imagem).dimY / 2;
    int j = (*imagem).dimX / 2;

    int cor_antiga = (*imagem).pixels[i][j];
    if (cor_antiga == cor_nova) {
        return;
    }
    floodFill(imagem, i, j, cor_antiga, cor_nova);
}

void floodFill (formatoImagem *imagem, int i, int j, int cor_antiga, int cor_nova){
    if (i < 0 || i >= (*imagem).dimY) return;
    if (j < 0 || j >= (*imagem).dimX) return;
    if (cor_antiga != (*imagem).pixels[i][j]) return;

    (*imagem).pixels[i][j] = cor_nova;
    floodFill (imagem, i+1, j, cor_antiga, cor_nova);
    floodFill (imagem, i-1, j, cor_antiga, cor_nova);
    floodFill (imagem, i, j+1, cor_antiga, cor_nova);
    floodFill (imagem, i, j-1, cor_antiga, cor_nova);
}

// Dúvida: quais valores colocar para a exponencial?
void opExp (formatoImagem *imagem) {
}

void opNegativo (formatoImagem *imagem) {
    int i, j; 
    for (i = 0; i < (*imagem).dimY; i++){
        for (j = 0; j < (*imagem).dimX; j++){
            (*imagem).pixels[i][j] = (*imagem).maxVAL - 1 - (*imagem).pixels[i][j];
        }
    }
}

void opThresholding (formatoImagem *imagem) {
    int i, j, value; 
    scanf("%d", &value);
    for (i = 0; i < (*imagem).dimY; i++){
        for (j = 0; j < (*imagem).dimX; j++){

            if ((*imagem).pixels[i][j] > value) {
                (*imagem).pixels[i][j] = 255; // Pixel branco
            } else {
                (*imagem).pixels[i][j] = 0; // Pixel preto
            }
        }
    }
}

// Funções Spatial filters

int termoCentral (int *lista, int n){ // Bubble sort + termo central (mediana)
    int k, j, aux, central;
    central = (n/2) + 0.5; // Achar a posição da mediana

    for (k = 1; k < n; k++) { // Realiza um bubble sort na lista da vizinhança
        for (j = 0; j < n - k; j++) {
            if (lista[j] > lista[j+1]) {
                aux = lista[j];
                lista[j] = lista[j+1];
                lista[j+1] = aux;
            }
        }
    }

    return lista[central]; // Retorna o valor da mediana da vizinhança
}

void preencherBordas(formatoImagem *imagem) {
    // Caso dado pixel não seja "coberto" pelo kernel, ele será preenchido com uma constante (100 - cinza)
    for (int i = 0; i < (*imagem).dimY; i++){
        for (int j = 0; j < (*imagem).dimX; j++){
            if (i <= KERNEL - 1 || i >= (*imagem).dimY - KERNEL - 1){
                (*imagem).pixels[i][j] = 100;
            } 
            if (j <= KERNEL - 1 || j >= (*imagem).dimX - KERNEL - 1){
                (*imagem).pixels[i][j] = 100;
            } 
        }
    }
}

void copiarImagem(formatoImagem *imagem, int **imagemSaida, int meioKernel){
    int y, x;
    for (y = meioKernel; y < (*imagem).dimY - meioKernel; y++) {
        for (x = meioKernel; x < (*imagem).dimX - meioKernel; x++) {
            (*imagem).pixels[y][x] = imagemSaida[y][x];
        }
    }
}

void opMeanFilter(formatoImagem *imagem, int kernel){
    int **imagemSaida = alocarMatriz((*imagem).dimX, (*imagem).dimY); // Matriz para "guardar temporariamente os pixels novos"
    int soma = 0, x, y;
    int meioKernel = kernel / 2; //Utilizado para percorrer a vizinhança do pixel de forma mais fácil 

    // Dúvida: para que não haja erro de segmentação do código ignorei os pixels da borda, ou seja,
    // o filtro só será aplicado onde o Kernel se encaixar perfeitamente. Por isso, os pixels da borda ficarão como os anteriores.

    /* Soluções --> Isso é um problema comum
        - Ignorar a borda
        - Preencher com valores constantes
        - Preencher com valores refletidos
    */  

    for (int y = meioKernel; y < (*imagem).dimY - meioKernel; y++) {
        for (int x = meioKernel; x < (*imagem).dimX - meioKernel; x++) {
            int sum = 0;
            
            // Percorre a vizinhança do pixel atual usando o tamanho do kernel
            for (int j = -meioKernel; j <= meioKernel; j++) {
                for (int i = -meioKernel; i <= meioKernel; i++) {
                    sum += (*imagem).pixels[y + j][x + i];
                }
            }
            
            // Calcula o valor da média e atualiza o pixel na saída
            imagemSaida[y][x] = sum / pow(kernel, 2);
        }
    }

    copiarImagem(imagem, imagemSaida, meioKernel);
    preencherBordas(imagem);
    free(imagemSaida);
}

void opMedianFilter(formatoImagem *imagem, int kernel){
    int **imagemSaida = alocarMatriz((*imagem).dimX, (*imagem).dimY); // Matriz para "guardar" temporariamente os pixels novos"
    int x, y, sum, index;
    int meioKernel = kernel / 2; //Utilizado para percorrer a vizinhança do pixel de forma mais fácil 
    int elementosAUX[kernel*kernel]; // Lista que armazenará a vizinhança

    for (y = meioKernel; y < (*imagem).dimY - meioKernel; y++) {
        for (x = meioKernel; x < (*imagem).dimX - meioKernel; x++) {
            sum = 0;
            index = 0;
            // Percorre a vizinhança do pixel atual usando o tamanho do kernel
            for (int j = -meioKernel; j <= meioKernel; j++) {
                for (int i = -meioKernel; i <= meioKernel; i++) {
                    elementosAUX[index++] = (*imagem).pixels[y + j][x + i];
                }
            }

            // Orderna a lista que contem a vizinhança e seleciona o termo central
            imagemSaida[y][x] = termoCentral(elementosAUX, kernel*kernel);
        }
    }

    copiarImagem(imagem, imagemSaida, meioKernel);
    preencherBordas(imagem);
    free(imagemSaida);
}

void opGaussianSmt (formatoImagem *imagem, int kernel, int stdDev){

}

void operacoes(formatoImagem *imagem, int operacao){
    int op;
    switch (operacao)
    {
        case 1:
            printf("\n--- Point Operations ---\n");
            printf("[1] Logarithmic Operation\n");
            printf("[2] Contrast Stretching Operation\n");
            printf("[3] Paint Bucket Operation (Flood Fill)\n");
            printf("[4] Exponential Operation\n");
            printf("[5] Negative Operation\n");
            printf("[6] Thresholding Operation\n");
            printf("\nOperation: ");
            scanf("%d", &op);

            switch (op)
            {
                case 1: // Operação de log
                    opLog(imagem);
                    break;
                case 2: // Operação de “esticar” o constraste
                    opEsticar(imagem);
                    break;
                case 3: // Operação “baldinho do paint brush”
                    opBaldinho(imagem, 100);
                    break;
                case 4: // Operação exponencial
                    opExp(imagem);
                    break;
                case 5: // Operação negativo
                    opNegativo(imagem);
                    break;
                case 6: // Operação negativo
                    opThresholding(imagem);
                    break;
                default: // Caso a entrada seja irregular
                    printf("ERROR: nonexistent or irregular operation!\n");
                    return;
            }

            break;

        case 2:
            printf("\n--- Digital Filters ---\n");
            printf("[1] Mean Filter\n");
            printf("[2] Median Filter\n");
            printf("[3] Gaussian Smoothing (work in progress)\n");
            printf("\nOperation: ");
            scanf("%d", &op);

            switch (op)
            {
                case 1: // Operação de Mean Filter
                    opMeanFilter(imagem, KERNEL); // Kernel/mascara entre 3x3, 5x5 ou 7x7
                    break;
                case 2: // Operação de Median Filter
                    opMedianFilter(imagem, KERNEL); // Kernel/mascara entre 3x3, 5x5 ou 7x7
                    break;
                case 3: // Operação de Gaussian Smoothing
                    exit(1);
                    break;

                default: // Caso a entrada seja irregular
                    printf("ERROR: nonexistent or irregular operation!\n");
                    return;
            }

            break;

        case 3:
            histograma(imagem);
            break;

        default: // Caso a entrada seja irregular
            printf("ERROR: nonexistent or irregular operation!\n");
            return;
    }
}

void preencherPixels (formatoImagem *imagem, FILE *arquivo, char *nome){
    int i, j, aux, menorValor;
    menorValor = 255; // Definimos o valor máximo como padrão, pois o ASCII vai de 0 até 255

    if ((*imagem).formato[1] == '5'){ // Se não estiver em ASCII, irá transformar
        unsigned char pixel; 
        for (i = 0; i < (*imagem).dimY; i++){
            for (j = 0; j < (*imagem).dimX; j++){
                fread(&pixel, sizeof(unsigned char), 1, arquivo); // Le os bytes
                (*imagem).pixels[i][j] = (int) pixel; // Transformar o unsigned char em int e o coloca na matriz pixels
                if ((*imagem).pixels[i][j] < menorValor) {
                    menorValor = (int) pixel;
                }
            }
        }
    (*imagem).formato[1] = '2'; // Muda o formato de binário para ASCII
    } else {
        // Percorre a matriz dos pixels e escreve o valor de cada pixel da imagem original
        for (i = 0; i < (*imagem).dimY; i++){
            for (j = 0; j < (*imagem).dimX; j++){
                fscanf(arquivo, "%d", &(*imagem).pixels[i][j]);
                if ((*imagem).pixels[i][j] < menorValor) {
                    menorValor = (*imagem).pixels[i][j];
                }
            }
        }
    }
    (*imagem).minVAL = menorValor;
}

void lerImagem(formatoImagem *imagem, char *nome){

    FILE *arquivo;
    if((arquivo = fopen(nome, "rb")) == NULL){ // Faz a verificação se o arquivo foi aberto corretamente
        printf("ERROR: Failed to open image file.\n");
        exit(1);
    }

    fgets((*imagem).formato, sizeof((*imagem).formato) + 2, arquivo); // Recolhe o formato dos pixels da imagem
    fgets((*imagem).comentario, sizeof((*imagem).comentario) + 3, arquivo); // Recolhe o comentário do arquivo
    fscanf(arquivo, "%d", &(*imagem).dimX); // Recolhe a dimensão X da imagem
    fscanf(arquivo, "%d", &(*imagem).dimY); // Recolhe a dimensão Y da imagem
    fscanf(arquivo, "%d", &(*imagem).maxVAL); // Recolhe o valor máximo de cada pixel da imagem

    (*imagem).pixels = alocarMatriz((*imagem).dimX, (*imagem).dimY); // Aloca dinamicamente a matriz para guardas os pixels da imagem
    
    preencherPixels(&(*imagem), arquivo, nome); // Preencher a matriz com os pixels da Imagem

    fclose(arquivo); // Fecha arquivo anteriormente aberto
}

void salvarImagem(formatoImagem *imagem) {
    int i, j;
    FILE *arquivoFinal;
    if((arquivoFinal = fopen("imagemGerada.pgm", "w")) == NULL){ // Faz a verificação se o arquivo foi aberto corretamente
        printf("ERROR: falha ao abrir o arquivo da imagem.\n");
        exit(1);
    }

    fprintf(arquivoFinal, "P%c\n", (*imagem).formato[1]);
    fprintf(arquivoFinal, "# CREATOR: Image Generator SCC-221 - ICC I\n");
    fprintf(arquivoFinal, "%d %d\n", (*imagem).dimX, (*imagem).dimY);
    fprintf(arquivoFinal, "%d\n", (*imagem).maxVAL);

    for (i =0; i < (*imagem).dimY; i++){
        for (j = 0; j < (*imagem).dimX; j++){
            fprintf(arquivoFinal, "%d\n", (*imagem).pixels[i][j]);
        }
    }

    fclose(arquivoFinal); // Fecha arquivo anteriormente aberto
}

int main(){
    formatoImagem imagem;
    int operacao, i, j;
    char nomeArquivo[30];

    printf("PGM image file: ");
    scanf("%s", nomeArquivo);

    printf("\n--- General Operations ---\n");
    printf("[1] Point Operations \n");
    printf("[2] Digital Filters \n");
    printf("[3] Histogram Prototype\n");

    // Recolhimento inicial dos dados
    printf("\nOperation: ");
    scanf("%d", &operacao);

    lerImagem(&imagem, nomeArquivo); // Lê os dados do arquivo da imagem
    operacoes(&imagem, operacao); // Redireciona de acordo com cada operação

    maiorPixel(&imagem);

    salvarImagem(&imagem); // Salva a imagem após a operação desejada

    printf("Image generated successfully!\n"); // Mensagem de sucesso

    desalocar(&imagem.pixels, imagem.dimY); // Libera espaço destinado a alocação dos pixels

    return 0;
}