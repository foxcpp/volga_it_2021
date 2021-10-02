# Volga IT :: Визуализация тректории

## Инструкции по сборке

### Зависимости

* CMake 3.21.1
* Qt 5.2.0
* Qt Charts 5.2.0
* MinGW 7.3.0

### Инструкции для Windows

1. Установить Qt SDK

https://www.qt.io/download-qt-installer

Выбрать пользовательскую установку и следующие компоненты:
* Qt -> Qt 5.12.11 -> MinGW 7.3.0 64-bit
* Qt -> Qt 5.12.11 -> Qt Charts
* Qt -> Developer and Designer tools -> MinGW 7.3.0 64-bit
* Qt -> Developer and Designer tools -> CMake 3.21.1 64-bit

2. Запустить CMD с переменными среды для MinGW + Qt

Искать "Qt 5.12.11 (MinGW 7.3.0 64-bit)" в меню Пуск.

3. Перейти в директорию проекта.

```
mkdir build
cd build
C:\Qt\Tools\CMake_64\bin\cmake.exe -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release ..
C:\Qt\Tools\CMake_64\bin\cmake.exe --build .
```

Полученный файл `volga_it.exe` может быть запущенной из той же оболочки.

Для того, чтобы исполняемый файл можно было запустить независимо от Qt SDK, 
необходимо скопировать все необходимые библиотеки в директорию с файлом.
windeployqt может использоваться для этого:
```
windeployqt --release volga_it.exe
```



