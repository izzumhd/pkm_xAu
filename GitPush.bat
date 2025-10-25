@echo off
echo ===============================
echo   Update dan Push ke GitHub 
echo ===============================

cd /d "D:\P codeng\GitHub\pkm_xAu"

echo  Menarik update dari GitHub dulu...
git pull origin main

echo  Mengecek perubahan lokal...
git add .

rem cek apakah ada perubahan yang perlu di-commit
git diff --cached --quiet
if %errorlevel%==0 (
    echo  Tidak ada perubahan untuk di-commit.
) else (
    echo  Commit perubahan...
    git commit -m "update dari folder local pkm_xAu"

    echo  Push ke GitHub...
    git push origin main
)

echo.
echo  Selesai! Semua udah sinkron 
pause
