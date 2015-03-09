
#include <windows.h>

#include "bstrlib.h"
#include "resource.h"

#define IDC_TIMER 100

LRESULT CALLBACK TimeTrayProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
   HDC hdc;
   PAINTSTRUCT sPaint;
   HGDIOBJ hfDefault;
   HWND hWndButton;
   SYSTEMTIME sTime;
   bstring sTimeString;

   switch( msg ) {
      case WM_DESTROY:
         PostQuitMessage( 0 );
         return 0;

      case WM_CREATE:
         break;

      case WM_TIMER:

         /* Get the current local time. */
         GetLocalTime( &sTime );
         
         /* Format the time into a displayable string and set the window title. */
         sTimeString = bformat( "%d:%02d:%02d", sTime.wHour % 12, sTime.wMinute, sTime.wSecond );
         SetWindowText( hWnd, bdata( sTimeString ) );
         bdestroy( sTimeString );
         break;

      case WM_SYSCOMMAND:
         /* Never restore from minimization. */
         if( SC_RESTORE == wParam ) {
            return 0;
         }
         break;
   }

cleanup:

   return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nShowCmd ) {
   WNDCLASSEX winclass;
   HBRUSH hBrush;
   INT nResult = 0;
   INT nRetVal = 0;
   HWND hWnd;
   MSG msg;
   
   ZeroMemory( &winclass, sizeof( WNDCLASSEX ) );
   ZeroMemory( &msg, sizeof( MSG ) );

   winclass.cbSize = sizeof( WNDCLASSEX );
   winclass.hbrBackground = GetStockObject( BLACK_BRUSH );
   winclass.hCursor = LoadCursor( NULL, IDC_ARROW );
   winclass.hInstance = hInst;
   winclass.lpfnWndProc = (WNDPROC)TimeTrayProc;
   winclass.lpszClassName = "TimeTrayClass";
   winclass.hIcon = LoadIcon( hInst, MAKEINTRESOURCE( IDI_CLOCK ) );

   /* Create a simple window and display it minimized. */

   nResult = RegisterClassEx( &winclass );
   if( !nResult ) {
      /* TODO: Display error. */
      GetLastError();
      MessageBox( NULL, "Error registering window class.", "Error", MB_ICONERROR | MB_OK );
      nRetVal = 1;
      goto cleanup;
   }

   hWnd = CreateWindowEx(
      NULL, "TimeTrayClass", "TimeTray",
      WS_OVERLAPPED | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
      0, 0, 100, 100, NULL, NULL, hInst, NULL
   );

   SetTimer( hWnd, IDC_TIMER, 1000, TimeTrayProc );

   ShowWindow( hWnd, SW_SHOWMINIMIZED );
   
   /* Message loop. */
   while( 0 != GetMessage( &msg, NULL, 0, 0 ) ) {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
   }

cleanup:

   return nRetVal;
}
