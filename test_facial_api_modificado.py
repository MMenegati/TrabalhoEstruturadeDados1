'''
Cliente para testar a API de reconhecimento facial.
'''
import requests
import json
import os

API_BASE_URL = "http://127.0.0.1:8000"

# Ajusta o caminho para o arquivo JSON de embeddings
# Assumindo que este script está na raiz do workspace d:\\UF\\5-Semestre\\ED\\TrabalhoEstruturadeDados1-main
ARQUIVO_EMBEDDINGS = os.path.join("facial_embeddings_data.json")

# Modifica os IDs para teste conforme solicitado
IDS_PARA_TESTE = ["Eu", "Convidado1", "Convidado2"]

# A função construir_arvore_api() foi removida pois a árvore é construída no startup do servidor FastAPI.

def inserir_api(embedding_lista, id_pessoa):
    payload = {"embedding": embedding_lista, "id_pessoa": id_pessoa} # Mudado de 'id' para 'id_pessoa' para corresponder ao que o servidor espera no payload, se necessário, ou manter 'id' se PontoEntrada no servidor usa 'id'
    # Verificando app.py, PontoEntrada usa 'id', então o payload deve ser {"embedding": embedding_lista, "id": id_pessoa}
    # No entanto, o script cliente original usava "id_pessoa" no payload para inserir_api.
    # Vamos assumir que o PontoEntrada em app.py é o correto e usa "id".
    # O erro 404 não é sobre o payload de /inserir, mas sobre /construir-arvore.
    # A chave no payload para inserir_api deve corresponder ao modelo Pydantic PontoEntrada em app.py
    # class PontoEntrada(BaseModel):
    #     embedding: List[float] = Field(..., min_items=K_DIMENSIONS, max_items=K_DIMENSIONS)
    #     id: str = Field(..., max_length=99)
    # Então, o payload para /inserir deve ser: payload = {"embedding": embedding_lista, "id": id_pessoa}
    payload = {"embedding": embedding_lista, "id": id_pessoa}
    response = requests.post(f"{API_BASE_URL}/inserir", json=payload)
    response.raise_for_status()
    return response.json() # Retornar a resposta pode ser útil

def buscar_api(embedding_lista_query, id_query="query"):
    print(f"   Buscando vizinho mais próximo...")
    # O endpoint /buscar espera um PontoEntrada, que inclui 'id' e 'embedding'
    payload = {"embedding": embedding_lista_query, "id": id_query} 
    response = requests.post(f"{API_BASE_URL}/buscar", json=payload)
    response.raise_for_status()
    return response.json()

