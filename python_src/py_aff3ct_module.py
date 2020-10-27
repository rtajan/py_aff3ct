#!/usr/bin/env python3
# -*- coding: utf-8 -*-

class Aff3ctModule:
	def __init__(self, n_frames = 1):
		self.n_frames = n_frames
		self.name = self.__module__ + '.' + self.__class__.__name__
		self.short_name = self.__class__.__name__
		self.task_list = []

	def set_name(self, name):
		self.name = name

	def set_short_name(self, short_name):
		self.short_name = short_name

	def create_task(self, codelet, task_name=""):
		the_task = Task(codelet, task_name)
		self.task_list.append(the_task)
		return the_task

	def create_socket_in(self, task, name, n_elmts, data_type):
		task.create_socket_in(name, n_elmts, data_type)

	def create_socket_out(self, task, name, n_elmts, data_type):
		task.create_socket_out(name, n_elmts, data_type)

	def get_task_list(self):
		return self.task_list

	def get_name(self):
		return self.name


class Task:
	def __init__(self, codelet, name=""):
		if name == "":
			self.name = codelet.__name__
		else:
			self.name = name

		self.codelet = codelet
		self.socket_in_list = []
		self.socket_out_list = []

	def create_socket_in(self, name, n_elmts, data_type):
		self.socket_in_list.append(Socket(name, n_elmts, data_type))

	def create_socket_out(self, name, n_elmts, data_type):
		self.socket_out_list.append(Socket(name, n_elmts, data_type))


class Socket:
	def __init__(self, name, n_elmts, data_type):
		self.name = name
		self.n_elmts = n_elmts
		self.data_type = data_type


