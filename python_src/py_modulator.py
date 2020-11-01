#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import math
import threading
import time
from py_aff3ct_module import Aff3ctModule
class Modulator(Aff3ctModule):
	def modulate(self, b, x):
		x[:] = self.is2 - self.s2 * b[:]
		return 0

	def __init__(self, N, n_frames=1):
		self.s2  = math.sqrt(2.0)
		self.is2 = 1/self.s2

		Aff3ctModule.__init__(self, n_frames)
		t_mod = self.create_task(self.modulate)
		self.create_socket_in (t_mod, "b", N, "int32")
		self.create_socket_out(t_mod, "x", N, "float")