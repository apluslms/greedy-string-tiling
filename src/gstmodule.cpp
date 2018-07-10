#include "gst.hpp"
// Enforce internal, signed size-type over unsigned size_t
// https://www.python.org/dev/peps/pep-0353
#define PY_SSIZE_T_CLEAN
#include "Python.h"

#define GSTMODULE_DOCSTRING "This module implements a pattern matching function for str and bytes objects."

#define GST_MATCH_DOCSTRING "Takes 5 arguments: pattern (ascii str/bytes), pattern_marks (ascii (1 or 0) str/bytes), text (ascii str/bytes), text_marks (ascii (1 or 0) str/bytes), minimum_match_length (uint)"

static PyObject* MatchError;

/*
 * Corresponding Python function definition
 * def gst.match(pattern: str/bytes, pattern_marks: str/bytes, text: str/bytes, text_marks: str/bytes, minimum_match_length: uint):
 *     #stuff
 *     return [(pattern_begin, text_begin, match_length) for ... in matches]
 */
static PyObject*
gst_match(PyObject* self, PyObject* args)
{
    const char* pattern_c_str;
    Py_ssize_t pattern_length;

    const char* pattern_marks;
    Py_ssize_t pattern_marks_length;

    const char* text_c_str;
    Py_ssize_t text_length;

    const char* text_marks;
    Py_ssize_t text_marks_length;

    unsigned long minimum_match_length;

    if (!PyArg_ParseTuple(args, "s#s#s#s#k",
            &pattern_c_str, &pattern_length,
            &pattern_marks, &pattern_marks_length,
            &text_c_str, &text_length,
            &text_marks, &text_marks_length,
            &minimum_match_length)) {
        PyErr_SetString(MatchError, "Invalid arguments, please see docstring");
        return (PyObject*)NULL;
    }

    // It is impossible to find a match in a text that is shorter than the minimum match length
    if (text_length < (Py_ssize_t)minimum_match_length) {
        return PyList_New(0); // Return an empty list
    }

    // TODO: replace with c++17 string_view to avoid copying the const char*
    const std::string pattern(pattern_c_str, pattern_length);
    const std::string text(text_c_str, text_length);

    const std::string pattern_marks_str(pattern_marks, pattern_marks_length);
    const std::string text_marks_str(text_marks, text_marks_length);

    const auto& matches = match_strings(pattern, text, minimum_match_length, pattern_marks_str, text_marks_str);

    // Build a list of 3-tuples from matches and return it

    PyObject* py_list_matches;
    Py_ssize_t py_matches_len = (Py_ssize_t)matches.size();
    // Note that on successful creation, py_list_matches owns one reference to the new list
    py_list_matches = PyList_New(py_matches_len);
    if (py_list_matches == (PyObject*)NULL) {
        // Could not create list, exit with errors
        return (PyObject*)NULL;
    }

    Py_ssize_t i = 0;
    for (const auto& match : matches) {
        // Build a Python 3-tuple from a Match object, which consists of 3 unsigned longs
        PyObject* py_tuple_match = Py_BuildValue("(kkk)",
                match.pattern_index,
                match.text_index,
                match.match_length);
        if (py_tuple_match == (PyObject*)NULL) {
            // Unable to build tuple, release reference to the list and exit with errors
            Py_DECREF(py_list_matches);
            return (PyObject*)NULL;
        }
        PyList_SET_ITEM(py_list_matches, i++, py_tuple_match);
    }

    return py_list_matches;
}


// Define the Python module

static PyMethodDef module_methods[] = {
    {"match", gst_match, METH_VARARGS, GST_MATCH_DOCSTRING},
    {NULL, NULL, 0, NULL} // Sentinel
};

static struct PyModuleDef module_definition = {
    PyModuleDef_HEAD_INIT,      // Boilerplate
    "gst",                      // Module name, as in e.g. 'python3 -c "import gst"'
    GSTMODULE_DOCSTRING,
    -1,                         // Module has no support for sub-interpreters due to global state
    module_methods              // Module method table
};

PyMODINIT_FUNC
PyInit_gst(void)
{
    PyObject* module;
    module = PyModule_Create(&module_definition);
    if (module == NULL)
        return NULL;

    // Add a new Exception class to the gst module
    MatchError = PyErr_NewException("gst.MatchError", NULL, NULL);
    // Increment the reference counter to avoid a dangling pointer
    // in case someone manages to delete the class from the module at runtime
    Py_INCREF(MatchError);
    PyModule_AddObject(module, "MatchError", MatchError);

    return module;
}
