from ctypes import Structure, c_float, c_char, POINTER, CDLL, c_int, byref, c_void_p


K_DIMENSIONS = 128 
class TReg(Structure):
    _fields_ = [("embedding", c_float * K_DIMENSIONS),
                ("id", c_char * 100)]


class Tarv(Structure):

    _fields_ = [("raiz", c_void_p), ]



lib = CDLL("./libkdtree.dll") 

lib.kdtree_construir.argtypes = []
lib.kdtree_construir.restype = None


lib.inserir_ponto.argtypes = [TReg] 
lib.inserir_ponto.restype = None

lib.get_tree.argtypes = []
lib.get_tree.restype = POINTER(Tarv)


lib.buscar_mais_proximo.argtypes = [POINTER(Tarv), POINTER(TReg)]
lib.buscar_mais_proximo.restype = POINTER(TReg)


lib.kdtree_buscar_n_vizinhos.argtypes = [POINTER(Tarv), POINTER(TReg), c_int, POINTER(TReg)]
lib.kdtree_buscar_n_vizinhos.restype = c_int 