FROM mcr.microsoft.com/windows:1903
RUN powershell -command set-executionpolicy Bypass -Scope process -Force; \
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
RUN choco install -y "visualstudio2019buildtools"
RUN choco install -y "visualstudio2019-workload-vctools"
RUN choco install -y "visualstuiod2019-workload-manageddesktopbuildtools"
RUN choco install -y "nsis"
# DotNet 3.5 doesn't install properly through chocolatey, and wix requires it,
# so...
#RUN choco install -y "wixtoolset"
RUN powershell -command Add-WindowsCapability -Name Net-Framework-Core -Online
RUN powershell -command Invoke-WebRequest https://github.com/wixtoolset/wix3/releases/download/wix3112rtm/wix311.exe -OutFile C:\\wix311.exe; \
    C:\\wix311.exe /install /quiet /norestart
