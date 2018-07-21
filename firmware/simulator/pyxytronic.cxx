/*
 * Xytronic LF-1600
 * Open Source firmware
 * Simulator Python interface
 *
 * Copyright (c) 2018 Michael Buesch <m@bues.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "simulator.h"

extern "C" {

#include <Python.h>


static PyObject * xy_simulator_setting_write(PyObject *self, PyObject *args)
{
	int value = 0;
	char *name = NULL;
	bool ok;

	if (!PyArg_ParseTuple(args, "si", &name, &value))
		return NULL;

	ok = simulator_setting_access(name, &value, true);
	if (!ok) {
		PyErr_SetString(PyExc_RuntimeError, "simulator_settings_access() failed");
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject * xy_simulator_setting_read(PyObject *self, PyObject *args)
{
	int value = 0;
	char *name = NULL;
	bool ok;
	PyObject *retValue;

	if (!PyArg_ParseTuple(args, "s", &name))
		return NULL;

	ok = simulator_setting_access(name, &value, false);
	if (!ok) {
		PyErr_SetString(PyExc_RuntimeError, "simulator_settings_access() failed");
		return NULL;
	}
	retValue = Py_BuildValue("i", value);

	return retValue;
}


static PyObject * xy_simulator_adc_set(PyObject *self, PyObject *args)
{
	int adc_index;
	unsigned int adc_value;
	bool ok;

	if (!PyArg_ParseTuple(args, "iI", &adc_index, &adc_value))
		return NULL;

	ok = simulator_adc_set(adc_index, std::min(adc_value, (unsigned int)UINT16_MAX));
	if (!ok) {
		PyErr_SetString(PyExc_RuntimeError, "simulator_adc_set() failed");
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject * xy_simulator_pwm_get(PyObject *self, PyObject *args)
{
	int pwm_index;
	uint16_t value, max_value;
	bool ok;
	PyObject *retTuple;

	if (!PyArg_ParseTuple(args, "i", &pwm_index))
		return NULL;

	ok = simulator_pwm_get(pwm_index, &value, &max_value);
	if (!ok) {
		PyErr_SetString(PyExc_RuntimeError, "simulator_pwm_get() failed");
		return NULL;
	}
	retTuple = Py_BuildValue("II", (unsigned int)value, (unsigned int)max_value);

	return retTuple;
}

static PyObject * xy_simulator_uart_get_tx(PyObject *self, PyObject *args)
{
	uint8_t buf[4096];
	size_t count;
	PyObject *bytes;

	count = simulator_uart_get_tx(buf, sizeof(buf));
	bytes = PyBytes_FromStringAndSize((char *)buf, count);

	return bytes;
}

static PyObject * xy_simulator_mainloop_once(PyObject *self, PyObject *args)
{
	simulator_mainloop_once();
	Py_RETURN_NONE;
}

static PyObject * xy_simulator_exit(PyObject *self, PyObject *args)
{
	simulator_exit();
	Py_RETURN_NONE;
}

static PyObject * xy_simulator_init(PyObject *self, PyObject *args)
{
	simulator_init();
	Py_RETURN_NONE;
}

static void xy_free(void *arg)
{
	simulator_exit();
}

static PyMethodDef xy_methods[] = {
	{ "simulator_setting_write", xy_simulator_setting_write, METH_VARARGS, "", },
	{ "simulator_setting_read", xy_simulator_setting_read, METH_VARARGS, "", },
	{ "simulator_adc_set", xy_simulator_adc_set, METH_VARARGS, "", },
	{ "simulator_pwm_get", xy_simulator_pwm_get, METH_VARARGS, "", },
	{ "simulator_uart_get_tx", xy_simulator_uart_get_tx, METH_NOARGS, "", },
	{ "simulator_mainloop_once", xy_simulator_mainloop_once, METH_NOARGS, "", },
	{ "simulator_exit", xy_simulator_exit, METH_NOARGS, "", },
	{ "simulator_init", xy_simulator_init, METH_NOARGS, "", },
	{ NULL, NULL, 0, NULL, },
};

static struct PyModuleDef xy_module = {
	PyModuleDef_HEAD_INIT,
	"pyxytronic",		/* m_name */
	NULL,			/* m_doc */
	-1,			/* m_size */
	xy_methods,		/* m_methods */
	NULL,			/* m_slots */
	NULL,			/* m_traverse */
	NULL,			/* m_clear */
	xy_free,		/* m_free */
};

PyMODINIT_FUNC PyInit_pyxytronic(void)
{
	PyObject *mod;

	mod = PyModule_Create(&xy_module);
	if (!mod)
		return NULL;

	return mod;
}

} /* extern "C" */
