# nlohmann/json 단일 헤더 파일 다운로드 스크립트
# PowerShell에서 실행: .\setup_nlohmann.ps1

$version  = "3.11.3"
$url      = "https://github.com/nlohmann/json/releases/download/v$version/json.hpp"
$dest_dir = "$PSScriptRoot\json_library_poc\include\nlohmann"
$dest     = "$dest_dir\json.hpp"

if (Test-Path $dest) {
    Write-Host "이미 존재합니다: $dest" -ForegroundColor Yellow
    exit 0
}

Write-Host "nlohmann/json v$version 다운로드 중..." -ForegroundColor Cyan
New-Item -ItemType Directory -Force -Path $dest_dir | Out-Null

try {
    Invoke-WebRequest -Uri $url -OutFile $dest -UseBasicParsing
    Write-Host "완료: $dest" -ForegroundColor Green
} catch {
    Write-Host "다운로드 실패: $_" -ForegroundColor Red
    exit 1
}
