# Rogue-C

## Общие зависимости для сборки приложения и тестов

1. json-c (хранение данных)
2. ncurses (для управления терминалом)
3. check (для модульного тестирования)

### Сборка под Linux/MacOS на примере Ubuntu

Необходимо прописать следующие команды:
1. sudo apt install check
2. sudo apt install libjson-c-dev
3. sudo apt install libncurses5-dev libncursesw5-dev

Если check был установлен иным образом, возможно, будет необходимо удалить в Makefile_Unix
зависимость от библиотек -lpthread -lrt -lsubunit для сборки тестов

## Цели Makefile

0. По умолчанию собирается rogue.exe без указания релизная это сборка или дебажная, создаётя файл build/rogue.exe
1. release -- релизная сборка приложения, создаётя файл build/rogue.exe
2. debug -- сборка приложения с отладочной информацией, создаётя файл build/rogue.exe
3. unit_tests -- сборка модульных тестов, создаётя файл build/unit_tests.exe
4. clean -- удаление исполняемых файлов, а также промежуточных объектных файлов для их получения
