import os
import pandas as pd
import matplotlib.pyplot as plt

def plotallocation(files, memory):
  # Definindo o caminho para o arquivo CSV
  index = 0
  colors = ["blue", "green", "yellow", "red"]
  for file in files:
    index += 1
    pasta = "results"
    arquivo = file
    caminho = os.path.join(pasta, arquivo)

    # Verifica se o arquivo existe
    if not os.path.exists(caminho):
        print(f'O arquivo {caminho} não existe.')
        exit()

    # Carrega os dados do CSV usando pandas
    dados = pd.read_csv(caminho, header=None)

    # Cria um vetor de tempo baseado no número de linhas no CSV
    tempo = range(1, len(dados) + 1)

    # Define as colunas como áreas no gráfico
    plt.fill_between(tempo, dados[0], dados[0] + dados[1], label=f'Memory {index}', alpha=0.5)
    # plt.fill_between(tempo, dados[0], alpha=0)

    # Adiciona rótulos ao gráfico
    plt.xlabel('Tempo (t)')
    plt.ylabel('Área')
    plt.ylim(0, memory)
    plt.title('Gráfico ')
    plt.legend()
  # Mostra o gráfico
  plt.show()

def plotpointers(files, memory):
  # Definindo o caminho para o arquivo CSV
  index = 0
  for file in files:
    index += 1
    pasta = "results"
    arquivo = file
    caminho = os.path.join(pasta, arquivo)
    
    # Verifica se o arquivo existe
    if not os.path.exists(caminho):
        print(f'O arquivo {caminho} não existe.')
        exit()

    # Carrega os dados do CSV usando pandas
    data = pd.read_csv(caminho, header=None)

    # Cria um vetor de tempo baseado no número de linhas no CSV
    tempo = range(1, len(data) + 1)
    numOP = int(data.shape[1]/3)
    # Define as colunas como áreas no gráfico
    for i in range(numOP):
      if i == 0:
        name = "Overlay"
      else:
        name = i - 1
      plt.fill_between(tempo,data[i*3 + 1], data[i*3 + 1] + data[i*3 + 2], label=f'Memory {index}, ID: {name}', alpha=0.5)

    # Adiciona rótulos ao gráfico
    plt.xlabel('Tempo (t)')
    plt.ylabel('Área')
    plt.ylim(0, memory)
    plt.title('Gráfico ')
    plt.legend()

  # Mostra o gráfico
  plt.show()

filesA = ["MenTask1.csv","MenTask2.csv","MenTask3.csv","MenTask4.csv"]
filesP = ["SegmentosTask1.csv","SegmentosTask2.csv","SegmentosTask3.csv","SegmentosTask4.csv",]
plotallocation(filesA, 300)
plotpointers(filesP, 300)