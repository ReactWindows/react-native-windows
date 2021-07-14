# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

#
# Start-TestServers.ps1
#
param (
	[System.IO.DirectoryInfo] $SourcesDirectory = ($PSScriptRoot | Split-Path | Split-Path),

	[switch] $Preload,

	[int] $SleepSeconds = 10,

	[switch] $UseNodeWsServer
)

Write-Host "Starting packager"

Set-Location -Path "$PSScriptRoot\..\.."

Start-Process npm	-PassThru `
				-NoNewWindow `
				-WorkingDirectory "$PSScriptRoot\..\.." `
				-ArgumentList "run start" `
				-OutVariable packagerProc

Write-Host 'Started packager'

if ($UseNodeWsServer.IsPresent) {
	Write-Host 'Starting WebSocket server'

	Start-Process	-PassThru `
					-FilePath (Get-Command node.exe).Definition `
					-ArgumentList "${SourcesDirectory}\IntegrationTests\websocket_integration_test_server.js" `
					-OutVariable wsProc
	
	Write-Host 'Started WebSocket server'
}

if ($Preload) {
	Write-Host 'Preloading bundles'

	Start-Sleep -Seconds $SleepSeconds

	# Preload the RNTesterApp integration bundle for better performance in integration tests.
	Invoke-WebRequest -UseBasicParsing -Uri "http://localhost:8081/IntegrationTests/IntegrationTestsApp.bundle?platform=windows&dev=true" | Out-Null

	Write-Host 'Done preloading bundles.'
}

# Use the environment variables input below to pass secret variables to this script.
Write-Host "##vso[task.setvariable variable=PackagerId;]$($packagerProc.Id)"

if ($UseNodeWsServer.IsPresent) {
	Write-Host "##vso[task.setvariable variable=WebSocketServerId;]$($wsProc.Id)"
	return ($packagerProc, $wsProc)
} else {
	return ($packagerProc)
}
