/**********************************************************************
 * Algoritmo de treinamento back-propagation para redes multicamadas
**********************************************************************/

/************************* BIBLIOTECAS *******************************/
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// Caso o SO seja windows
#if defined(__MINGW32__) || defined(_MSC_VER) || defined(_WIN32) || defined(WIN32)
#define limpar_input() fflush(stdin)
#define limpar_tela() system("cls")
#include <windows.h>
// Caso o SO seja Linux
#else
#define limpar_input() __fpurge(stdin)
#define limpar_tela() system("clear")
#endif

/************************* DEFINICOES ********************************/
#define MAXCAM 5   // N�mero m�ximo de camadas
#define MAXNEU 100 // N�mero m�ximo de neur�nios
#define MAXPES 100 // N�mero m�ximo de pesos
#define MAXLIN 100 // N�mero m�ximo de linhas
#define MAXCOL 20  // N�mero m�ximo de colunas

#define NUMLIN 26   // N�mero de Linhas da Matriz de Entrada
#define NUMCOLENT 8 // N�mero de Colunas da Matriz de Entrada
#define NUMCOLSAI 5 // N�mero de Colunas de Saida
#define NUMITE 1    // Numero de Iteracoes
#define ESC 27

#define MI 0.6
#define TOLERANCIA 0.00001 // N�mero de erros consecutivos

using namespace std;

double BETA = MI;          // Fator de ajuste das correcoes
void gotoxy(int x, int y); // Implementando gotoxy() manualmente

/************************* CLASSES **********************************/
/*********************** CLASSE NEURONIO ****************************/
class Neuronio
{
private:
  int numPesos; // N�mero de pesos do neur�nio
  double Pesos[MAXPES]; // Vetor de pesos

public:
  void Inicializar_Neuronio(int num_pesos);                     // Inicia os valores dos pesos
  void Ajustar_Peso(double entrada, double erro, int indice_peso); // Ajusta os valores dos pesos
  double Somatorio(double Entrada[]);                              // Retorna os pesos e quantos s�o
  double Erro_Peso(double erro, int indice_peso);
};

/*********************************************************
  Inicializa o N�mero de Pesos e tamb�m os valores
  iniciais dos pesos
*********************************************************/
void Neuronio ::Inicializar_Neuronio(int num_pesos)
{
  int i, p, q;

  this->numPesos = num_pesos;

  srand(time(NULL));
  for (i = 0; i < numPesos; i++)
  {
    p = rand() % 11 / 10.0;
    q = rand() % 3 - 1;
    Pesos[i] = (double)(p * q);
  }
}

/*********************************************************
  Multilica o Erro da sa�da de um neur�nio por um Peso de
  Indice_Peso
*********************************************************/
double Neuronio ::Erro_Peso(double erro, int indice_peso)
{
  return (erro * Pesos[indice_peso]);
}

/*********************************************************
  Dada uma entrada, retorna a sa�da do neur�nio
  multiplicando-a pelos pesos
*********************************************************/
double Neuronio ::Somatorio(double Entrada[])
{
  int i;
  double soma = 0;

  for (i = 0; i < numPesos; i++)
    soma += Entrada[i] * Pesos[i];

  return soma;
}

/*********************************************************
  Dado o erro da camada da frente, a sa�da do neur�nio,
  e Indice do Peso, calcula-se o novo peso
*********************************************************/
void Neuronio ::Ajustar_Peso(double entrada, double erro, int indice_peso)
{
  Pesos[indice_peso] += BETA * erro * entrada;
}

/*********************** CLASSE CAMADA ****************************/
class Camada
{
private:
  int numNeuronios; // N�mero de neur�nios na camada
  int numPesos;
  double Saidas[MAXNEU]; // Sa�da dos neur�nios da camada
  Neuronio Neuronios[MAXNEU];   // Neur�nios da camada

public:
  void Inicializar_Camada(int num_neuronios, int num_pesos); // Atribui o n�mero de neur�nios
  void Treinar_Neuronios(double Entrada[]);                        // Treina os neur�nios com uma entrada
  void Funcao_Ativacao();                                          // Joga sa�da linear na funcao de ativacao
  void Retornar_Saida(double Linha[]);                             // Retorna a sa�da dos neur�nios
  void Ajustar_Pesos_Neuronios(double Erros[], double Entrada[]);
  void Calcular_Erro_Camada_Saida(double Erros[], double Y[]);
  void Calcular_Erro_Camada(double Erros[]);
  double Somatorio_Erro(double Erros[]);
  void Calcular_Erro_Final(double Erros[], double Y[]);
};

