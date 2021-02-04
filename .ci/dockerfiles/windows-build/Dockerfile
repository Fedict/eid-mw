FROM mcr.microsoft.com/windows:$WINDOWS_CONTAINER_VERSION
RUN powershell -command set-executionpolicy Bypass -Scope process -Force; \
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
RUN choco install -y "visualstudio2019buildtools"
RUN choco install -y "visualstudio2019-workload-vctools"
RUN choco install -y "visualstuiod2019-workload-manageddesktopbuildtools"
RUN choco install -y "wixtoolset"
RUN choco install -y "nsis"
