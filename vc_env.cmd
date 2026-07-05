where cl > nul: 2>&1
if %errorlevel% == 0 exit /b 0
for /f "delims=" %%i in ('vswhere -all -latest -products * -prerelease -property installationPath') do set _p=%%i
set _x=%1
if "%_x%" == "" set _x=x64
shift
if not "%_p%" == "" call "%_p%\VC\Auxiliary\Build\vcvarsall.bat" %_x% %*
