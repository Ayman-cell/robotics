# Script pour pousser le projet Mars Rover vers GitHub
# A executer depuis le dossier G8_P4_S4 (celui qui contient "mars rover")

$ErrorActionPreference = "Stop"
$repoUrl = "https://github.com/Ayman-cell/mars-rover.git"

Write-Host "=== Push Mars Rover vers GitHub ===" -ForegroundColor Cyan
Write-Host ""

if (-not (Test-Path "mars rover")) {
    Write-Host "ERREUR: Le dossier 'mars rover' est introuvable. Lance ce script depuis G8_P4_S4." -ForegroundColor Red
    exit 1
}

if (-not (Test-Path ".git")) {
    Write-Host "Initialisation du depot Git..." -ForegroundColor Yellow
    git init
}
else {
    Write-Host "Depot Git deja initialise." -ForegroundColor Green
}

Write-Host "Ajout des fichiers (mars rover + .gitignore)..." -ForegroundColor Yellow
git add "mars rover"
if (Test-Path ".gitignore") { git add ".gitignore" }
git status

Write-Host ""
Write-Host "Commit..." -ForegroundColor Yellow
git commit -m "Mars Rover: Arduino + Streamlit + QR detection YOLO"

Write-Host ""
Write-Host "Branche main..." -ForegroundColor Yellow
git branch -M main

if (-not (git remote get-url origin 2>$null)) {
    Write-Host "Ajout du remote origin: $repoUrl" -ForegroundColor Yellow
    git remote add origin $repoUrl
}
else {
    Write-Host "Remote origin deja configure: $(git remote get-url origin)" -ForegroundColor Green
}

Write-Host ""
Write-Host "Push vers GitHub..." -ForegroundColor Yellow
git push -u origin main

Write-Host ""
Write-Host "Termine. Verifie sur https://github.com/Ayman-cell/mars-rover" -ForegroundColor Green
