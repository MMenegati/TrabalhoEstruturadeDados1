# TrabalhoEstruturadeDados1

# KD-Tree API com FastAPI

Este projeto implementa uma KD-Tree para busca eficiente de vizinhos próximos em dados multidimensionais (embeddings) e expõe essa funcionalidade através de uma API web construída com FastAPI.

A lógica principal da KD-Tree é implementada em C ([kdtree.c](kdtree.c)) e acessada pelo Python ([app.py](app.py)) através de um wrapper ([kdtree_wrapper.py](kdtree_wrapper.py)) que utiliza `ctypes` para interagir com a biblioteca C compilada (`libkdtree.dll`).

## Funcionalidades

*   **Inserir Ponto**: Adiciona um novo ponto (com um ID e um vetor de embedding) à KD-Tree.
*   **Buscar Ponto Mais Próximo**: Dado um ponto de consulta, encontra o ponto mais similar na KD-Tree.
*   **Buscar N Vizinhos Mais Próximos**: Dado um ponto de consulta, encontra os N pontos mais similares na KD-Tree.

## Pré-requisitos

1.  **Python**: Versão 3.7 ou superior. Você pode baixá-lo em [python.org](https://www.python.org/).
2.  **Compilador C (GCC/MinGW)**: Necessário se você precisar recompilar o arquivo `kdtree.c` para gerar `libkdtree.dll`. Se o arquivo `libkdtree.dll` já estiver presente e funcionando no seu sistema, você pode pular a compilação.
    *   Para Windows, uma opção popular é o MinGW-w64 (que pode ser instalado via MSYS2).
3.  **PIP**: O gerenciador de pacotes Python, geralmente instalado junto com o Python.

## Configuração e Instalação

### 1. Biblioteca C (`libkdtree.dll`)

O projeto espera que o arquivo `libkdtree.dll` (a biblioteca C compilada) esteja no mesmo diretório que os arquivos Python.

*   **Se `libkdtree.dll` já existe**: Você pode tentar pular para o passo 2.
*   **Se `libkdtree.dll` precisa ser compilado**:
    1.  Abra um terminal (Prompt de Comando, PowerShell, ou Git Bash no Windows; Terminal no Linux/macOS).
    2.  Navegue até o diretório onde o arquivo `kdtree.c` está localizado.
    3.  Compile o código C para gerar a DLL. Se você tem GCC (MinGW no Windows) configurado no seu PATH, o comando é:
        ```sh
        gcc -shared -o libkdtree.dll kdtree.c -lm
        ```
        Este comando cria o arquivo `libkdtree.dll` a partir de `kdtree.c`. A flag `-lm` é para linkar a biblioteca matemática, caso seja necessária por `math.h`.

### 2. Dependências Python

1.  Abra seu terminal no diretório raiz do projeto.
2.  É uma boa prática criar um ambiente virtual para isolar as dependências do projeto:
    ```sh
    python -m venv venv
    ```
3.  Ative o ambiente virtual:
    *   No Windows:
        ```sh
        venv\Scripts\activate
        ```
    *   No macOS e Linux:
        ```sh
        source venv/bin/activate
        ```
4.  Instale as bibliotecas Python necessárias:
    ```sh
    pip install fastapi uvicorn[standard] pydantic
    ```
    *   `fastapi`: Para construir a API.
    *   `uvicorn`: Para rodar a API (servidor ASGI).
    *   `pydantic`: Para validação de dados.

## Executando a Aplicação

1.  Certifique-se de que `libkdtree.dll` está no mesmo diretório que `app.py`.
2.  No seu terminal (com o ambiente virtual ativado, se você criou um), navegue até o diretório do projeto.
3.  Inicie o servidor da API com Uvicorn:
    ```sh
    uvicorn app:app --reload
    ```
    *   `app:app` significa: no arquivo `app.py`, use a instância `app` do FastAPI.
    *   `--reload` faz o servidor reiniciar automaticamente se você modificar os arquivos do projeto (útil para desenvolvimento).

4.  Você deverá ver uma saída no terminal indicando que o servidor está rodando, algo como:
    ```
    INFO:     Uvicorn running on http://127.0.0.1:8000 (Press CTRL+C to quit)
    INFO:     KD-Tree initialized on startup.
    ```

## Utilizando a API

Uma vez que o servidor está rodando, você pode interagir com a API. A maneira mais fácil é através da documentação interativa (Swagger UI) que o FastAPI gera automaticamente.

1.  Abra seu navegador de internet.
2.  Acesse o endereço: `http://127.0.0.1:8000/docs`

Lá você verá todos os endpoints disponíveis e poderá testá-los diretamente do navegador.

### Endpoints Principais:

*   **`POST /inserir`**:
    *   **Descrição**: Insere um novo ponto na KD-Tree.
    *   **Corpo da Requisição (Exemplo JSON)**:
        ```json
        {
          "embedding": [0.1, 0.2, 0.3, ..., (lista de 128 números)],
          "id": "identificador_do_ponto"
        }
        ```
    *   **Resposta**: Uma mensagem de sucesso.

*   **`POST /buscar`**:
    *   **Descrição**: Busca o ponto mais próximo ao ponto fornecido na consulta.
    *   **Corpo da Requisição (Exemplo JSON)**:
        ```json
        {
          "embedding": [0.11, 0.22, 0.33, ..., (lista de 128 números)],
          "id": "ponto_de_consulta"
        }
        ```
    *   **Resposta**: O ponto mais próximo encontrado, no mesmo formato JSON.

*   **`POST /buscar-n`**:
    *   **Descrição**: Busca os `n` vizinhos mais próximos ao ponto fornecido.
    *   **Parâmetro de URL**: `n` (um número inteiro, ex: `/buscar-n?n=5`)
    *   **Corpo da Requisição (Exemplo JSON)**:
        ```json
        {
          "embedding": [0.11, 0.22, 0.33, ..., (lista de 128 números)],
          "id": "ponto_de_consulta_n"
        }
        ```
    *   **Resposta**: Uma lista dos `n` pontos mais próximos encontrados.

**Nota sobre Embeddings**: O sistema está configurado para embeddings de `128` dimensões (definido por `K_DIMENSIONS` em [kdtree.c](kdtree.c) e [kdtree_wrapper.py](kdtree_wrapper.py)). Certifique-se de que suas listas de `embedding` sempre contenham 128 números de ponto flutuante.

O arquivo `a.exe` não é diretamente utilizado pela aplicação Python/FastAPI, mas pode ser um executável de teste ou um compilador auxiliar para o código C.

# KD-Tree API com FastAPI

Este projeto implementa uma KD-Tree para busca eficiente de vizinhos próximos em dados multidimensionais (embeddings) e expõe essa funcionalidade através de uma API web construída com FastAPI.

A lógica principal da KD-Tree é implementada em C ([kdtree.c](kdtree.c)) e acessada pelo Python ([app.py](app.py)) através de um wrapper ([kdtree_wrapper.py](kdtree_wrapper.py)) que utiliza `ctypes` para interagir com a biblioteca C compilada (`libkdtree.dll`).

## Funcionalidades

*   **Inserir Ponto**: Adiciona um novo ponto (com um ID e um vetor de embedding) à KD-Tree.
*   **Buscar Ponto Mais Próximo**: Dado um ponto de consulta, encontra o ponto mais similar na KD-Tree.
*   **Buscar N Vizinhos Mais Próximos**: Dado um ponto de consulta, encontra os N pontos mais similares na KD-Tree.

## Pré-requisitos

1.  **Python**: Versão 3.7 ou superior. Você pode baixá-lo em [python.org](https://www.python.org/).
2.  **Compilador C (GCC/MinGW)**: Necessário se você precisar recompilar o arquivo `kdtree.c` para gerar `libkdtree.dll`. Se o arquivo `libkdtree.dll` já estiver presente e funcionando no seu sistema, você pode pular a compilação.
    *   Para Windows, uma opção popular é o MinGW-w64 (que pode ser instalado via MSYS2).
3.  **PIP**: O gerenciador de pacotes Python, geralmente instalado junto com o Python.

## Configuração e Instalação

### 1. Biblioteca C (`libkdtree.dll`)

O projeto espera que o arquivo `libkdtree.dll` (a biblioteca C compilada) esteja no mesmo diretório que os arquivos Python.

*   **Se `libkdtree.dll` já existe**: Você pode tentar pular para o passo 2.
*   **Se `libkdtree.dll` precisa ser compilado**:
    1.  Abra um terminal (Prompt de Comando, PowerShell, ou Git Bash no Windows; Terminal no Linux/macOS).
    2.  Navegue até o diretório onde o arquivo `kdtree.c` está localizado.
    3.  Compile o código C para gerar a DLL. Se você tem GCC (MinGW no Windows) configurado no seu PATH, o comando é:
        ```sh
        gcc -shared -o libkdtree.dll kdtree.c -lm
        ```
        Este comando cria o arquivo `libkdtree.dll` a partir de `kdtree.c`. A flag `-lm` é para linkar a biblioteca matemática, caso seja necessária por `math.h`.

### 2. Dependências Python

1.  Abra seu terminal no diretório raiz do projeto.
2.  É uma boa prática criar um ambiente virtual para isolar as dependências do projeto:
    ```sh
    python -m venv venv
    ```
3.  Ative o ambiente virtual:
    *   No Windows:
        ```sh
        venv\Scripts\activate
        ```
    *   No macOS e Linux:
        ```sh
        source venv/bin/activate
        ```
4.  Instale as bibliotecas Python necessárias:
    ```sh
    pip install fastapi uvicorn[standard] pydantic
    ```
    *   `fastapi`: Para construir a API.
    *   `uvicorn`: Para rodar a API (servidor ASGI).
    *   `pydantic`: Para validação de dados.

## Executando a Aplicação

1.  Certifique-se de que `libkdtree.dll` está no mesmo diretório que `app.py`.
2.  No seu terminal (com o ambiente virtual ativado, se você criou um), navegue até o diretório do projeto.
3.  Inicie o servidor da API com Uvicorn:
    ```sh
    uvicorn app:app --reload
    ```
    *   `app:app` significa: no arquivo `app.py`, use a instância `app` do FastAPI.
    *   `--reload` faz o servidor reiniciar automaticamente se você modificar os arquivos do projeto (útil para desenvolvimento).

4.  Você deverá ver uma saída no terminal indicando que o servidor está rodando, algo como:
    ```
    INFO:     Uvicorn running on http://127.0.0.1:8000 (Press CTRL+C to quit)
    INFO:     KD-Tree initialized on startup.
    ```

## Utilizando a API

Uma vez que o servidor está rodando, você pode interagir com a API. A maneira mais fácil é através da documentação interativa (Swagger UI) que o FastAPI gera automaticamente.

1.  Abra seu navegador de internet.
2.  Acesse o endereço: `http://127.0.0.1:8000/docs`

Lá você verá todos os endpoints disponíveis e poderá testá-los diretamente do navegador.

### Endpoints Principais:

*   **`POST /inserir`**:
    *   **Descrição**: Insere um novo ponto na KD-Tree.
    *   **Corpo da Requisição (Exemplo JSON)**:
        ```json
        {
          "embedding": [0.1, 0.2, 0.3, ..., (lista de 128 números)],
          "id": "identificador_do_ponto"
        }
        ```
    *   **Resposta**: Uma mensagem de sucesso.

*   **`POST /buscar`**:
    *   **Descrição**: Busca o ponto mais próximo ao ponto fornecido na consulta.
    *   **Corpo da Requisição (Exemplo JSON)**:
        ```json
        {
          "embedding": [0.11, 0.22, 0.33, ..., (lista de 128 números)],
          "id": "ponto_de_consulta"
        }
        ```
    *   **Resposta**: O ponto mais próximo encontrado, no mesmo formato JSON.

*   **`POST /buscar-n`**:
    *   **Descrição**: Busca os `n` vizinhos mais próximos ao ponto fornecido.
    *   **Parâmetro de URL**: `n` (um número inteiro, ex: `/buscar-n?n=5`)
    *   **Corpo da Requisição (Exemplo JSON)**:
        ```json
        {
          "embedding": [0.11, 0.22, 0.33, ..., (lista de 128 números)],
          "id": "ponto_de_consulta_n"
        }
        ```
    *   **Resposta**: Uma lista dos `n` pontos mais próximos encontrados.

**Nota sobre Embeddings**: O sistema está configurado para embeddings de `128` dimensões (definido por `K_DIMENSIONS` em [kdtree.c](kdtree.c) e [kdtree_wrapper.py](kdtree_wrapper.py)). Certifique-se de que suas listas de `embedding` sempre contenham 128 números de ponto flutuante.

O arquivo `a.exe` não é diretamente utilizado pela aplicação Python/FastAPI, mas pode ser um executável de teste ou um compilador auxiliar para o código C.