/*********************************************************
  Inicializa o Numero de Neuronios e o Numero de Pesos
  e invoca a inicializa��o dos neur�nios
*********************************************************/
void Camada ::Inicializar_Camada(int num_neuronios, int num_pesos)
{
  int i;

  this->numNeuronios = num_neuronios;
  this->numPesos = num_pesos;

  for (i = 0; i < numNeuronios; i++)
    Neuronios[i].Inicializar_Neuronio(numPesos);
}

/*********************************************************
  Calcula os erros da camada de sa�da com base na sa�da
  desejada Y, retornando os erros
*********************************************************/
void Camada ::Calcular_Erro_Final(double Erros[], double Y[])
{
  int i;

  for (i = 0; i < numNeuronios; i++)
    Erros[i] = (Y[i] - Saidas[i]);
}

/*********************************************************
  Dispara o somat�rio de um neur�nio para uma certa entrada
  armazenando a sua sa�da
*********************************************************/
void Camada ::Treinar_Neuronios(double Entrada[])
{
  int i;

  for (i = 0; i < numNeuronios; i++)
    Saidas[i] = Neuronios[i].Somatorio(Entrada);
}

/*********************************************************
  Calcula os erros da camada de sa�da com base na sa�da
  desejada Y, retornando os erros
*********************************************************/
void Camada ::Calcular_Erro_Camada_Saida(double Erros[], double Y[])
{
  int i;

  for (i = 0; i < numNeuronios; i++)
    Erros[i] = (Y[i] - Saidas[i]) * Saidas[i] * (1 - Saidas[i]);
}

/*********************************************************
  Calcula os erros da camada escondida com base no erro
  da camada da frente, retornando os erros
*********************************************************/
void Camada ::Calcular_Erro_Camada(double Erros[])
{
  int i;

  for (i = 0; i < numNeuronios; i++)
    Erros[i] = Saidas[i] * (1 - Saidas[i]) * Erros[i];
}

/*********************************************************
  Ajusta os pesos dos neur�nios da camada de acordo com os
  erros da camada da frente, e retorna o som�r�rio de erros
  da pr�pria camada
*********************************************************/
void Camada ::Ajustar_Pesos_Neuronios(double Erros[], double Entrada[])
{
  int i, j;
  double Temp, Erro_Aux[MAXNEU];

  /* C�lculo do Somat�rio que ser� usado para o c�lculo do erro
     da camada anterior */

  for (i = 1; i < numPesos; i++)
  {
    Temp = 0;
    for (j = 0; j < numNeuronios; j++)
    {
      Temp += Neuronios[j].Erro_Peso(Erros[j], i);
    }
    Erro_Aux[i - 1] = Temp;
  }

  /* Ajusta os pesos de cada neur�nio  de acordo com o erro
     da camada da frente e a sa�da da pr�pria camada */

  for (i = 0; i < numNeuronios; i++)
    for (j = 0; j < numPesos; j++)
      Neuronios[i].Ajustar_Peso(Entrada[j], Erros[i], j);

  /* Atribui o vetor de erros calculado, para o vetor erro
     que ser� retornado */

  for (i = 0; i < (numPesos - 1); i++)
    Erros[i] = Erro_Aux[i];
}

/*********************************************************
  Atualiza a sa�da da camada passando-a por uma fun��o
  de ativa��o
*********************************************************/
void Camada ::Funcao_Ativacao()
{
  int i;

  for (i = 0; i < numNeuronios; i++)
    Saidas[i] = 1 / (1 + exp(-Saidas[i]));
}

/*********************************************************
  Retorna a Sa�da da Camada
*********************************************************/
void Camada ::Retornar_Saida(double Linha[])
{
  int i;

  Linha[0] = 1;
  for (i = 1; i <= numNeuronios; i++)
    Linha[i] = Saidas[i - 1];
}

/*********************** CLASSE REDE ******************************/
class Rede
{
private:
  int numCamadas;         // N�mero de camadas da rede
  int numLinhas;          // N�mero de linhas de entrada
  int numColunasEntrada; // N�mero de colunas de entrada
  int numColunasSaida;   // N�mero de colunas da sa�da

