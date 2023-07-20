#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define inf 99999;

// int root = 0;

int TspApproximate(int lineNum, int matrix[lineNum][lineNum]);
int TspExact(int lineNum, int matrix[lineNum][lineNum]);

/*
============================================
main

  A função main é responsável por ler
  o arquivo de texto com a matriz de
  adjascência, calcular o número de
  linhas, e oferecer ao usuário a opção
  de resolver o TSP utilizando um dos
  algoritmos.

============================================
*/
int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Erro! Para executar o programa, é necessário passar um arquivo de "
           "texto contendo uma matriz!\n");
    exit(EXIT_FAILURE);
  }

  int lineNum = 1;
  int aux = 0;
  int option = 0;
  char *tspFile = argv[1];
  clock_t approximateStart;
  clock_t approximateEnd;
  clock_t exactStart;
  clock_t exactEnd;

  FILE *filePtr;
  filePtr = fopen(tspFile, "r");

  if (filePtr == NULL) {
    printf("\nErro! Não foi possível abrir o arquivo!\n\n");
    exit(1);
  }

  while (!feof(filePtr)) {
    aux = fgetc(filePtr);

    if (aux == '\n') {
      lineNum++;
    }
  }

  rewind(filePtr);

  int matrix[lineNum][lineNum];

  for (int i = 0; i < lineNum; i++) {
    for (int j = 0; j < lineNum; j++) {
      if (!fscanf(filePtr, "%d", &matrix[i][j])) {
        break;
      }
    }
  }

  fclose(filePtr);

  while (1) {
    printf("\n------------------- TSP Solver -------------------\n");
    printf("\n1 - Resolver utilizando um algoritmo aproximativo\n");
    printf("2 - Resolver utilizando um algoritmo exato\n");
    printf("3 - Encerrar o programa\n");
    printf("\n--------------------------------------------------\n");
    printf("\nSelecione uma opção: ");
    scanf("%d", &option);

    switch (option) {
    case 1:
      approximateStart = clock();
      int approximateResult = TspApproximate(lineNum, matrix);
      approximateEnd = clock();
      double approximateTime =
      (double)(approximateEnd - approximateStart) / CLOCKS_PER_SEC;

      printf("Resultados - Algoritmo aproximativo:\n");
      printf("\nCusto total: %d\n", approximateResult);
      printf("Tempo de execução: %.20f segundos\n", approximateTime);
      break;

    case 2:
      exactStart = clock();
      int exactResult = TspExact(lineNum, matrix);
      exactEnd = clock();
      double exactTime = (double)(exactEnd - exactStart) / CLOCKS_PER_SEC;

      printf("Resultados - Algoritmo exato:\n");
      printf("\nCusto total: %d\n", exactResult);
      printf("Tempo de execução: %.20f segundos\n", exactTime);
      break;

    case 3:
      exit(0);
      break;

    default:
      printf("\nErro! Opção inválida!\n");
      break;
    }
  }
}

/*
============================================
========== Algoritmo Aproximativo ==========
============================================
*/

/*
============================================
Find

  Função auxiliar utilizada no funcionamento
  do algoritmo de kruskal. Encontra o
  conjunto do vértice "x".

============================================
 */
int Find(int vertex[], int x) {
  while (vertex[x] != x) {
    x = vertex[x];
  }
  return x;
}

