#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.
#NoTrayIcon
#SingleInstance Force

; change these paths

F20::
Run, C:\path\to\python.exe "C:\path\to\qmk_spot.py" "toggle", , Hide
return

!F20::
Run, C:\path\to\python.exe "C:\path\to\qmk_spot.py" "up", , Hide
return

^F20::
Run, C:\path\to\python.exe "C:\path\to\qmk_spot.py" "down", , Hide
return

+^F20::
Run, C:\path\to\python.exe "C:\path\to\qmk_spot.py" "shuffle", , Hide
return
