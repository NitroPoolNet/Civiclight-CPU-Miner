@echo off
setlocal

rem Replace YOUR_CIVIC_ADDRESS before starting the miner.
rem Change au to eu or us if another NitroPool region is closer.

"%~dp0civiclight-miner.exe" ^
  -a civiclight ^
  -o stratum+tcp://au.nitropool.net:3032 ^
  -u YOUR_CIVIC_ADDRESS.worker ^
  -p x

echo.
echo Miner stopped. Press any key to close this window.
pause >nul
