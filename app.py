from fastapi import FastAPI, Query, HTTPException
from pydantic import BaseModel, Field
from typing import List

# Assuming kdtree_wrapper.py is in the same directory or Python path
from kdtree_wrapper import lib, Tarv, TReg, K_DIMENSIONS
from ctypes import POINTER, c_float, c_char, c_int, byref, create_string_buffer

app = FastAPI()

class PontoEntrada(BaseModel):
    embedding: List[float] = Field(..., min_items=K_DIMENSIONS, max_items=K_DIMENSIONS)
    id: str = Field(..., max_length=99) # Max 99 chars to allow for null terminator in c_char[100]

class PontoResultado(BaseModel):
    embedding: List[float]
    id: str

@app.on_event("startup")
async def startup_event():
    lib.kdtree_construir()
    print("KD-Tree initialized on startup.")

@app.post("/inserir")
def inserir(ponto: PontoEntrada):
    id_bytes = ponto.id.encode('utf-8')
    c_id_buffer = create_string_buffer(100)
    c_id_buffer.value = id_bytes

    embedding_array = (c_float * K_DIMENSIONS)(*ponto.embedding)
    novo_ponto_reg = TReg(embedding=embedding_array, id=c_id_buffer.raw[:100])

    lib.inserir_ponto(novo_ponto_reg)
    
    return {"mensagem": f"Ponto com ID '{ponto.id}' inserido com sucesso."}

@app.post("/buscar", response_model=PontoResultado)
def buscar(query: PontoEntrada):
    embedding_query_array = (c_float * K_DIMENSIONS)(*query.embedding)
    query_id_bytes = query.id.encode('utf-8')
    c_query_id_buffer = create_string_buffer(100)
    c_query_id_buffer.value = query_id_bytes

    query_reg = TReg(embedding=embedding_query_array, id=c_query_id_buffer.raw[:100])

    arv_ptr = lib.get_tree()
    if not arv_ptr or not arv_ptr.contents.raiz:
        pass

    resultado_ptr = lib.buscar_mais_proximo(arv_ptr, byref(query_reg))

    if not resultado_ptr:
        raise HTTPException(status_code=404, detail="Nenhum ponto encontrado.")

    resultado_obj = resultado_ptr.contents
    
    try:
        res_id = resultado_obj.id.decode('utf-8', errors='replace').rstrip('\x00')
    except Exception as e:
        res_id = f"Error decoding ID: {e}"

    return PontoResultado(
        embedding=list(resultado_obj.embedding),
        id=res_id
    )

@app.post("/buscar-n", response_model=List[PontoResultado])
def buscar_n_vizinhos(query: PontoEntrada, n: int = Query(..., gt=0, description="Número de vizinhos a buscar")):
    if n <= 0:
        raise HTTPException(status_code=400, detail="N deve ser um inteiro positivo.")

    embedding_query_array = (c_float * K_DIMENSIONS)(*query.embedding)
    query_id_bytes = query.id.encode('utf-8')
    c_query_id_buffer = create_string_buffer(100)
    c_query_id_buffer.value = query_id_bytes
    
    query_reg = TReg(embedding=embedding_query_array, id=c_query_id_buffer.raw[:100])

    arv_ptr = lib.get_tree()

    resultados_array_type = TReg * n
    resultados_c_array = resultados_array_type()

    num_encontrados = lib.kdtree_buscar_n_vizinhos(arv_ptr, byref(query_reg), n, resultados_c_array)

    if num_encontrados == 0:
        return []

    resultados_finais = []
    for i in range(num_encontrados):
        res_obj = resultados_c_array[i]
        try:
            res_id = res_obj.id.decode('utf-8', errors='replace').rstrip('\x00')
        except Exception as e:
            res_id = f"Error decoding ID for neighbor {i}: {e}"
        
        resultados_finais.append(
            PontoResultado(
                embedding=list(res_obj.embedding),
                id=res_id
            )
        )
    
    return resultados_finais
