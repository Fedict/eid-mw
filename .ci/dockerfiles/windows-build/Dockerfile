FROM mcr.microsoft.com/windows:$WINDOWS_CONTAINER_VERSION
RUN powershell -command set-executionpolicy Bypass -Scope process -Force; \
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
COPY packages.config "C:\\packages.config"
RUN choco install -y "C:\\packages.config"