  Camada Camadas[MAXCAM];         // Camadas da rede
  double X[MAXLIN][MAXCOL]; // Matriz de entrada da rede
  double Y[MAXLIN][MAXCOL]; // Matriz de sa�da da rede

public:
  void Inicializar_Rede(int, int, int Num_Neuronios_Camada[]); // Inicializa os valores das vari�veis
  void Treinar();                                                          // Treina toda a rede
  void Calcular_Resultado(double Entrada[], double Saida[]);
};

/*********************************************************
  Inicializa todas as vari�veis da rede, inclusive a
  leitura das entradas e sa�das da rede
*********************************************************/
void Rede ::Inicializar_Rede(int num_colunas_entrada, int num_colunas_saida, int Num_Neuronios_Camada[])
{
  int i, j;
  FILE *Entrada, *Saida;

  this->numCamadas = 4;
  this->numLinhas = 26;
  this->numColunasEntrada = num_colunas_entrada;
  this->numColunasSaida = num_colunas_saida;

  Entrada = fopen("X.txt", "rb");
  Saida = fopen("Y.txt", "rb");

  for (i = 0; i < numLinhas; i++)
    for (j = 0; j < numColunasEntrada; j++)
      fread(&X[i][j], sizeof(double), 1, Entrada);

  for (i = 0; i < numLinhas; i++)
    for (j = 0; j < numColunasSaida; j++)
      fread(&Y[i][j], sizeof(double), 1, Saida);

  fclose(Entrada);
  fclose(Saida);

  int numEntradasRede = 2;
  int numEntradasCamadaOculta = 5;

  // Inicializar Camada: numNeuroniosCamada, numPesosPorNeuronio...

  Camadas[0].Inicializar_Camada(numEntradasRede, numEntradasRede);

  Camadas[1].Inicializar_Camada(numEntradasCamadaOculta, numColunasEntrada+1);

  for (i = 2; i < numCamadas-1; i++)
    Camadas[i].Inicializar_Camada(numEntradasCamadaOculta, numEntradasCamadaOculta+1);

  Camadas[i].Inicializar_Camada(1, numEntradasCamadaOculta+1);
}

/*********************************************************
  Calcula a resposta da rede para uma certa entrada,
  retornando a sa�da
*********************************************************/
void Rede ::Calcular_Resultado(double Entrada[], double Saida[])
{
  int i, j;

  for (i = 0; i < numCamadas; i++)
  {
    Camadas[i].Treinar_Neuronios(Entrada);
    Camadas[i].Funcao_Ativacao();
    Camadas[i].Retornar_Saida(Saida);

    for (j = 0; j < MAXNEU; j++)
      Entrada[j] = Saida[j];
  }
}

