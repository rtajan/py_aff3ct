#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 22 10:42:15 2020

@author: rtajan
"""
import numpy as np
from py_aff3ct_module import Aff3ctModule
class Py_Module(Aff3ctModule):
	def print(self, x):
		print(x)

	def add_1(self, x):
		y = x + 1
		z = x + 2
		t = x + 3
		return y, z, t

	def __init__(self, N):
		Aff3ctModule.__init__(self)
		self.set_name("Py_Module")
		self.set_short_name("Py_Module")

		t_print = self.create_task("print")
		self.create_socket_in (t_print, "x", N, "B")

		t_add_1 = self.create_task("add_1")
		self.create_socket_in  (t_add_1, "x", N, "B")
		self.create_socket_out (t_add_1, "y", N, "B")
		self.create_socket_out (t_add_1, "z", N, "R")
		self.create_socket_out (t_add_1, "t", N, "R")