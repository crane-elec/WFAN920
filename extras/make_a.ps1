# Spresense用ツールチェインのパスを取得
$toolPath = "$env:USERPROFILE\AppData\Local\Arduino15\packages\SPRESENSE\tools\gcc-arm-none-eabi\9.2.1\windows\bin"
$env:PATH = "$toolPath;$env:PATH"

# .oファイルのパス
$objectFile = "..\\.build\\libraries\\WFAN920\\libWFAN920_cmd.cpp.o"
# .aファイルの出力先
$archiveFile = "..\\src\\libWFAN920_cmd.a"

# .oファイルが存在するかチェック
if (Test-Path $objectFile) {
    Write-Host "Creating static library: $archiveFile"
    arm-none-eabi-ar rcs $archiveFile $objectFile
    Write-Host "Done."
} else {
    Write-Host "Error: $objectFile not found."
}
