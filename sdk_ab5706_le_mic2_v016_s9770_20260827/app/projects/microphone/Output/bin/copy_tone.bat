@echo off
cd /d %~dp0
set tone_dir=..\bin\res_tone
set res_dir=..\bin\res
set tone_zh_dir=%tone_dir%\zh
set tone_en_dir=%tone_dir%\en
set tone_eq_dir=%tone_dir%\eq
set res_zh_dir=%res_dir%\zh
set res_en_dir=%res_dir%\en
set res_en_dir=%res_dir%\en
set res_eq_dir=%res_dir%\eq
if exist "%res_dir%" (
echo del /s %res_dir%\*.mp3
echo del /s %res_dir%\*.sbc
del /s %res_dir%\*.mp3 > NUL
del /s %res_dir%\*.sbc > NUL
) else (
mkdir %res_dir%
mkdir %res_zh_dir%
mkdir %res_en_dir%
)
if exist "%res_eq_dir%" (
echo del /s %res_eq_dir%\*.eq
echo del /s %res_eq_dir%\*.drc
del /s %res_eq_dir%\*.eq > NUL
del /s %res_eq_dir%\*.drc > NUL
) else (
mkdir %res_eq_dir%
)
echo copy %tone_dir% file to %res_dir%
copy /y %tone_dir%\ring.mp3 %res_dir%\ring.mp3 > NUL
copy /y %tone_dir%\update.mp3 %res_dir%\update.mp3 > NUL
copy /y %tone_dir%\update_done.mp3 %res_dir%\update_done.mp3 > NUL
copy /y %tone_en_dir%\language.sbc %res_en_dir%\language.sbc > NUL
copy /y %tone_zh_dir%\language.sbc %res_zh_dir%\language.sbc > NUL
copy /y %tone_en_dir%\wl_key_tone.sbc %res_en_dir%\wl_key_tone.sbc > NUL
copy /y %tone_zh_dir%\wl_key_tone.sbc %res_zh_dir%\wl_key_tone.sbc > NUL
