#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 22 10:42:15 2020

@author: rtajan
"""
import numpy as np
from py_aff3ct_module import Aff3ctModule
class Py_Module(Aff3ctModule):
	def modulate(self, b):
		x = 1.0 - 2.0*b.astype(np.float32)
		return [x]

	def __init__(self, N):
		Aff3ctModule.__init__(self)
		self.set_name("Py_Module")
		self.set_short_name("Py_Module")

		t_mod = self.create_task("modulate")
		self.create_socket_in (t_mod, "b", N, "B")
		self.create_socket_out(t_mod, "x", N, "R")