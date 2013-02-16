// Unicode support by Jim Park -- 08/02/2007

#include <windows.h>
#include "pluginapi.h" // nsis plugin
#include "extractor.h"

HINSTANCE g_hInstance;
HWND g_hwndParent;

using namespace omaha;

extern "C" void __declspec(dllexport) extract(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop)
{
  g_hwndParent = hwndParent;

  EXDLL_INIT();

  TCHAR fileName[MAX_PATH];
  GetModuleFileName(NULL, fileName, sizeof(fileName));

  TagExtractor ext;
  if (!ext.OpenFile(fileName))
    return;

  int len = 0;
  if (!ext.ExtractTag(NULL, &len))
    return;

  char *buffer = new char[len];
  if (!ext.ExtractTag(buffer, &len))
    return;

  popstring(fileName);

  HANDLE out = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (out != INVALID_HANDLE_VALUE) {
    DWORD wrote = 0;
    WriteFile(out, buffer, len - 1, &wrote, NULL);
    CloseHandle(out);
  }

  delete []buffer;
}


BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
  g_hInstance = (HINSTANCE) hInst;
  return TRUE;
}
