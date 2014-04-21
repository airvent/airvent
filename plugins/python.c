#include <python/python.h>

int main(int argc, char **argv) {
  assert(argc>=2);

  Py_SetProgramName(argv[1]);
  Py_Initialize();
  PySys_SetArgv(argc-1, &argv[2]);

  // Get a reference to the main module.
  PyObject* main_module =  PyImport_AddModule("__main__");
  // Get the main module's dictionary
  PyObject* main_dict = PyModule_GetDict(main_module);
  FILE *script = fopen(argv[2], "r");
  PyRun_File(script, argv[2],
    Py_file_input,
    main_dict, main_dict);
  Py_Finalize();
}