if __name__ == "__main__":
    try:
        # A chamada para construir_arvore_api() foi removida.
        print("-> Passo 1: KD-Tree é inicializada automaticamente pelo servidor FastAPI no startup.")
        print(f"\n-> Passo 2: Lendo arquivo JSON '{ARQUIVO_EMBEDDINGS}' e populando a árvore com embeddings...")

        if not os.path.exists(ARQUIVO_EMBEDDINGS):
            print(f"   ERRO CRÍTICO: Arquivo '{ARQUIVO_EMBEDDINGS}' não encontrado. Verifique o caminho.")
            exit()

        try:
            with open(ARQUIVO_EMBEDDINGS, 'r', encoding='utf-8') as f:
                todos_os_dados = json.load(f)
            print(f"   Arquivo '{ARQUIVO_EMBEDDINGS}' carregado com {len(todos_os_dados)} registros.")
        except json.JSONDecodeError as e:
            print(f"   ERRO CRÍTICO: Falha ao decodificar o JSON em '{ARQUIVO_EMBEDDINGS}'. Detalhes: {e}")
            exit()
        except Exception as e:
            print(f"   ERRO CRÍTICO: Ocorreu um erro ao ler o arquivo '{ARQUIVO_EMBEDDINGS}'. Detalhes: {e}")
            exit()

        sucessos_insercao = 0
        if not isinstance(todos_os_dados, list):
            print(f"   ERRO CRÍTICO: O conteúdo do JSON não é uma lista de registros.")
            exit()

        for i, item in enumerate(todos_os_dados):
            if not isinstance(item, dict) or 'person_id' not in item or 'embedding_vector' not in item:
                print(f"   AVISO: Registro {i+1} ignorado devido a formato inválido ou chaves ausentes ('person_id', 'embedding_vector'). Item: {item}")
                continue
            
            person_id = item['person_id']
            embedding_vector = item['embedding_vector']

            if not isinstance(embedding_vector, list) or not embedding_vector:
                print(f"   AVISO: Embedding para '{person_id}' (Registro {i+1}) está vazio ou não é uma lista. Ignorando.")
                continue

            if (i + 1) % 100 == 0 or (i + 1) == len(todos_os_dados) or i == 0:
                 print(f"   Inserindo registro {i + 1}/{len(todos_os_dados)}: {person_id}")
            try:
                # Passando person_id como o 'id' para a API
                resposta_insercao = inserir_api(embedding_vector, person_id)
                # print(f"      Resposta da inserção para {person_id}: {resposta_insercao}") # Opcional
                sucessos_insercao += 1
            except requests.exceptions.RequestException as e:
                print(f"   ERRO ao inserir '{person_id}' (Registro {i+1}): {e}. Resposta: {e.response.text if e.response else 'Sem resposta'}")
            except Exception as e:
                 print(f"   ERRO inesperado ao processar '{person_id}' (Registro {i+1}): {e}")

        print(f"   População concluída. {sucessos_insercao}/{len(todos_os_dados)} registros processados para inserção.")

        print("\n-> Passo 3: Verificando se a API reconhece as faces conhecidas...")

        dados_para_teste = {}
        for item in todos_os_dados:
            if isinstance(item, dict) and item.get('person_id') in IDS_PARA_TESTE and isinstance(item.get('embedding_vector'), list) and item.get('embedding_vector'):
                dados_para_teste[item['person_id']] = item['embedding_vector']

        if not dados_para_teste:
            print(f"   AVISO: Nenhum dos IDs em {IDS_PARA_TESTE} foi encontrado no arquivo JSON '{ARQUIVO_EMBEDDINGS}' com embeddings válidos ou os IDs não correspondem a 'person_id'.")
        else:
            for id_real, embedding_query in dados_para_teste.items():
                print("-" * 30)
                print(f"   TESTANDO PARA: '{id_real}'")
                try:
                    # Passando id_real também como o 'id' para a query de busca
                    resultado_busca = buscar_api(embedding_query, id_query=id_real)
                    id_encontrado = resultado_busca.get("id") # O PontoResultado no servidor retorna "id"
                    distancia = resultado_busca.get("distancia") # Assumindo que a API de busca retorna distância

                    if id_encontrado == id_real:
                        print(f"   SUCESSO! A API encontrou a pessoa correta: '{id_encontrado}'")
                        if distancia is not None:
                            print(f"     (Distância: {distancia:.4f})")
                    else:
                        print(f"   FALHA! A API encontrou '{id_encontrado}', mas o esperado era '{id_real}'.")
                        if distancia is not None:
                            print(f"     (Distância: {distancia:.4f})")

                except requests.exceptions.RequestException as e:
                    print(f"   ERRO ao buscar por '{id_real}': {e}. Resposta: {e.response.text if e.response else 'Sem resposta'}")
                except Exception as e:
                    print(f"   ERRO inesperado ao buscar por '{id_real}': {e}")

        print("\n--- FIM DO PROCESSO ---")

    except requests.exceptions.ConnectionError:
        print(f"ERRO DE CONEXÃO: Não foi possível conectar à API em {API_BASE_URL}. Verifique se o servidor Uvicorn (app.py) está rodando.")
    except Exception as e:
        print(f"Ocorreu um erro inesperado no script cliente: {e}")
