@echo off
REM    ============================================
REM    ˵����
REM          ʹ��doxygen����chm�ĵ��������ļ�
REM    ֧��eclipse����chm�ĵ�
REM    �������ڣ�2012-12-4
REM    �汾��v1.1
REM    ���ߣ�����
REM    ʱ�䣺2012-3-4
REM    ============================================
REM    1. ִ������
REM    ============================================
echo   1. �����ǰ���
if exist refman.chm del /f /q dproc.chm
if exist html del /f /q html\*.*
REM    ============================================
REM    2. ִ��ִ��doxygen�����ĵ�
REM    ============================================
echo   2. �����ĵ�
doxygen chm.doxyfile
REM    ============================================
REM    3. ��Doxygen����ļ������utf-8ת����gbk
REM    ============================================
echo   3. ��Doxygen����ļ������utf-8ת����gbk
set    path=%path%;%cd%
cd     html

echo   ����chm�����ļ�
REM    call utf82gbk.bat index.hhp
echo   ��index.hhp��utf-8����ת����gbk����
for    %%f in (index.hhp) do copy %%f %%f.utf8
for    %%f in (index.hhp) do iconv -c -f utf-8 -t gbk %%f.utf8 > %%f

REM    call utf82gbk.bat index.hhc
echo   ��index.hhc��utf-8����ת����gbk����
for    %%f in (index.hhc) do copy %%f %%f.utf8
for    %%f in (index.hhc) do iconv -c -f utf-8 -t gbk %%f.utf8 > %%f

REM    call utf82gbk.bat index.hhk
echo   ��index.hhp��utf-8����ת����gbk����
for    %%f in (index.hhk) do copy %%f %%f.utf8
for    %%f in (index.hhk) do iconv -c -f utf-8 -t gbk %%f.utf8 > %%f

REM    call html-utf82gbk *.html
echo   ��index.hhp��utf-8����ת����gbk����
for    %%f in (*.html) do copy %%f %%f.utf8
for    %%f in (*.html) do iconv -c -f utf-8 -t gbk %%f.utf8 > %%f
echo   ��*.html�е�charset��UTF-8��Ϊgb2312
fr     *.html -fic:charset=UTF-8 -t:charset=gb2312

REM    ============================================
REM    4. ʹ��hhc.exe����chm�ĵ�
REM    ============================================
REM    call makechm.bat
echo   4. ʹ��hhc.exe����chm�ĵ�
"C:\Program Files\HTML Help Workshop\hhc.exe" index.hhp

if exist index.chm copy index.chm ..\refman.chm
del /f /q *.chm
cd ..\
REM    ============================================
REM    5. rebuild����
REM    ============================================
if exist html del /f /q html
echo   5. rebuild����
pause
