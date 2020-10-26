#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from py_aff3ct_module import Aff3ctModule
class Display(Aff3ctModule):
	def plot(self, x):
		if  self.i_plt % 50 == 0:
			self.line.set_data(x[::2], x[1::2])
			self.fig.canvas.draw()
			self.fig.canvas.flush_events()
			plt.pause(0.000000000001)

		self.i_plt = self.i_plt + 1
		return []

	def __init__(self, N):
		Aff3ctModule.__init__(self)
		t_plot = self.create_task("plot")
		self.create_socket_in (t_plot, "x", N, "float")

		self.fig   = plt.figure()
		self.ax    = self.fig.add_subplot(1, 1, 1)
		self.line, = self.ax.plot([], '.b')
		self.i_plt = 0
		plt.xlabel("Real part")
		plt.ylabel("Imaginary part")
		plt.ylim(-2,2)
		plt.xlim(-2,2)