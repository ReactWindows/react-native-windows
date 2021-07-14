param(# Parameter help description
[Parameter(Mandatory=$true)]
[string]$PackagePath)

# First, you have to download the nuget package from the internal CI here (or use a newer one)
# pipeline:     https://microsoft.visualstudio.com/WinUI/_build?definitionId=38157
# package link: https://microsoft.visualstudio.com/_apis/resources/Containers/38370988/packaging?itemPath=packaging%2FMicrosoft.WinUI.3.0.0-development.200707.1-CI.nupkg
# (only works for authenticated Microsoft users)
#

if (!((Test-Path $PackagePath) -and $PackagePath.EndsWith('.nupkg'))) {
    throw "PackagePath should point to the .nupkg file"
}
if (!(Get-Command nuget -ErrorAction SilentlyContinue)) {
    throw "Couldn't find nuget.exe"
}



$nugetPathToSet = Split-Path $PackagePath
nuget sources remove -Name WinUI 2>&1 | Out-Null
nuget sources add -Name WinUI -Source $nugetPathToSet


$rnwDir = Resolve-Path $PSScriptRoot\..\..
$from = '3.0.0-preview2.200713.0';
#   $to = '3.0.0-development.200706.1-CI';
$to = (Split-Path $PackagePath -Leaf).Replace('Microsoft.WinUI.', '').Replace('.nupkg', '')


# Convert WinUI from 3.0.0-preview2.200713.0 to 3.0.0-development.200706.1-CI
$filesToUpdate = @(
    "$rnwDir\vnext\PropertySheets\WinUI.props",
);

foreach ($filePath in $filesToUpdate) {
    Write-Host $filePath
    $content = (Get-Content $filePath) -replace $from, $to
    $content | Out-File $filePath
}

$env:UseWinUI3='true'
$env:UseExperimentalIXP='true'
& "C:\Program Files (x86)\Microsoft Visual Studio\2019\Preview\Common7\IDE\devenv.exe" $PSScriptRoot\windows\playground-win32-packaged.sln


