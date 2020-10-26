#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import math
from py_aff3ct_module import Aff3ctModule
class Modulator(Aff3ctModule):
	def modulate(self, b):
		x = self.is2 - self.s2 * b
		return [x.astype(np.float32)]

	def __init__(self, N):
		self.s2  = math.sqrt(2.0)
		self.is2 = 1/self.s2

		Aff3ctModule.__init__(self)
		t_mod = self.create_task("modulate")
		self.create_socket_in (t_mod, "b", N, "int32")
		self.create_socket_out(t_mod, "x", N, "float")