/*
============================================
TspApproximate

  Execução de um algoritmo aproximativo
  para resolver o TSP, dado o grafo de
  entrada utilizado. O algoritmo usado
  entrada utilizado. O algoritmo usado
  é o 2 aproximado.


============================================
*/
int TspApproximate(int lineNum, int matrix[lineNum][lineNum]) {
  // 1º passo: Encontrar uma árvore geradora
  // mínima para o grafo de entrada.

  int count = 0;
  int vertex[lineNum];

  int mst[lineNum][lineNum];
  //memset(mst, 0, lineNum * lineNum * sizeof(int));
  for (int i = 0; i < lineNum; i++) {
    for (int j = 0; j < lineNum; j++) {
      mst[i][j] = 0;
    }
  }

  // Inicializa o conjunto disjunto
  for (int i = 0; i < lineNum; i++) {
    vertex[i] = i;
  }

  // Busca ( vértices - 1 ) arestas, com ordem
  // crescente de peso
  while (count < lineNum - 1) {
    int source, destination, a, b, min = inf;

    for (int i = 0; i < lineNum; i++) {
      for (int j = 0; j < lineNum; j++) {
        // Procura a menor aresta entre dois
        // vértices de conjuntos distintos
        if (Find(vertex, i) != Find(vertex, j) && matrix[i][j] != 0 &&
            matrix[i][j] < min) {
          min = matrix[i][j];
          source = i;
          destination = j;
        }
      }
    }

    // faz a união dos conjuntos representados
    // por "source" e "destination".
    a = Find(vertex, source);
    b = Find(vertex, destination);
    vertex[a] = b;

    // Cria uma matriz de adjascência para
    // representar a MST.
    mst[source][destination] = min;
    mst[destination][source] = min;

    count++;
  }

  // 2º passo: Fazer uma busca em profundidade
  // (DFS) pela MST gerada pela matriz.

  int k;
  int memory = 0;
  int cost = 0;
  int root = 0;
  int reg = -1;

  int stack[lineNum];
  //memset(stack, -1, lineNum * sizeof(int));
  for (int i = 0; i < lineNum; i++) {
    stack[i] = -1;
  }

  int visited[lineNum];
  //memset(visited, 0, lineNum * sizeof(int));
  for (int i = 0; i < lineNum; i++) {
    visited[i] = 0;
  }

  stack[++reg] = root;
  visited[root] = 1;

  while (reg >= 0) {
    int current = stack[reg--];

    for (k = 0; k < lineNum; ++k) {
      if (mst[current][k] != 0 && !visited[k]) {
        stack[++reg] = k;
        visited[k] = 1;
      }
    }

    if (current != root) {
      cost = cost + matrix[current][memory];
    }

    if (reg < 0) {
      cost = cost + matrix[current][root];
    }

    memory = current;
  }
  return cost;
}

/*
============================================
============= Algoritmo Exato ==============
============================================
*/

/*
============================================
Swap

  Função auxiliar para trocar dois elementos
  de um array.

============================================
*/
void Swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

/*
============================================
NextPermutation

  Função auxiliar para gerar permutações. A
  função next_permutation recebe um array e
  o número de elementos no array. Ela utiliza
  o algoritmo de troca de elementos para
  gerar a próxima permutação. Ela retorna 1
  se uma nova permutação foi gerada e 0 caso
  contrário.

============================================
*/
int NextPermutation(int *array, int n) {
  int i = n - 2;
  while (i >= 0 && array[i] > array[i + 1])
    i--;
  if (i < 0)
    return 0;

  int j = n - 1;
  while (array[i] > array[j])
    j--;

  Swap(&array[i], &array[j]);

  int left = i + 1;
  int right = n - 1;
  while (left < right) {
    Swap(&array[left], &array[right]);
    left++;
    right--;
  }

  return 1;
}

/*
============================================
TspExact

  Função para encontrar o caminho mais curto
  no grafo completo usando força bruta.
  Dentro da função tsp, criamos um array
  chamado vertices para armazenar os vértices
  do grafo. Inicializamos esse array com os
  valores de 0 a V-1, representando a ordem
  dos vértices.
  Dentro da função tsp, criamos um array
  chamado vertices para armazenar os vértices
  do grafo. Inicializamos esse array com os
  valores de 0 a V-1, representando a ordem
  dos vértices.

============================================
*/
int TspExact(int lineNum, int grafo[lineNum][lineNum]) {
  int root = 0;
  int vertices[lineNum];
  for (int i = 0; i < lineNum; i++)
    vertices[i] = i;

  int caminho_min = INT_MAX;
  do {
    int caminho_atual = 0;
    int k = root;
    for (int i = 0; i < lineNum; i++) {
      caminho_atual += grafo[k][vertices[i]];
      k = vertices[i];
    }
    caminho_atual += grafo[k][root];

    caminho_min = (caminho_atual < caminho_min) ? caminho_atual : caminho_min;

  } while (NextPermutation(vertices, lineNum));

  return caminho_min;
}
