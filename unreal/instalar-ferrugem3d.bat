@echo off
chcp 65001 >nul
title Instalador do Ferrugem 3D
echo.
echo  =============================================
echo     INSTALADOR DO FERRUGEM 3D  (Nexvora)
echo  =============================================
echo.
echo  IMPORTANTE: a Unreal precisa estar FECHADA.
echo.
pause

rem --- 1. Acha a pasta Documentos (funciona mesmo se ela estiver no OneDrive)
for /f "usebackq delims=" %%D in (`powershell -NoProfile -Command "[Environment]::GetFolderPath('MyDocuments')"`) do set "DOCS=%%D"
set "PROJ=%DOCS%\Unreal Projects\Ferrugem3D"
set "SRC=%PROJ%\Source\Ferrugem3D"

if not exist "%PROJ%\Ferrugem3D.uproject" (
  echo.
  echo  [ERRO] Nao achei o projeto em:
  echo  %PROJ%
  echo  Crie o projeto Ferrugem3D na Unreal 5.1 ^(Top Down, C++^) e rode de novo.
  echo.
  pause
  exit /b 1
)
echo.
echo  [1/4] Projeto encontrado: %PROJ%

rem --- 2. Baixa o codigo do GitHub
set "RAW=https://raw.githubusercontent.com/pedrootavioantoniazzi-dev/ferrugem/main/unreal/Source/Ferrugem3D"
echo  [2/4] Baixando o codigo do Ferrugem 3D...
powershell -NoProfile -Command "$ProgressPreference='SilentlyContinue'; try { Invoke-WebRequest '%RAW%/FerrugemJogo.h' -OutFile '%SRC%\FerrugemJogo.h' -UseBasicParsing; Invoke-WebRequest '%RAW%/FerrugemJogo.cpp' -OutFile '%SRC%\FerrugemJogo.cpp' -UseBasicParsing; Invoke-WebRequest '%RAW%/FerrugemDados.h' -OutFile '%SRC%\FerrugemDados.h' -UseBasicParsing } catch { Write-Host $_; exit 1 }"
if errorlevel 1 (
  echo.
  echo  [ERRO] Nao consegui baixar os arquivos. Confira a internet e tente de novo.
  pause
  exit /b 1
)
echo        OK: FerrugemJogo.h, FerrugemJogo.cpp e FerrugemDados.h copiados.

rem --- 3. Faz o projeto usar o modo de jogo do Ferrugem (FerGameMode)
echo  [3/4] Configurando o modo de jogo...
powershell -NoProfile -Command "$f='%PROJ%\Config\DefaultEngine.ini'; $t=Get-Content $f; if ($t -match '^GlobalDefaultGameMode=') { $t=$t -replace '^GlobalDefaultGameMode=.*','GlobalDefaultGameMode=/Script/Ferrugem3D.FerGameMode' } else { $t += '[/Script/EngineSettings.GameMapsSettings]'; $t += 'GlobalDefaultGameMode=/Script/Ferrugem3D.FerGameMode' }; Set-Content $f $t"

rem --- 4. Compila
set "UE=C:\Program Files\Epic Games\UE_5.1"
if not exist "%UE%\Engine\Build\BatchFiles\Build.bat" (
  echo.
  echo  [ERRO] Nao achei a Unreal 5.1 em: %UE%
  echo  Se ela esta instalada em outro lugar, me avise ^(Claude^).
  pause
  exit /b 1
)
echo  [4/4] Compilando... ^(pode levar alguns minutos, nao feche esta janela^)
echo.
call "%UE%\Engine\Build\BatchFiles\Build.bat" Ferrugem3DEditor Win64 Development -Project="%PROJ%\Ferrugem3D.uproject" -WaitMutex
if errorlevel 1 (
  echo.
  echo  =============================================
  echo   DEU ERRO NA COMPILACAO.
  echo   Tire um print das linhas com "error" acima
  echo   e mande para o Claude.
  echo  =============================================
  pause
  exit /b 1
)

echo.
echo  =============================================
echo   PRONTO! O Ferrugem 3D foi instalado.
echo   Abrindo a Unreal... depois aperte Play.
echo  =============================================
start "" "%PROJ%\Ferrugem3D.uproject"
pause
