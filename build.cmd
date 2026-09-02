cd "%~dp0"
call vc_env
cd CPP\7zip\Bundles\Alone2
rd /q/s x64
nmake PLATFORM=x64
