import numpy as np
import math
from py_aff3ct import Py_Module
import copy

class Modulator(Py_Module):
	def modulate(self, b, x):
		x[:] = self.isqrt2 - self.sqrt2 * b[:]
		return 0

	def __init__(self, N):
		Py_Module.__init__(self)
		t_mod = self.create_task("modulate")
		sb = self.create_socket_in (t_mod, "b", N, np.int32)
		sx = self.create_socket_out(t_mod, "x", N, np.float32)
		self.create_codelet(t_mod, lambda m,l,f: m.modulate(l[0], l[1]))
		self.sqrt2 = math.sqrt(2)
		self.isqrt2 = 1/math.sqrt(2)
