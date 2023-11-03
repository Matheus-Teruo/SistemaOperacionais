import os
import pandas as pd
import matplotlib.pyplot as plt

def plotallocation(files, memory):
  # Definindo o caminho para o arquivo CSV
  index = 0
  colors = ["blue", "green", "orange", "red"]
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
    plt.fill_between(tempo, dados[0], dados[0] + dados[1], label=f'Memory {index}', color=colors[index-1], alpha=0.5)
    # plt.fill_between(tempo, dados[0], alpha=0)

    # Adiciona rótulos ao gráfico
    plt.xlabel('Tempo (t)')
    plt.ylabel('Memória Mb')
    plt.ylim(0, memory)
    plt.title('Alocação de Task na Memória')
    plt.legend()
  # Mostra o gráfico
  plt.show()

def plotpointers(files, memory):
  # Definindo o caminho para o arquivo CSV
  index = 0
  colors = [["#0000FF", "#005AFF", "#0089FF"],
            ["#00FF00", "#29E229", "#29C029", "#3FA446", "#38823E", "#2F6E34"],
            ["#FF7300", "#DB6300", "#C45800", "#A84C00", "#9C4600"],
            ["#FF0000", "#FF4444", "#FF6969", "#FF8B8D"]]
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
      plt.fill_between(tempo,data[i*3 + 1], data[i*3 + 1] + data[i*3 + 2], color=colors[index-1][i], label=f'Memory {index}, ID: {name}', alpha=0.5)

    # Adiciona rótulos ao gráfico
    plt.xlabel('Tempo (t)')
    plt.ylabel('Memória Mb')
    plt.ylim(0, memory)
    plt.title('Alocação de Pointer na Memória')
    plt.legend()

  # Mostra o gráfico
  plt.show()

def plotDevices(files):
  index = 0
  colors = ["blue", "green", "orange", "red"]
  labels = []

  for file in files:
    index += 1
    pasta = "results"
    arquivo = file
    caminho = os.path.join(pasta, arquivo)

    if not os.path.exists(caminho):
      print(f'O arquivo {caminho} não existe.')

    try:
      dados = pd.read_csv(caminho, header=None)
    except pd.errors.EmptyDataError:
      # print(f'O arquivo {caminho} está vazio.')
      labels.append(f'Task{index} Device1')
      labels.append(f'Task{index} Device2')
      plt.axhline((index-1)*2+1.5, color=colors[index-1], linewidth=0.5)
      continue

    # Extract start and end times for each device
    device1_start_times = dados.loc[dados[0] == 'device1Start', 1].values
    device1_end_times = dados.loc[dados[0] == 'device1End', 1].values
    device2_start_times = dados.loc[dados[0] == 'device2Start', 1].values
    device2_end_times = dados.loc[dados[0] == 'device2End', 1].values

    # Plot broken barh for device 1
    for i, (start, end) in enumerate(zip(device1_start_times, device1_end_times), 1):
      plt.broken_barh([(start, end-start)], ((index-1)*2+0.5, 1), color=colors[index-1])
    labels.append(f'Task{index} Device1')

    # Plot broken barh for device 2
    for i, (start, end) in enumerate(zip(device2_start_times, device2_end_times), 1):
      plt.broken_barh([(start, end-start)], ((index-1)*2+1.5, 1), color=colors[index-1])
    labels.append(f'Task{index} Device2')
    # Add a thin line between the devices
    plt.axhline((index-1)*2+1.5, color=colors[index-1], linewidth=0.5)

  plt.xlabel('Tempo (t)')
  plt.yticks(range(1, len(labels)+1,), labels)  # Adjusted y-axis labels for two bars per file
  plt.ylabel('File Name')
  plt.ylim(0.5, len(labels) + 0.5)  # Adjusted y-axis limits for two bars per file
  plt.title('Gráfico')

  plt.show()

def plotCPU(files):
  # Definindo o caminho para o arquivo CSV
    index = 0
    colors = ["blue", "green", "orange", "red"]
    file_names = [os.path.splitext(file)[0] for file in files]  # Extract file names without extensions
    
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

        # Extract start and end times
        start_times = dados.loc[dados[0] == 'cpuStart', 1].values
        end_times = dados.loc[dados[0] == 'cpuEnd', 1].values

        # Extract arrival time
        arrival_time = dados.loc[dados[0] == 'arrival', 1].values[0]

        # Plot broken barh
        for i, (start, end) in enumerate(zip(start_times, end_times), 1):
            plt.broken_barh([(start, end-start)], (index-0.5, 1), color=colors[index-1])
        
        # Mark arrival with a star
        plt.plot(arrival_time, index, marker='*', markersize=10, color='black')

    # Adiciona rótulos ao gráfico
    plt.xlabel('Tempo (t)')
    plt.yticks(range(1, index+1), file_names)  # Set y-axis labels to file names
    plt.ylabel('File Name')
    plt.ylim(0.5, len(files) + 0.5)  # Adjusted y-axis limits
    plt.title('Gráfico')

    # Mostra o gráfico
    plt.show()

filesA = ["MenTask1.csv","MenTask2.csv","MenTask3.csv","MenTask4.csv"]
filesP = ["SegmentosTask1.csv","SegmentosTask2.csv","SegmentosTask3.csv","SegmentosTask4.csv"]
filesD = ["Devices1.csv","Devices2.csv","Devices3.csv","Devices4.csv"]
filesC = ["CPUTask1.csv","CPUTask2.csv","CPUTask3.csv","CPUTask4.csv"]
filesT = ["Task1.csv","Task2.csv","Task3.csv","Task4.csv"]
plotallocation(filesA, 300)
plotpointers(filesP, 300)
plotDevices(filesD)
plotCPU(filesC)
plotCPU(filesT)