#include <iostream>
#include <curl/curl.h>
#include <windows.h>

const int IDC_SETTINGS_BTN = 1;
const int IDC_BRIDGES_EDIT = 2;

std::string g_bridges;
std::string filePath;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_CREATE:
    {
      // Create a "Settings" button in the main window
      HWND hSettingsBtn = CreateWindow(L"BUTTON", L"Settings", 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10, 100, 50, hwnd, (HMENU)IDC_SETTINGS_BTN, 
        GetModuleHandle(NULL), NULL);
      break;
    }
    case WM_COMMAND:
    {
      int wmId = LOWORD(wParam);
      switch (wmId)
      {
        case IDC_SETTINGS_BTN:
        {
          // Show the settings window when the "Settings" button is clicked
          DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS), hwnd, SettingsDlgProc);
          break;
        }
        default:
          return DefWindowProc(hwnd, uMsg, wParam, lParam);
      }
      break;
    }
    // ...
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SettingsDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_INITDIALOG:
    {
      // Create a "Bridges" edit control in the settings window
      HWND hBridgesEdit = CreateWindow(L"EDIT", L"", 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 
        10, 10, 200, 200, hwnd, (HMENU)IDC_BRIDGES_EDIT, 
        GetModuleHandle(NULL), NULL);
      break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
            case IDOK:
            {


              // Get the text from the edit control
                int len = GetWindowTextLength(hBridgesEdit);
                std::vector<char> buffer(len + 1);
                GetWindowText(hBridgesEdit, &buffer[0], len + 1);

                // Open the file for writing
                std::ofstream file(filePath);
                if (file.is_open())
                {
                    // Write the text from the edit control to the file
                    file << &buffer[0];
                    file.close();
                }

                EndDialog(hwnd, 0);
                break;
            }
            case IDCANCEL:
                EndDialog(hwnd, 0);
                break;
            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
  return 0;
}

void fetchPage(const std::string& url, std::string& response)
{
    CURL* curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set the bridges if specified
        if (!g_bridges.empty())
        {
        curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1L);
        curl_easy_setopt(curl, CURLOPT_PROXY, g_bridges.c_str());
        }

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"MainWndClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    RegisterClassEx(&wcex);

    // Create the main window
    HWND hMainWnd = CreateWindow(L"MainWndClass", L"Main Window", 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, 
        hInstance, NULL);

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    // Fetch the page from the onion site
    std::string response;
    fetchPage("http://someonionsite.onion", response);

    // Display the content in the main window
    SetWindowText(g_hMainWndEdit, response.c_str());

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}
