@echo off
setlocal enabledelayedexpansion

echo [*] Iniciando setup do ambiente de desenvolvimento...
echo.

net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [!] Este script precisa ser executado como Administrador
    echo [!] Por favor, execute novamente com privilegios administrativos
    pause
    exit /b 1
)

:: Verifica Visual Studio
echo [*] Verificando instalacao do Visual Studio...
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\14.0" >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [!] Visual Studio nao encontrado
    echo [*] Baixando Visual Studio Community...
    powershell -Command "& { Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vs_community.exe' -OutFile 'vs_community.exe' }"
    echo [*] Instalando Visual Studio com componentes necessarios...
    start /wait vs_community.exe --add Microsoft.VisualStudio.Workload.NativeDesktop --add Microsoft.VisualStudio.Component.Windows10SDK.19041 --quiet
)

:: Verifica Windows SDK
echo [*] Verificando Windows SDK...
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Microsoft SDKs\Windows\v10.0" >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [!] Windows SDK nao encontrado
    echo [*] Baixando Windows SDK...
    powershell -Command "& { Invoke-WebRequest -Uri 'https://go.microsoft.com/fwlink/p/?LinkID=2164675' -OutFile 'winsdksetup.exe' }"
    echo [*] Instalando Windows SDK...
    start /wait winsdksetup.exe /features + /quiet
)

:: Instalação do Visual C++ Redistributable
echo [*] Instalando Visual C++ Redistributable...
powershell -Command "& { Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vc_redist.x64.exe' -OutFile 'vc_redist.x64.exe' }"
start /wait vc_redist.x64.exe /install /quiet /norestart

echo [*] Configurando variaveis de ambiente...
setx VCPKG_DEFAULT_TRIPLET x64-windows
setx VCPKG_ROOT "C:\vcpkg"

echo [*] Limpando arquivos temporarios...
if exist "vs_community.exe" del "vs_community.exe"
if exist "winsdksetup.exe" del "winsdksetup.exe"
if exist "vc_redist.x64.exe" del "vc_redist.x64.exe"

echo.
echo [+] Setup concluido! O ambiente esta pronto para desenvolvimento.
echo [*] Recomenda-se reiniciar o computador antes de comecar.
echo.

set /p reiniciar="Deseja reiniciar o computador agora? (S/N): "
if /i "%reiniciar%"=="S" (
    shutdown /r /t 10 /c "Reiniciando para concluir a instalacao..."
    echo Reiniciando em 10 segundos...
)

pause
endlocal