/*********************************************************
  Algoritmmo de Treinamento Back Propagation
*********************************************************/
void Rede ::Treinar()
{
  int i, j, Linha_Escolhida, Iteracoes, Camada_Saida, Marcados[MAXLIN];
  int p, q;
  double Vetor_Saida[MAXNEU], Erros[MAXNEU], Somatorio_Erro, Maior;
  long Contador, Dinamico;
  char Sair;

  /* Inicializando vari�veis */
  for (i = 0; i < MAXLIN; i++)
    Marcados[i] = 0;

  Dinamico = 0;
  Sair = 0;
  Contador = 0;
  Maior = 1;
  Iteracoes = 0;
  Camada_Saida = numCamadas - 1;

  do
  {
    Linha_Escolhida = rand() % NUMLIN;

    j = 0;
    while (Marcados[Linha_Escolhida] == 1)
    {
      Linha_Escolhida++;
      j++;

      if (Linha_Escolhida == NUMLIN)
        Linha_Escolhida = 0;

      if (j == NUMLIN)
        for (i = 0; i < MAXLIN; i++)
          Marcados[i] = 0;
    }

    Marcados[Linha_Escolhida] = 1;
    Contador++;

    // FEED-FORWARD
    Camadas[0].Treinar_Neuronios(X[Linha_Escolhida]);
    Camadas[0].Funcao_Ativacao();
    Camadas[0].Retornar_Saida(Vetor_Saida);

    for (i = 1; i < numCamadas; i++)
    {
      Camadas[i].Treinar_Neuronios(Vetor_Saida);
      Camadas[i].Funcao_Ativacao();
      Camadas[i].Retornar_Saida(Vetor_Saida);
    }

    // BACK-PROPAGATION
    /* Ajustando pesos da camada de sa�da */
    Camadas[Camada_Saida].Calcular_Erro_Camada_Saida(Erros, Y[Linha_Escolhida]);
    Camadas[Camada_Saida - 1].Retornar_Saida(Vetor_Saida);
    Camadas[Camada_Saida].Ajustar_Pesos_Neuronios(Erros, Vetor_Saida);

    /* Ajustando pesos das camadas intermedi�rias */
    for (i = Camada_Saida - 1; i > 0; i--)
    {
      Camadas[i].Calcular_Erro_Camada(Erros);
      Camadas[i - 1].Retornar_Saida(Vetor_Saida);
      Camadas[i].Ajustar_Pesos_Neuronios(Erros, X[Linha_Escolhida]);
    }

    /* Ajustando pesos da primeira camada */
    Camadas[0].Calcular_Erro_Camada(Erros);
    Camadas[0].Ajustar_Pesos_Neuronios(Erros, X[Linha_Escolhida]);

    /* Calculando o erro global */
    Camadas[Camada_Saida].Calcular_Erro_Final(Erros, Y[Linha_Escolhida]);

    Somatorio_Erro = 0;
    for (i = 0; i < numColunasSaida; i++)
      Somatorio_Erro += pow(Erros[i], 2);

    Somatorio_Erro /= 2;

    /* Verificando condi��es */
    if (Somatorio_Erro < Maior)
    {
      Dinamico = 0;
      gotoxy(1, 10);
      cout << "\n\nErro = " << Somatorio_Erro << "   ";
      Maior = Somatorio_Erro;
    }
    else
      Dinamico++;

    if (Somatorio_Erro <= TOLERANCIA)
      Iteracoes++;
    else
      Iteracoes = 0;

    /* Beta din�mico */
    if (Dinamico == 200000)
    {
      Dinamico = 0;
      p = rand() % 6;
      q = rand() % 3;
      BETA += (p / 10.0) * (q - 1);
    }

    if (Dinamico == 50000)
      BETA = MI;

    /* Exibi��o na tela */
    if (Contador % 10000 == 0)
    {
      gotoxy(1, 10);
      cout << "\nIteracoes = " << Contador;
      cout << "\n\nBeta = " << BETA << "  ";
    }

    /* Op��o de escape */
    if (Contador % 10000000 == 0)
    {
      cout << "\nRede treinada!" << endl;
	  cout << "Pressione 'y' para sair do treinamento ou qualquer outro botão para continuar. " << endl; 
	  cin.clear();
	  cin.ignore(256, '\n');	  
      Sair = cin.get();
    }

  } while (Iteracoes < NUMITE && Sair != 'y');
}

/****************** PROGRAMA PRINCIPAL *****************************/
void gotoxy(int x, int y)
{
  printf("%c[%d;%df", 0x1B, y, x);
}

int main()
{
  int Numero_Camadas;         // N�mero de camadas da rede
  int Numero_Linhas;          // N�mero de linhas de entrada
  int Numero_Colunas_Entrada; // N�mero de colunas de entrada
  int Numero_Colunas_Saida;   // N�mero de colunas da sa�da
  int Numero_Neuronio_Camada[MAXCAM];
  int i;

  double Entrada[MAXNEU];
  double Saida[MAXNEU];

  char Continua = 'r';
  Rede R;

  Numero_Linhas = NUMLIN;
  Numero_Colunas_Entrada = NUMCOLENT;
  Numero_Colunas_Saida = NUMCOLSAI;

  while (Continua != 'n')
  {
    limpar_tela();

    if (Continua == 'r')
    {
      R.Inicializar_Rede(
          Numero_Colunas_Entrada,
          Numero_Colunas_Saida,
          Numero_Neuronio_Camada);
      R.Treinar();
    }

    cout << "\n\nDigite as entradas da rede:\n";

    for (i = 0; i < Numero_Colunas_Entrada; i++)
    {
      cout << "\nEntrada " << i << " : ";
      cin >> Entrada[i];
    }

    R.Calcular_Resultado(Entrada, Saida);

    for (i = 1; i <= Numero_Colunas_Saida; i++)
    {
      cout << "\nSaida " << i << " : " << Saida[i];
    }

    cout << "\n\nContinua ? (s/n/r)";
    cin >> Continua;
  }

  return 0;
}
